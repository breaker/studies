// main.cpp
//
// 测试多线程 Peterson 互斥算法
// 重点关注在支持指令乱序执行 (Out-of-Order Execution) 的现代 CPU 上,
// Peterson 算法是否有效
//

#include "pch.h"
#include "threadpool.h"

using std::vector;

////////////////////////////////////////////////////////////////////////////////
// Diagnostic Helpers
////////////////////////////////////////////////////////////////////////////////

// Print function name
#define PRINT_FUNC(fs)      fprintf(fs, "----- %s -----\n", __FUNCTION__)

#ifdef _DEBUG
#define dbg_fputs(str, fs)  fputs(str, fs)
#define dbg_getch()         _getch()
#else
#define dbg_fputs(str, fs)  ((void) 0)
#define dbg_getch()         ((void) 0)
#endif

////////////////////////////////////////////////////////////////////////////////
// Helpers
////////////////////////////////////////////////////////////////////////////////

// Generates a random integer in range [imin, imax)
errno_t randint(_Out_ int& rnd, int imin, int imax)
{
    if (imin > imax)
        return EINVAL;

    // rand_s() generates a cryptographically secure pseudorandom unsigned int in range [0, UINT_MAX]
    // rand_s() depends on RtlGenRandom(), and does not use srand() seed.
    unsigned rnd0;
    errno_t err = rand_s(&rnd0);
    if (err)
        return err;
    rnd = (int) ((double) rnd0 / UINT_MAX * (imax - imin) + imin);

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Globals
////////////////////////////////////////////////////////////////////////////////

const char PROGRAM_NAME[]   = "TestPeterson";

// 每个线程累加值的上下限
// 预期累加和 = 所有线程的累加值之和
const int NMIN  = 10000;
const int NMAX  = 30000;

// 多线程的累加和结果
int g_nSum;

// 测试回合
const int TEST_ROUND    = 50;

// 测试线程数
// 用于 [Baseline 测试] 和 [Peterson-N 测试]
// [Peterson-2 测试] 中的线程数固定为 2
// THREAD_NUM 的最大值为 MAXIMUM_WAIT_OBJECTS (64)
// 这是 class ThreadPool 支持的最大线程数
const int THREAD_NUM    = 4;

// 临界区对象
// 用于 [Baseline 测试]
CRITICAL_SECTION g_CSec;

// 用于临界区对象的自旋计数参数
// 最高位 (0x80) 是为了兼容 Win2000 的预分配临界区对象内部的 Event 对象
// WinXP 之后此 Event 对象采用按需分配方式, 最高位会被忽略
// 自旋计数 0x0400 = 1024
const DWORD CSEC_SPINCOUNT = 0x80000400;

////////////////////////////////////////////////////////////////////////////////
// Setup and Cleanup
////////////////////////////////////////////////////////////////////////////////

bool setup()
{
    setlocale(LC_ALL, "");
    return true;
}

void cleanup()
{
    dbg_fputs("Press any key to Exit...\n", stderr);
    dbg_getch();
}

////////////////////////////////////////////////////////////////////////////////
// Baseline Test
////////////////////////////////////////////////////////////////////////////////

// 使用系统临界区对象 (Critical Section) 进行多线程累加中的互斥
// 作为与 Peterson 算法的对比测试基线

DWORD WINAPI ThreadProcBaseline(LPVOID param)
{
    int count = (int) (intptr_t) param;

    for (int i = 0; i < count; ++i) {
        EnterCriticalSection(&g_CSec);
        ++g_nSum;
        LeaveCriticalSection(&g_CSec);
    }
    return 0;
}

bool StartBaseline(int threadNum)
{
    int expSum = 0;
    g_nSum = 0;

    // 初始化 ThreadPool 中的线程
    vector<LPVOID> threadParam(threadNum);
    for (int i = 0; i < threadNum; ++i) {
        int rnd;
        randint(rnd, NMIN, NMAX);
        expSum += rnd;
        threadParam[i] = (LPVOID) (intptr_t) rnd;
    }
    printf("Expected Sum: %d\n", expSum);

    ThreadPool threadPool(ThreadProcBaseline, &threadParam[0], threadNum);

    // 初始化临界区对象
    if (!InitializeCriticalSectionAndSpinCount(&g_CSec, CSEC_SPINCOUNT)) {
        fprintf(stderr, "InitializeCriticalSectionAndSpinCount() failed: %d\n", GetLastError());
        exit(-1);
    }

    // 启动 ThreadPool 中的线程
    if (!threadPool.StartThreads()) {
        fprintf(stderr, "ThreadPool::StartThreads() failed\n");
        exit(-1);
    }

    // 等待所有线程结束
    _ASSERTE(threadNum <= MAXIMUM_WAIT_OBJECTS);
    if (threadPool.WaitThreads(INFINITE) == WAIT_FAILED) {
        fprintf(stderr, "ThreadPool::WaitThreads() failed: %d\n", GetLastError());
        exit(-1);
    }

    DeleteCriticalSection(&g_CSec);

    printf("Result Sum: %d\n", g_nSum);
    return (g_nSum == expSum);
}

void TestBaseline(bool on)
{
    if (!on)
        return;
    PRINT_FUNC(stderr);

    printf("Threads: %d\n"
           "Round: %d\n\n",
            THREAD_NUM, TEST_ROUND);

    for (int i = 0; i < TEST_ROUND; ++i) {
        printf("Round %d\n", i);
        if (!StartBaseline(THREAD_NUM)) {
            printf("Data Race Detected!\n");
            Beep(750, 500);
            return;
        }
    }
    printf("All Round (%d) Passed\n", TEST_ROUND);
}

////////////////////////////////////////////////////////////////////////////////
// 2-Threads Peterson Test
////////////////////////////////////////////////////////////////////////////////

// 使用 Peterson 算法进行 2 线程累加中的互斥

#define USE_MEMORY_BARRIER

#ifdef USE_MEMORY_BARRIER
#define USEMEMBAR   "on"
#define membar()    MemoryBarrier()
#else
#define USEMEMBAR   "off"
#define membar()    ((void) 0)
#endif

struct Peterson2
{
    int     m_Id;
    int     m_Turn;
    LPVOID  m_Param;

    static volatile bool    Flag[2];
    static volatile int     Turn;

    void init(int id, LPVOID param)
    {
        m_Id = id;
        m_Param = param;
        m_Turn = 1 - id;
    }

    static void reset()
    {
        Flag[0] = false;
        Flag[1] = false;
        Turn = 0;
    }

    void lock()
    {
        Flag[m_Id] = true;
        membar();
        Turn = m_Turn;

        // busy wait
        while (Flag[m_Turn] && Turn == m_Turn)
            YieldProcessor();
    }

    void unlock()
    {
        Flag[m_Id] = false;
    }
};

volatile bool   Peterson2::Flag[2];
volatile int    Peterson2::Turn;

DWORD WINAPI ThreadProcPeterson2(LPVOID param)
{
    _ASSERTE(param != NULL);
    Peterson2* pet = (Peterson2*) param;
    int cnt = (int) (intptr_t) pet->m_Param;

    for (int i = 0; i < cnt; ++i) {
        pet->lock();
        ++g_nSum;
        pet->unlock();
    }
    return 0;
}

bool StartPeterson2()
{
    int expSum = 0;
    g_nSum = 0;
    Peterson2::reset();

    // 初始化 ThreadPool 中的线程
    LPVOID threadParam[2];
    Peterson2 pet[2];
    for (int i = 0; i < 2; ++i) {
        int rnd;
        randint(rnd, NMIN, NMAX);
        expSum += rnd;
        pet[i].init(i, (LPVOID) (intptr_t) rnd);
        threadParam[i] = &pet[i];
    }
    printf("Expected Sum: %d\n", expSum);

    ThreadPool threadPool(ThreadProcPeterson2, threadParam, 2);

    // 启动 ThreadPool 中的线程
    if (!threadPool.StartThreads()) {
        fprintf(stderr, "ThreadPool::StartThreads() failed\n");
        exit(-1);
    }

    // 等待所有线程结束
    if (threadPool.WaitThreads(INFINITE) == WAIT_FAILED) {
        fprintf(stderr, "ThreadPool::WaitThreads() failed: %d\n", GetLastError());
        exit(-1);
    }

    printf("Result Sum: %d\n", g_nSum);
    return (g_nSum == expSum);
}

void TestPeterson2(bool on)
{
    if (!on)
        return;
    PRINT_FUNC(stderr);

    printf("Membar: %s\n"
           "Threads: 2\n"
           "Round: %d\n\n",
           USEMEMBAR, TEST_ROUND);

    for (int i = 0; i < TEST_ROUND; ++i) {
        printf("Round %d\n", i);
        if (!StartPeterson2()) {
            printf("Data Race Detected!\n");
            Beep(750, 500);
            return;
        }
    }
    printf("All Round (%d) Passed\n", TEST_ROUND);
}

////////////////////////////////////////////////////////////////////////////////
// N-Threads Peterson Test
////////////////////////////////////////////////////////////////////////////////

// 使用 Peterson 算法进行 N 线程累加中的互斥

struct PetersonN
{
    int     m_Id;
    LPVOID  m_Param;

    static volatile int*    Flag;
    static volatile int*    Turn;
    static int              ThreadNum;

    void init(int id, LPVOID param)
    {
        m_Id = id;
        m_Param = param;
    }

    static void reset(int threadNum)
    {
        clean();
        ThreadNum = threadNum;

        Flag = new int[threadNum + 1];
        ZeroMemory((void*) Flag, (threadNum + 1) * sizeof(int));

        Turn = new int[threadNum];
        ZeroMemory((void*) Turn, threadNum * sizeof(int));
    }

    static void clean()
    {
        ThreadNum = 0;
        delete[] Flag;
        Flag = NULL;
        delete[] Turn;
        Turn = NULL;
    }

    void lock()
    {
        for (int i = 1; i <= ThreadNum - 1; ++i) {
            Flag[m_Id] = i;
            membar();
            Turn[i] = m_Id;
            for (int j = 1; j <= ThreadNum; ++j) {
                if (j == m_Id)
                    continue;
                while (Flag[j] >= i && Turn[i] == m_Id)
                    YieldProcessor();
            }
        }
    }

    void unlock()
    {
        Flag[m_Id] = 0;
    }
};

volatile int*   PetersonN::Flag = NULL;
volatile int*   PetersonN::Turn = NULL;
int             PetersonN::ThreadNum;

DWORD WINAPI ThreadProcPetersonN(LPVOID param)
{
    _ASSERTE(param != NULL);
    PetersonN* pet = (PetersonN*) param;
    int cnt = (int) (intptr_t) pet->m_Param;

    for (int i = 0; i < cnt; ++i) {
        pet->lock();
        ++g_nSum;
        pet->unlock();
    }
    return 0;
}

bool StartPetersonN(int threadNum)
{
    int expSum = 0;
    g_nSum = 0;
    PetersonN::reset(threadNum);

    // 初始化 ThreadPool 中的线程
    vector<LPVOID> threadParam(threadNum);
    vector<PetersonN> pet(threadNum);
    for (int i = 0; i < threadNum; ++i) {
        int rnd;
        randint(rnd, NMIN, NMAX);
        expSum += rnd;

        // NOTE: 第一个线程的 id 是 1, 而不是 0
        pet[i].init(i + 1, (LPVOID) (intptr_t) rnd);
        threadParam[i] = &pet[i];
    }
    printf("Expected Sum: %d\n", expSum);

    ThreadPool threadPool(ThreadProcPetersonN, &threadParam[0], threadNum);

    // 启动 ThreadPool 中的线程
    if (!threadPool.StartThreads()) {
        fprintf(stderr, "ThreadPool::StartThreads() failed\n");
        exit(-1);
    }

    // 等待所有线程结束
    _ASSERTE(threadNum <= MAXIMUM_WAIT_OBJECTS);
    if (threadPool.WaitThreads(INFINITE) == WAIT_FAILED) {
        fprintf(stderr, "ThreadPool::WaitThreads() failed: %d\n", GetLastError());
        exit(-1);
    }

    printf("Result Sum: %d\n", g_nSum);
    return (g_nSum == expSum);
}

void TestPetersonN(bool on)
{
    if (!on)
        return;
    PRINT_FUNC(stderr);

    printf("Membar: %s\n"
           "Threads: %d\n"
           "Round: %d\n\n",
           USEMEMBAR, THREAD_NUM, TEST_ROUND);

    for (int i = 0; i < TEST_ROUND; ++i) {
        printf("Round %d\n", i);
        if (!StartPetersonN(THREAD_NUM)) {
            printf("Data Race Detected!\n");
            Beep(750, 500);
            return;
        }
    }
    PetersonN::clean();
    printf("All Round (%d) Passed\n", TEST_ROUND);
}

////////////////////////////////////////////////////////////////////////////////
// Entry Point
////////////////////////////////////////////////////////////////////////////////

// Return value:
// 0        Successful
// -1       Setup failed
// others   Runtime errors
int main()
{
    if (!setup()) {
        fprintf(stderr, "%s: setup failed\n", PROGRAM_NAME);
        cleanup();
        return -1;
    }

    TestBaseline(false);
    TestPeterson2(false);
    TestPetersonN(true);

    cleanup();

    return 0;
}

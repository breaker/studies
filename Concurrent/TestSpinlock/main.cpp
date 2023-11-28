// main.cpp
//

#include "pch.h"
#include "threadpool.h"
#include "spinlock.h"

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

const char PROGRAM_NAME[]   = "TestSpinlock";

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

// 自旋锁 Spinlock 对象指针
Spinlock* g_pSpinlock;
// 用于 Spinlock 的 slot
long g_SpinSlot;

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
// 作为与 Spinlock 的对比测试基线

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
// Spinlock Test
////////////////////////////////////////////////////////////////////////////////

// 使用自旋锁 Spinlock 对象进行多线程累加中的互斥

DWORD WINAPI ThreadProcSpinlock(LPVOID param)
{
    int count = (int) (intptr_t) param;

    _ASSERTE(g_pSpinlock != NULL);
    for (int i = 0; i < count; ++i) {
        g_pSpinlock->lock();
        ++g_nSum;
        g_pSpinlock->unlock();
    }
    return 0;
}

bool StartSpinlock(int threadNum)
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

    ThreadPool threadPool(ThreadProcSpinlock, &threadParam[0], threadNum);

    // 初始化 Spinlock 对象
    Spinlock::initslot(&g_SpinSlot);
    g_pSpinlock = new Spinlock(&g_SpinSlot);

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

    delete g_pSpinlock;
    g_pSpinlock = NULL;

    printf("Result Sum: %d\n", g_nSum);
    return (g_nSum == expSum);
}

void TestSpinlock(bool on)
{
    if (!on)
        return;
    PRINT_FUNC(stderr);

    printf("Threads: %d\n"
           "Round: %d\n\n",
           THREAD_NUM, TEST_ROUND);

    for (int i = 0; i < TEST_ROUND; ++i) {
        printf("Round %d\n", i);
        if (!StartSpinlock(THREAD_NUM)) {
            printf("Data Race Detected!\n");
            Beep(750, 500);
            return;
        }
    }
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
    TestSpinlock(true);

    cleanup();

    return 0;
}

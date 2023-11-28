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

// ÿ���߳��ۼ�ֵ��������
// Ԥ���ۼӺ� = �����̵߳��ۼ�ֵ֮��
const int NMIN  = 10000;
const int NMAX  = 30000;

// ���̵߳��ۼӺͽ��
int g_nSum;

// ���Իغ�
const int TEST_ROUND    = 50;

// �����߳���
// ���� [Baseline ����] �� [Peterson-N ����]
// [Peterson-2 ����] �е��߳����̶�Ϊ 2
// THREAD_NUM �����ֵΪ MAXIMUM_WAIT_OBJECTS (64)
// ���� class ThreadPool ֧�ֵ�����߳���
const int THREAD_NUM    = 4;

// �ٽ�������
// ���� [Baseline ����]
CRITICAL_SECTION g_CSec;

// �����ٽ��������������������
// ���λ (0x80) ��Ϊ�˼��� Win2000 ��Ԥ�����ٽ��������ڲ��� Event ����
// WinXP ֮��� Event ������ð�����䷽ʽ, ���λ�ᱻ����
// �������� 0x0400 = 1024
const DWORD CSEC_SPINCOUNT = 0x80000400;

// ������ Spinlock ����ָ��
Spinlock* g_pSpinlock;
// ���� Spinlock �� slot
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

// ʹ��ϵͳ�ٽ������� (Critical Section) ���ж��߳��ۼ��еĻ���
// ��Ϊ�� Spinlock �ĶԱȲ��Ի���

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

    // ��ʼ�� ThreadPool �е��߳�
    vector<LPVOID> threadParam(threadNum);
    for (int i = 0; i < threadNum; ++i) {
        int rnd;
        randint(rnd, NMIN, NMAX);
        expSum += rnd;
        threadParam[i] = (LPVOID) (intptr_t) rnd;
    }
    printf("Expected Sum: %d\n", expSum);

    ThreadPool threadPool(ThreadProcBaseline, &threadParam[0], threadNum);

    // ��ʼ���ٽ�������
    if (!InitializeCriticalSectionAndSpinCount(&g_CSec, CSEC_SPINCOUNT)) {
        fprintf(stderr, "InitializeCriticalSectionAndSpinCount() failed: %d\n", GetLastError());
        exit(-1);
    }

    // ���� ThreadPool �е��߳�
    if (!threadPool.StartThreads()) {
        fprintf(stderr, "ThreadPool::StartThreads() failed\n");
        exit(-1);
    }

    // �ȴ������߳̽���
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

// ʹ�������� Spinlock ������ж��߳��ۼ��еĻ���

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

    // ��ʼ�� ThreadPool �е��߳�
    vector<LPVOID> threadParam(threadNum);
    for (int i = 0; i < threadNum; ++i) {
        int rnd;
        randint(rnd, NMIN, NMAX);
        expSum += rnd;
        threadParam[i] = (LPVOID) (intptr_t) rnd;
    }
    printf("Expected Sum: %d\n", expSum);

    ThreadPool threadPool(ThreadProcSpinlock, &threadParam[0], threadNum);

    // ��ʼ�� Spinlock ����
    Spinlock::initslot(&g_SpinSlot);
    g_pSpinlock = new Spinlock(&g_SpinSlot);

    // ���� ThreadPool �е��߳�
    if (!threadPool.StartThreads()) {
        fprintf(stderr, "ThreadPool::StartThreads() failed\n");
        exit(-1);
    }

    // �ȴ������߳̽���
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

// threadpool.h
//
// 基于 WinAPI 线程的简单线程池
//

#pragma once

////////////////////////////////////////////////////////////////////////////////
// class ThreadPool
////////////////////////////////////////////////////////////////////////////////

class ThreadPool
{
public:
    typedef std::vector<LPVOID> VecThreadParam;
    typedef std::vector<HANDLE> VecThreadHandle;

    // 支持的最大线程数为 MAXIMUM_WAIT_OBJECTS (64)
    // 由 WaitForMultipleObjects() 决定
    static const DWORD MAX_THREADS  = MAXIMUM_WAIT_OBJECTS;

private:
    LPTHREAD_START_ROUTINE  m_ThreadProc;
    VecThreadParam          m_ThreadParams;
    VecThreadHandle         m_ThreadHandles;

private:
    // forbid copy
    ThreadPool(const ThreadPool&);
    ThreadPool& operator=(const ThreadPool&);

public:
    VecThreadHandle& ThreadHandles() { return m_ThreadHandles; }

public:

    // threadProc   线程过程, 线程池中的所有线程过程都为 threadProc
    // params       线程过程的参数, params[0] ~ params[threadCnt-1]
    // threadCnt    线程数, 如果 > MAX_THREADS, 则设置为 MAX_THREADS
    ThreadPool(LPTHREAD_START_ROUTINE threadProc, LPVOID* params = NULL, DWORD threadCnt = 0)
    {
        _ASSERTE(threadProc != NULL);
        m_ThreadProc = threadProc;
        AddThreads(params, (threadCnt > MAX_THREADS ? MAX_THREADS : threadCnt));
    }

    ~ThreadPool()
    {
        Reset();
    }

    void Reset(LPTHREAD_START_ROUTINE threadProc = NULL)
    {
        if (threadProc != NULL)
            m_ThreadProc = threadProc;

        VecThreadHandle::iterator i = m_ThreadHandles.begin();
        for (; i != m_ThreadHandles.end(); ++i) {
            if (*i != NULL)
                CloseHandle(*i);
        }
        m_ThreadHandles.clear();
        m_ThreadParams.clear();
    }

    // 增加新线程的运行参数
    void AddThreads(LPVOID* params, DWORD threadCnt)
    {
        _ASSERTE((threadCnt != 0 && params != NULL) || (threadCnt == 0 && params == NULL));
        for (DWORD i = 0; i < threadCnt; ++i) {
            m_ThreadParams.push_back(params[i]);
            m_ThreadHandles.push_back(NULL);
        }
    }

    bool StartThreads(DWORD flag = 0, SIZE_T stackSize = 0, LPSECURITY_ATTRIBUTES secAttr = NULL)
    {
        size_t threadCnt = m_ThreadParams.size();
        _ASSERTE(threadCnt == m_ThreadHandles.size());
        for (size_t i = 0; i < threadCnt; ++i) {
            m_ThreadHandles[i] = CreateThread(secAttr, stackSize, m_ThreadProc, m_ThreadParams[i], flag, NULL);
            if (m_ThreadHandles[i] == NULL)
                return false;
        }
        return true;
    }

    void ResumeThreads()
    {
        VecThreadHandle::iterator i = m_ThreadHandles.begin();
        for (; i != m_ThreadHandles.end(); ++i) {
            if (*i != NULL)
                ResumeThread(*i);
        }
    }

    DWORD WaitThreads(DWORD timeOut)
    {
        DWORD threadCnt = (DWORD) m_ThreadHandles.size();
        _ASSERTE(threadCnt <= MAX_THREADS);
        return WaitForMultipleObjects(threadCnt, &m_ThreadHandles[0], TRUE, timeOut);
    }
};

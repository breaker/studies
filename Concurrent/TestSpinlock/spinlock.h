// spinlock.h
//
// 基于处理器原子操作 InterlockedExchange() 内建函数的简单自旋锁
//

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Dependent Libraries
////////////////////////////////////////////////////////////////////////////////

// MSVC Intrinsics
#include <intrin.h>

// C Runtime Library (CRT)
#include <crtdbg.h>

// Windows API
// Exclude rarely-used stuff from Windows headers
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

////////////////////////////////////////////////////////////////////////////////
// class Spinlock
////////////////////////////////////////////////////////////////////////////////

class Spinlock
{
private:
    volatile long* m_Slot;

public:
    enum SPIN_STATE {
        // SPIN_FREE 必须是 0, 以便 lock() 中的 _InterlockedExchange 返回 0
        SPIN_FREE   = 0,
        SPIN_HOLD   = 1
    };

private:
    // forbid copy
    Spinlock(const Spinlock&);
    Spinlock& operator=(const Spinlock&);

public:
    // CAUTION:
    // 用户负责在单线程启动阶段将 *slot 初始化为 SPIN_FREE (0)
    static void initslot(_Inout_ volatile long* slot)
    {
        _ASSERTE(slot != NULL);
        *slot = SPIN_FREE;
    }

    // slot     用户提供的 slot
    // owner    是否在构造 Spinlock 对象时加锁
    Spinlock(_Inout_ volatile long* slot, bool owner = false) : m_Slot(slot)
    {
        _ASSERTE(slot != NULL);
        if (owner)
            lock();
    }

    ~Spinlock()
    {
        if (!m_Slot)
            unlock();
    }

public:
    void lock()
    {
        while (_InterlockedExchange(m_Slot, SPIN_HOLD))
            YieldProcessor();
    }

    void unlock()
    {
        _InterlockedExchange(m_Slot, SPIN_FREE);
    }
};

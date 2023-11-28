// spinlock.h
//
// ���ڴ�����ԭ�Ӳ��� InterlockedExchange() �ڽ������ļ�������
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
        // SPIN_FREE ������ 0, �Ա� lock() �е� _InterlockedExchange ���� 0
        SPIN_FREE   = 0,
        SPIN_HOLD   = 1
    };

private:
    // forbid copy
    Spinlock(const Spinlock&);
    Spinlock& operator=(const Spinlock&);

public:
    // CAUTION:
    // �û������ڵ��߳������׶ν� *slot ��ʼ��Ϊ SPIN_FREE (0)
    static void initslot(_Inout_ volatile long* slot)
    {
        _ASSERTE(slot != NULL);
        *slot = SPIN_FREE;
    }

    // slot     �û��ṩ�� slot
    // owner    �Ƿ��ڹ��� Spinlock ����ʱ����
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

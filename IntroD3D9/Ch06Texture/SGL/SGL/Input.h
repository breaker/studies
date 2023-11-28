// Input.h
//

#ifndef __SGL_INPUT_H__
#define __SGL_INPUT_H__

#include <SGL/SGLDepends.h>
#include <SGL/SGLDefs.h>

#include <SGL/SGLUtils.h>
#include <SGL/SGLGame.h>

_SGL_NS_BEGIN

class SGLGameRun;

////////////////////////////////////////////////////////////////////////////////
// 输入处理类 Input
////////////////////////////////////////////////////////////////////////////////

class SGL_API Input : private Uncopyable {
public:
    Input(SGLGameRun* gameRun) : m_GameRun(gameRun) {}
    virtual ~Input() {}

    virtual void ProcessInput() {}


    static BOOL KEY_DOWN(int vkey) {
        return (::GetAsyncKeyState(vkey) & 0x8000);
    }

    static BOOL KEY_UP(int vkey) {
        return !(::GetAsyncKeyState(vkey) & 0x8000);
    }

protected:
    SGLGameRun* m_GameRun;
};

_SGL_NS_END

#endif  // __SGL_INPUT_H__

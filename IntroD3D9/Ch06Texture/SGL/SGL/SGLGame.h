// SGLGame.h
//

#ifndef __SGLGAME_H__
#define __SGLGAME_H__

#include <SGL/SGLDepends.h>
#include <SGL/SGLDefs.h>

#include <SGL/SGLUtils.h>
#include <SGL/Input.h>
#include <SGL/SGLRender.h>

_SGL_NS_BEGIN

////////////////////////////////////////////////////////////////////////////////
// 游戏运行异常类 SGLGameRunExcept
////////////////////////////////////////////////////////////////////////////////

class SGL_API SGLGameRunExcept : public SGLExcept {
public:
    explicit SGLGameRunExcept(DWORD err = SGL_ERR::ERR_UNKNOWN, const char* msg = "") : SGLExcept(err, msg) {}
    explicit SGLGameRunExcept(const char* msg) : SGLExcept(msg) {}

    static const char*  ERR_INVAL_RENDER;
};

////////////////////////////////////////////////////////////////////////////////
// 游戏运行类 SGLGameRun
////////////////////////////////////////////////////////////////////////////////

class SGL_API SGLGameRun : private Uncopyable {
public:
    enum GAME_STATE {
        GAME_STOP,
        GAME_RUN
    };

public:
    SGLGameRun(SGLRender* render);
    virtual ~SGLGameRun();

public:
    virtual void run(float timeDelta);

    void stop();
    void resume();

    SGLRender* getRender() {
        return render_.get();
    }

protected:
    auto_ptr<SGLRender> render_;
    GAME_STATE          state_;
};

_SGL_NS_END

#endif  // __SGLGAME_H__

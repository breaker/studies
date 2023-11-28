// SGLGame.cpp
//

#include "StdAfx.h"
#include "SGL.h"
#include <SGL/SGLGame.h>

_SGL_NS_BEGIN

////////////////////////////////////////////////////////////////////////////////
// 游戏运行异常类 SGLGameRunExcept
////////////////////////////////////////////////////////////////////////////////

const char* SGLGameRunExcept::ERR_INVAL_RENDER = "render is invalid";

////////////////////////////////////////////////////////////////////////////////
// 游戏运行类 SGLGameRun
////////////////////////////////////////////////////////////////////////////////

SGLGameRun::SGLGameRun(SGLRender* render) : state_(GAME_STOP), render_(render)
{
    if (render == NULL)
        throw SGLGameRunExcept(SGL_ERR::ERR_INVAL, SGLGameRunExcept::ERR_INVAL_RENDER);
    state_ = GAME_RUN;
}

SGLGameRun::~SGLGameRun()
{
    state_ = GAME_STOP;
}

void SGLGameRun::stop()
{
    state_ = GAME_STOP;
}

void SGLGameRun::resume()
{
    state_ = GAME_RUN;
}

void SGLGameRun::run(float timeDelta)
{
    if (state_ != GAME_RUN)
        return;

    render_->render(timeDelta);
}

_SGL_NS_END

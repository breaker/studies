//////////////////////////////////////////////////////////////////////////
//                  Character Animation with Direct3D                   //
//                         Author: C. Granberg                          //
//                             2008 - 2009                              //
//////////////////////////////////////////////////////////////////////////

#ifndef _ANIMATION_CLASS_
#define _ANIMATION_CLASS_

#include <d3dx9.h>

class Animation {
public:
    Animation();
    void Update(float deltaTime);
    void Draw();

private:
    ID3DXKeyframedAnimationSet *m_pAnimSet;
    float m_time;
};

#endif

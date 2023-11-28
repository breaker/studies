//////////////////////////////////////////////////////////////////////////
//                  Character Animation with Direct3D                   //
//                         Author: C. Granberg                          //
//                             2008 - 2009                              //
//////////////////////////////////////////////////////////////////////////

#ifndef MTEST
#define MTEST

#include <d3dx9.h>
#include <fstream>

using namespace std;

class MorphTest {
    friend class Application;
public:
    MorphTest();
    void Init();
    void Update(float deltaTime);
    void Render();

private:
    ID3DXMesh *m_pTarget01, *m_pTarget02, *m_pFace;
    float m_blend;
};

#endif
//////////////////////////////////////////////////////////////////////////
//                  Character Animation with Direct3D                   //
//                         Author: C. Granberg                          //
//                             2008 - 2009                              //
//////////////////////////////////////////////////////////////////////////

#ifndef _DUMMY_FACE_
#define _DUMMY_FACE_

#include <d3dx9.h>

class DummyFace {
public:
    DummyFace();
    ~DummyFace();
    void Render();

private:
    ID3DXMesh *m_pMesh;
    IDirect3DTexture9* m_pTexture;
};

#endif
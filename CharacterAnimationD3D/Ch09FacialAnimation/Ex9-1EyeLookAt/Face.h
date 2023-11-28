//////////////////////////////////////////////////////////////////////////
//                  Character Animation with Direct3D                   //
//                         Author: C. Granberg                          //
//                             2008 - 2009                              //
//////////////////////////////////////////////////////////////////////////

#ifndef _FACE_
#define _FACE_

#include <d3dx9.h>
#include <fstream>
#include "eye.h"

using namespace std;

class Face {
public:
    Face();
    void Init();
    void Update(float deltaTime);
    void Render();
    void RenderEye(D3DXVECTOR3 pos);

private:
    ID3DXMesh *m_pFaceMesh;
    IDirect3DTexture9 *m_pFaceTexture;
    Eye m_eyes[2];
};

#endif
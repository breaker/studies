//////////////////////////////////////////////////////////////////////////
//                  Character Animation with Direct3D                   //
//                         Author: C. Granberg                          //
//                             2008 - 2009                              //
//////////////////////////////////////////////////////////////////////////

#ifndef _FACE_
#define _FACE_

#include <d3dx9.h>
#include <vector>
#include <fstream>
#include "eye.h"

using namespace std;

class FaceController;

class Face {
public:
    Face(string filename);
    ~Face();
    void Render(FaceController *pController);
    void ExtractMeshes(D3DXFRAME *frame);

public:
    ID3DXMesh *m_pBaseMesh;
    ID3DXMesh *m_pBlinkMesh;
    vector<ID3DXMesh*> m_emotionMeshes;
    vector<ID3DXMesh*> m_speechMeshes;

    IDirect3DVertexDeclaration9 *m_pFaceVertexDecl;

    IDirect3DTexture9 *m_pFaceTexture;
    IDirect3DTexture9 *m_pFaceNormalMap;
};

#endif
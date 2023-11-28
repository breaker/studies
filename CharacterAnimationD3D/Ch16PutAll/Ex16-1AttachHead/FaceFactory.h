//////////////////////////////////////////////////////////////////////////
//                  Character Animation with Direct3D                   //
//                         Author: C. Granberg                          //
//                             2008 - 2009                              //
//////////////////////////////////////////////////////////////////////////

#ifndef _FACEFACTORY_
#define _FACEFACTORY_

#include <d3dx9.h>
#include <vector>
#include "face.h"

using namespace std;

class FaceFactory {
public:
    FaceFactory(string filename);
    ~FaceFactory();
    void ExtractMeshes(D3DXFRAME *frame);
    Face* GenerateRandomFace();
    ID3DXMesh* CreateMorphTarget(ID3DXMesh* mesh, vector<float> &morphWeights);

public:
    ID3DXMesh *m_pBaseMesh;
    ID3DXMesh *m_pBlinkMesh;
    ID3DXMesh *m_pEyeMesh;
    vector<ID3DXMesh*> m_emotionMeshes;
    vector<ID3DXMesh*> m_speechMeshes;
    vector<ID3DXMesh*> m_morphMeshes;

    vector<IDirect3DTexture9*> m_faceTextures;
};

#endif
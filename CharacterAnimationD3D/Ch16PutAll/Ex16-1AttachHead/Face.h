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

class FaceHierarchyLoader: public ID3DXAllocateHierarchy {
public:
    STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame);
    STDMETHOD(CreateMeshContainer)(THIS_ LPCTSTR Name, CONST D3DXMESHDATA * pMeshData, CONST D3DXMATERIAL * pMaterials, CONST D3DXEFFECTINSTANCE * pEffectInstances, DWORD NumMaterials, CONST DWORD * pAdjacency, LPD3DXSKININFO pSkinInfo, LPD3DXMESHCONTAINER * ppNewMeshContainer);
    STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree);
    STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerBase);
};

class Face {
public:
    Face();
    Face(string filename);
    ~Face();
    void Render(FaceController *pController);
    void ExtractMeshes(D3DXFRAME *frame);
    void SetStreamSources(FaceController *pController);

public:
    ID3DXMesh *m_pBaseMesh;
    ID3DXMesh *m_pBlinkMesh;
    vector<ID3DXMesh*> m_emotionMeshes;
    vector<ID3DXMesh*> m_speechMeshes;

    IDirect3DVertexDeclaration9 *m_pFaceVertexDecl;

    IDirect3DTexture9 *m_pFaceTexture;
};

#endif
//////////////////////////////////////////////////////////////////////////
//                  Character Animation with Direct3D                   //
//                         Author: C. Granberg                          //
//                             2008 - 2009                              //
//////////////////////////////////////////////////////////////////////////

#ifndef MTEST
#define MTEST

#include <d3dx9.h>
#include <fstream>
#include "skinnedmesh.h"

using namespace std;

class MorphTest : public SkinnedMesh {
    friend class Application;
public:
    MorphTest();
    void Init();
    void Update(float deltaTime);
    void Render();
    void RenderHuman(Bone *bone);

private:
    ID3DXAnimationController* m_animControl;
    IDirect3DVertexDeclaration9 *m_pDecl;

    IDirect3DTexture9* m_pHumanTexture;
    IDirect3DTexture9* m_pWolfTexture;
};

#endif
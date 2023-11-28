//////////////////////////////////////////////////////////////////////////
//                  Character Animation with Direct3D                   //
//                         Author: C. Granberg                          //
//                             2008 - 2009                              //
//////////////////////////////////////////////////////////////////////////

#ifndef SKINNED_MESH
#define SKINNED_MESH

#include <windows.h>
#include <d3dx9.h>

struct Bone: public D3DXFRAME {
    D3DXMATRIX CombinedTransformationMatrix;    // 子骨骼到根骨骼的变换矩阵, 子骨骼 => 模型整体
};

struct BoneMesh: public D3DXMESHCONTAINER {
    //More to come here later...
};

class SkinnedMesh {
public:
    SkinnedMesh();
    ~SkinnedMesh();
    void Load(char fileName[]);
    void RenderSkeleton(Bone* bone, Bone *parent, D3DXMATRIX world);

private:
    void UpdateMatrices(Bone* bone, D3DXMATRIX *parentMatrix);

    D3DXFRAME *m_pRootBone;
    LPD3DXMESH m_pSphereMesh;
};

#endif
//////////////////////////////////////////////////////////////////////////
//                  Character Animation with Direct3D                   //
//                         Author: C. Granberg                          //
//                             2008 - 2009                              //
//////////////////////////////////////////////////////////////////////////

#ifndef SKINNED_MESH
#define SKINNED_MESH

#include <windows.h>
#include <d3dx9.h>
#include <string>
#include <vector>

using namespace std;

struct Bone: public D3DXFRAME {
    D3DXMATRIX CombinedTransformationMatrix;
};

struct BoneMesh: public D3DXMESHCONTAINER {
    ID3DXMesh* OriginalMesh;
    vector<D3DMATERIAL9> materials;
    vector<IDirect3DTexture9*> textures;

    DWORD NumAttributeGroups;
    D3DXATTRIBUTERANGE* attributeTable;
    D3DXMATRIX** boneMatrixPtrs;
    D3DXMATRIX* boneOffsetMatrices;
    D3DXMATRIX* currentBoneMatrices;
};

class SkinnedMesh {
public:
    SkinnedMesh();
    ~SkinnedMesh();
    void Load(char fileName[]);
    void Render(Bone *bone);
    void RenderSkeleton(Bone* bone, Bone *parent, D3DXMATRIX world);

    void SetPose(D3DXMATRIX world, float time);
    void SetAnimation(string name);
    void GetAnimations(vector<string> &animations);

private:
    void UpdateMatrices(Bone* bone, D3DXMATRIX *parentMatrix);
    void SetupBoneMatrixPointers(Bone *bone);

    D3DXFRAME *m_pRootBone;
    ID3DXAnimationController *m_pAnimControl;
    LPD3DXMESH m_pSphereMesh;
};

#endif
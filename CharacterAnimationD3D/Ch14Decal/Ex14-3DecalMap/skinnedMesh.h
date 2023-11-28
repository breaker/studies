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
#include <map>
#include "CharacterDecal.h"

using namespace std;

struct LineVertex {
    LineVertex();
    LineVertex(D3DXVECTOR3 pos, D3DCOLOR col) {
        position = pos;
        color = col;
    }
    D3DXVECTOR3 position;
    D3DCOLOR color;
    static const DWORD FVF;
};

struct Vertex {
    D3DXVECTOR3 position;
    D3DXVECTOR3 normal;
    D3DXVECTOR2 uv;
    static const DWORD FVF;
};

struct DecalVertex {
    D3DXVECTOR3 position;
    float       blendweights;
    byte        blendindices[4];
    D3DXVECTOR3 normal;
    D3DXVECTOR2 uv;
};


struct Bone: public D3DXFRAME {
    D3DXMATRIX CombinedTransformationMatrix;
};

class BoneMesh: public D3DXMESHCONTAINER {
public:
    BoneMesh();

    D3DXINTERSECTINFO GetFace(D3DXVECTOR3 &rayOrg, D3DXVECTOR3 &rayDir);
    ID3DXMesh* CreateDecalMesh(D3DXVECTOR3 &rayOrg, D3DXVECTOR3 &rayDir, float decalSize);
    void CalculateDecalUV(ID3DXMesh* decalMesh, D3DXVECTOR3 &hitPos, float decalSize);
    void AddDecal(D3DXVECTOR3 &rayOrg, D3DXVECTOR3 &rayDir, float decalSize);

public:
    ID3DXMesh* OriginalMesh;
    vector<D3DMATERIAL9> materials;
    vector<IDirect3DTexture9*> textures;
    DWORD NumAttributeGroups;
    D3DXATTRIBUTERANGE* attributeTable;
    D3DXMATRIX** boneMatrixPtrs;
    D3DXMATRIX* boneOffsetMatrices;
    D3DXMATRIX* currentBoneMatrices;

    vector<CharacterDecal*> m_decals;
};

class SkinnedMesh {
public:
    SkinnedMesh();
    ~SkinnedMesh();
    void Load(char fileName[]);
    void Render(Bone *bone);

    void SetPose(D3DXMATRIX world);
    void GetAnimations(vector<string> &animations);

    ID3DXAnimationController* GetController();

    D3DXFRAME* GetBone(string name);

    void AddDecal(Bone* pBone = NULL);

private:
    void UpdateMatrices(Bone* bone, D3DXMATRIX *parentMatrix);
    void SetupBoneMatrixPointers(Bone *bone);

    D3DXFRAME *m_pRootBone;
    ID3DXAnimationController *m_pAnimControl;
};

#endif
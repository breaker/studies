#include "skinnedMesh.h"

#pragma warning(disable:4996)

extern IDirect3DDevice9* g_pDevice;

class BoneHierarchyLoader: public ID3DXAllocateHierarchy {
public:
    STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame);
    STDMETHOD(CreateMeshContainer)(THIS_ LPCTSTR Name, CONST D3DXMESHDATA * pMeshData, CONST D3DXMATERIAL * pMaterials, CONST D3DXEFFECTINSTANCE * pEffectInstances, DWORD NumMaterials, CONST DWORD * pAdjacency, LPD3DXSKININFO pSkinInfo, LPD3DXMESHCONTAINER * ppNewMeshContainer);
    STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree);
    STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerBase);
};

HRESULT BoneHierarchyLoader::CreateFrame(LPCSTR Name, LPD3DXFRAME *ppNewFrame) {
    Bone *newBone = new Bone;
    memset(newBone, 0, sizeof(Bone));

    //Copy name
    if (Name != NULL) {
        newBone->Name = new char[strlen(Name)+1];
        strcpy(newBone->Name, Name);
    }

    //Set the transformation matrices
    D3DXMatrixIdentity(&newBone->TransformationMatrix);
    // 先初始化为单位矩阵, 然后在 SkinnedMesh::UpdateMatrices 中建立每个骨骼的变换矩阵
    D3DXMatrixIdentity(&newBone->CombinedTransformationMatrix);

    //Return the new bone...
    *ppNewFrame = (D3DXFRAME*)newBone;

    return S_OK;
}

HRESULT BoneHierarchyLoader::CreateMeshContainer(LPCSTR Name,
        CONST D3DXMESHDATA *pMeshData,
        CONST D3DXMATERIAL *pMaterials,
        CONST D3DXEFFECTINSTANCE *pEffectInstances,
        DWORD NumMaterials,
        CONST DWORD *pAdjacency,
        LPD3DXSKININFO pSkinInfo,
        LPD3DXMESHCONTAINER *ppNewMeshContainer) {
    //Just return a temporary mesh for now...
    *ppNewMeshContainer = new BoneMesh;
    memset(*ppNewMeshContainer, 0, sizeof(BoneMesh));

    return S_OK;
}

HRESULT BoneHierarchyLoader::DestroyFrame(LPD3DXFRAME pFrameToFree) {
    if (pFrameToFree) {
        //Free Name String
        if (pFrameToFree->Name != NULL)
            delete [] pFrameToFree->Name;

        //Free Frame
        delete pFrameToFree;
    }

    pFrameToFree = NULL;

    return S_OK;
}

HRESULT BoneHierarchyLoader::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase) {
    if (pMeshContainerBase != NULL)
        delete pMeshContainerBase;

    return S_OK;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//                                  SKINNED MESH                                                //
//////////////////////////////////////////////////////////////////////////////////////////////////

struct VERTEX {
    VERTEX();
    VERTEX(D3DXVECTOR3 pos, D3DCOLOR col) {
        position = pos;
        color = col;
    }
    D3DXVECTOR3 position;
    D3DCOLOR color;
    static const DWORD FVF;
};

const DWORD VERTEX::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

SkinnedMesh::SkinnedMesh() {
    m_pRootBone = NULL;
    m_pSphereMesh = NULL;
}

SkinnedMesh::~SkinnedMesh() {
    BoneHierarchyLoader boneHierarchy;
    boneHierarchy.DestroyFrame(m_pRootBone);
}

void SkinnedMesh::Load(char fileName[]) {
    BoneHierarchyLoader boneHierarchy;

    //Load a bone hierarchy from a file
    D3DXLoadMeshHierarchyFromX(fileName, D3DXMESH_MANAGED, g_pDevice, &boneHierarchy,
                               NULL, &m_pRootBone, NULL);

    //Update Bone transformation matrices
    D3DXMATRIX i;
    D3DXMatrixIdentity(&i);
    UpdateMatrices((Bone*)m_pRootBone, &i);

    //Create Sphere
    D3DXCreateSphere(g_pDevice, 0.02f, 10, 10, &m_pSphereMesh, NULL);
}

// 建立每个骨骼的变换矩阵 CombinedTransformationMatrix
void SkinnedMesh::UpdateMatrices(Bone* bone, D3DXMATRIX *parentMatrix) {
    if (bone == NULL) return;

    //Calculate the combined transformation matrix
    D3DXMatrixMultiply(&bone->CombinedTransformationMatrix,
                       &bone->TransformationMatrix,
                       parentMatrix);

    //Perform the same calculation on all siblings
    if (bone->pFrameSibling) {
        UpdateMatrices((Bone*)bone->pFrameSibling,
                       parentMatrix);
    }

    //... and all children
    if (bone->pFrameFirstChild) {
        UpdateMatrices((Bone*)bone->pFrameFirstChild,
                       &bone->CombinedTransformationMatrix);
    }
}

void SkinnedMesh::RenderSkeleton(Bone* bone, Bone *parent, D3DXMATRIX world) {
    //Temporary function to render the bony hierarchy
    if (bone == NULL) bone = (Bone*)m_pRootBone;

    D3DXMATRIX r, s;
    D3DXMatrixRotationYawPitchRoll(&r, -D3DX_PI * 0.5f, 0.0f, 0.0f);

    //Draw line between bones
    if (parent != NULL && bone->Name != NULL && parent->Name != NULL) {
        //Draw Sphere
        g_pDevice->SetRenderState(D3DRS_LIGHTING, true);
        g_pDevice->SetTransform(D3DTS_WORLD, &(r * bone->CombinedTransformationMatrix * world));
        m_pSphereMesh->DrawSubset(0);

        D3DXMATRIX w1 = bone->CombinedTransformationMatrix;
        D3DXMATRIX w2 = parent->CombinedTransformationMatrix;

        //Extract translation 骨骼位置
        D3DXVECTOR3 thisBone = D3DXVECTOR3(w1(3, 0), w1(3, 1), w1(3, 2));
        D3DXVECTOR3 ParentBone = D3DXVECTOR3(w2(3, 0), w2(3, 1), w2(3, 2));

        // 绘制骨骼连线. < 2.0f 为只绘制临近骨骼, 对于本例模型都是临近骨骼, 所以这个条件可以去掉
        if (D3DXVec3Length(&(thisBone - ParentBone)) < 2.0f) {
            g_pDevice->SetTransform(D3DTS_WORLD, &world);
            VERTEX vert[] = {VERTEX(ParentBone, 0xffff0000), VERTEX(thisBone, 0xff00ff00)};
            g_pDevice->SetRenderState(D3DRS_LIGHTING, false);
            g_pDevice->SetFVF(VERTEX::FVF);
            g_pDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 1, &vert[0], sizeof(VERTEX));
        }
    }

    if (bone->pFrameSibling) RenderSkeleton((Bone*)bone->pFrameSibling, parent, world);
    if (bone->pFrameFirstChild) RenderSkeleton((Bone*)bone->pFrameFirstChild, bone, world);
}

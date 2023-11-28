#include "skinnedMesh.h"
#include <fstream>
#include <queue>
#include <map>

#pragma warning(disable:4996)

extern IDirect3DDevice9* g_pDevice;
extern ID3DXEffect *g_pEffect;
extern ofstream g_debug;
extern D3DXVECTOR3 g_rayOrg;
extern D3DXVECTOR3 g_rayDir;

void PrintMeshDeclaration(ID3DXMesh* pMesh) {
    //Get vertex declaration
    D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE];
    pMesh->GetDeclaration(decl);

    //Loop through valid elements
    for (int i=0; i<MAX_FVF_DECL_SIZE; i++) {
        if (decl[i].Type != D3DDECLTYPE_UNUSED) {
            g_debug << "Offset: " << (int)decl[i].Offset << ", Type: " << (int)decl[i].Type << ", Usage: " << (int)decl[i].Usage << "\n";
        }
        else break;
    }
}

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
    //Create new Bone Mesh
    BoneMesh *boneMesh = new BoneMesh;
    memset(boneMesh, 0, sizeof(BoneMesh));

    //Get mesh data
    boneMesh->OriginalMesh = pMeshData->pMesh;
    boneMesh->MeshData.pMesh = pMeshData->pMesh;
    boneMesh->MeshData.Type = pMeshData->Type;
    pMeshData->pMesh->AddRef();     //Add Reference so that the mesh isnt deallocated

    //Copy materials and load textures (just like with a static mesh)
    for (int i=0; i<(int)NumMaterials; i++) {
        D3DXMATERIAL mtrl;
        memcpy(&mtrl, &pMaterials[i], sizeof(D3DXMATERIAL));
        boneMesh->materials.push_back(mtrl.MatD3D);
        IDirect3DTexture9* newTexture = NULL;

        if (mtrl.pTextureFilename != NULL) {
            char textureFname[200];
            strcpy(textureFname, "resources/meshes/");
            strcat(textureFname, mtrl.pTextureFilename);

            //Load texture
            D3DXCreateTextureFromFile(g_pDevice, textureFname, &newTexture);
        }

        boneMesh->textures.push_back(newTexture);
    }

    if (pSkinInfo != NULL) {
        //Get Skin Info
        boneMesh->pSkinInfo = pSkinInfo;
        pSkinInfo->AddRef();    //Add reference so that the SkinInfo isnt deallocated

        //Clone mesh and store in boneMesh->MeshData.pMesh
        //pMeshData->pMesh->CloneMeshFVF(D3DXMESH_MANAGED, pMeshData->pMesh->GetFVF(),
        //                             g_pDevice, &boneMesh->MeshData.pMesh);

        DWORD maxVertInfluences = 0;
        DWORD numBoneComboEntries = 0;
        ID3DXBuffer* boneComboTable = 0;

        pSkinInfo->ConvertToIndexedBlendedMesh(pMeshData->pMesh,
                                               D3DXMESH_MANAGED | D3DXMESH_WRITEONLY,
                                               30,
                                               0, // ignore adjacency in
                                               0, // ignore adjacency out
                                               0, // ignore face remap
                                               0, // ignore vertex remap
                                               &maxVertInfluences,
                                               &numBoneComboEntries,
                                               &boneComboTable,
                                               &boneMesh->MeshData.pMesh);

        if (boneComboTable != NULL)
            boneComboTable->Release();

        //Get Attribute Table
        boneMesh->MeshData.pMesh->GetAttributeTable(NULL, &boneMesh->NumAttributeGroups);
        boneMesh->attributeTable = new D3DXATTRIBUTERANGE[boneMesh->NumAttributeGroups];
        boneMesh->MeshData.pMesh->GetAttributeTable(boneMesh->attributeTable, NULL);

        //Create bone offset and current matrices
        int NumBones = pSkinInfo->GetNumBones();
        boneMesh->boneOffsetMatrices = new D3DXMATRIX[NumBones];
        boneMesh->currentBoneMatrices = new D3DXMATRIX[NumBones];

        //Get bone offset matrices
        for (int i=0; i < NumBones; i++)
            boneMesh->boneOffsetMatrices[i] = *(boneMesh->pSkinInfo->GetBoneOffsetMatrix(i));
    }

    //Set ppNewMeshContainer to the newly created boneMesh container
    *ppNewMeshContainer = boneMesh;

    return S_OK;
}

HRESULT BoneHierarchyLoader::DestroyFrame(LPD3DXFRAME pFrameToFree) {
    if (pFrameToFree) {
        //Free name
        if (pFrameToFree->Name != NULL)
            delete [] pFrameToFree->Name;

        //Free bone
        delete pFrameToFree;
    }
    pFrameToFree = NULL;

    return S_OK;
}

HRESULT BoneHierarchyLoader::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase) {
    BoneMesh *boneMesh = (BoneMesh*)pMeshContainerBase;

    //Release textures
    for (int i=0; i < (int)boneMesh->textures.size(); i++)
        if (boneMesh->textures[i] != NULL)
            boneMesh->textures[i]->Release();

    //Release mesh data
    if (boneMesh->MeshData.pMesh)boneMesh->MeshData.pMesh->Release();
    if (boneMesh->pSkinInfo)boneMesh->pSkinInfo->Release();
    if (boneMesh->OriginalMesh)boneMesh->OriginalMesh->Release();
    delete boneMesh;

    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//                                  BONE MESH                                                   //
//////////////////////////////////////////////////////////////////////////////////////////////////

BoneMesh::BoneMesh() {
}

D3DXINTERSECTINFO BoneMesh::GetFace(D3DXVECTOR3 &rayOrg, D3DXVECTOR3 &rayDir) {
    D3DXINTERSECTINFO hitInfo;

    //Must test against software skinned model
    if (pSkinInfo != NULL) {
        //Make sure vertex format is corrent
        if (OriginalMesh->GetFVF() != Vertex::FVF) {
            hitInfo.FaceIndex = 0xffffffff;
            return hitInfo;
        }

        // set up bone transforms
        int numBones = pSkinInfo->GetNumBones();
        for (int i=0; i < numBones; i++)
            D3DXMatrixMultiply(&currentBoneMatrices[i],
                               &boneOffsetMatrices[i],
                               boneMatrixPtrs[i]);

        //Create temp mesh
        ID3DXMesh *tempMesh = NULL;
        OriginalMesh->CloneMeshFVF(D3DXMESH_MANAGED,
                                   OriginalMesh->GetFVF(),
                                   g_pDevice,
                                   &tempMesh);

        //calculate pose of the skinned mesh
        BYTE *src = NULL, *dest = NULL;
        OriginalMesh->LockVertexBuffer(D3DLOCK_READONLY, (VOID**)&src);
        tempMesh->LockVertexBuffer(0, (VOID**)&dest);
        pSkinInfo->UpdateSkinnedMesh(currentBoneMatrices, NULL, src, dest);
        OriginalMesh->UnlockVertexBuffer();
        tempMesh->UnlockVertexBuffer();

        //Intersection Test
        BOOL Hit;
        D3DXIntersect(tempMesh,
                      &rayOrg,
                      &rayDir,
                      &Hit,
                      &hitInfo.FaceIndex,
                      &hitInfo.U,
                      &hitInfo.V,
                      &hitInfo.Dist,
                      NULL,
                      NULL);

        //Release temporary mesh
        tempMesh->Release();

        if (Hit) {
            return hitInfo;
        }
    }

    //No Hit
    hitInfo.FaceIndex = 0xffffffff;
    hitInfo.Dist = -1.0f;
    return hitInfo;
}

ID3DXMesh* BoneMesh::CreateDecalMesh(D3DXVECTOR3 &rayOrg, D3DXVECTOR3 &rayDir, float decalSize) {
    //Only supports skinned meshes for now
    if (pSkinInfo == NULL)
        return NULL;

    D3DXINTERSECTINFO hitInfo = GetFace(rayOrg, rayDir);

    //No face was hit
    if (hitInfo.FaceIndex == 0xffffffff)
        return NULL;

    //Generate adjacency lookup table
    DWORD* adj = new DWORD[OriginalMesh->GetNumFaces() * 3];
    OriginalMesh->GenerateAdjacency(0.001f, adj);

    //Get Vertex & index buffer of temp mesh
    Vertex *v = NULL;
    WORD *i = NULL;
    OriginalMesh->LockVertexBuffer(D3DLOCK_READONLY, (VOID**)&v);
    OriginalMesh->LockIndexBuffer(D3DLOCK_READONLY, (VOID**)&i);

    //Calculate hit position on original mesh
    WORD i1 = i[hitInfo.FaceIndex * 3 + 0];
    WORD i2 = i[hitInfo.FaceIndex * 3 + 1];
    WORD i3 = i[hitInfo.FaceIndex * 3 + 2];
    D3DXVECTOR3 hitPos;
    D3DXVec3BaryCentric(&hitPos,
                        &v[i1].position,
                        &v[i2].position,
                        &v[i3].position,
                        hitInfo.U,
                        hitInfo.V);

    //Find adjacent faces within in range of hit location
    queue<WORD> openFaces;
    map<WORD, bool> decalFaces;

    //Add first face
    openFaces.push((WORD)hitInfo.FaceIndex);

    while (!openFaces.empty()) {
        //Get first face
        WORD face = openFaces.front();
        openFaces.pop();

        //Get Triangle data for open face
        WORD i1 = i[face * 3 + 0];
        WORD i2 = i[face * 3 + 1];
        WORD i3 = i[face * 3 + 2];
        D3DXVECTOR3 &v1 = v[i1].position;
        D3DXVECTOR3 &v2 = v[i2].position;
        D3DXVECTOR3 &v3 = v[i3].position;

        float testSize = max(decalSize, 0.1f);

        //Should this face be added?
        if (D3DXVec3Length(&(hitPos - v1)) < testSize ||
                D3DXVec3Length(&(hitPos - v2)) < testSize ||
                D3DXVec3Length(&(hitPos - v3)) < testSize ||
                decalFaces.empty()) {
            decalFaces[face] = true;

            //Add adjacent faces to open queue
            for (int a=0; a<3; a++) {
                DWORD adjFace = adj[face * 3 + a];

                if (adjFace != 0xffffffff) {
                    //Check that it hasnt been added to decal faces
                    if (decalFaces.count((WORD)adjFace) == 0)
                        openFaces.push((WORD)adjFace);
                }
            }
        }
    }

    OriginalMesh->UnlockIndexBuffer();
    OriginalMesh->UnlockVertexBuffer();

    //Create decal mesh
    ID3DXMesh* decalMesh = NULL;

    //No faces to create decal with
    if (decalFaces.empty())
        return NULL;

    //Create a new mesh from selected faces
    D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE];
    MeshData.pMesh->GetDeclaration(decl);

    D3DXCreateMesh((int)decalFaces.size(),
                   (int)decalFaces.size() * 3,
                   D3DXMESH_MANAGED,
                   decl,
                   g_pDevice,
                   &decalMesh);

    //Lock dest & src buffers
    DecalVertex* vDest = NULL;
    WORD* iDest = NULL;
    DecalVertex* vSrc = NULL;
    WORD* iSrc = NULL;
    decalMesh->LockVertexBuffer(0, (VOID**)&vDest);
    decalMesh->LockIndexBuffer(0, (VOID**)&iDest);
    MeshData.pMesh->LockVertexBuffer(D3DLOCK_READONLY, (VOID**)&vSrc);
    MeshData.pMesh->LockIndexBuffer(D3DLOCK_READONLY, (VOID**)&iSrc);

    //Iterate through all faces in the decalFaces map
    map<WORD, bool>::iterator f;
    int index = 0;
    for (f=decalFaces.begin(); f!=decalFaces.end(); f++) {
        WORD faceIndex = (*f).first;

        //Copy vertex data
        vDest[index * 3 + 0] = vSrc[iSrc[faceIndex * 3 + 0]];
        vDest[index * 3 + 1] = vSrc[iSrc[faceIndex * 3 + 1]];
        vDest[index * 3 + 2] = vSrc[iSrc[faceIndex * 3 + 2]];

        //Create indices
        iDest[index * 3 + 0] = index * 3 + 0;
        iDest[index * 3 + 1] = index * 3 + 1;
        iDest[index * 3 + 2] = index * 3 + 2;

        index++;
    }

    //Unlock buffers
    decalMesh->UnlockIndexBuffer();
    decalMesh->UnlockVertexBuffer();
    MeshData.pMesh->UnlockIndexBuffer();
    MeshData.pMesh->UnlockIndexBuffer();

    return decalMesh;
}

void BoneMesh::AddDecal(D3DXVECTOR3 &rayOrg, D3DXVECTOR3 &rayDir, float decalSize) {
    //Create decal mesh
    ID3DXMesh* decalMesh = CreateDecalMesh(rayOrg, rayDir, decalSize);
    if (decalMesh == NULL)
        return;

    //Add to decal vector
    m_decals.push_back(new CharacterDecal(decalMesh));
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//                                  SKINNED MESH                                                //
//////////////////////////////////////////////////////////////////////////////////////////////////

const DWORD LineVertex::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

SkinnedMesh::SkinnedMesh() {
    m_pRootBone = NULL;
    m_pAnimControl = NULL;
}

SkinnedMesh::~SkinnedMesh() {
    BoneHierarchyLoader boneHierarchy;
    boneHierarchy.DestroyFrame(m_pRootBone);
    if (m_pAnimControl)m_pAnimControl->Release();
}

void SkinnedMesh::Load(char fileName[]) {
    BoneHierarchyLoader boneHierarchy;

    D3DXLoadMeshHierarchyFromX(fileName, D3DXMESH_MANAGED,
                               g_pDevice, &boneHierarchy,
                               NULL, &m_pRootBone, &m_pAnimControl);

    SetupBoneMatrixPointers((Bone*)m_pRootBone);

    //Update all the bones
    D3DXMATRIX i;
    D3DXMatrixIdentity(&i);
    UpdateMatrices((Bone*)m_pRootBone, &i);
}

void SkinnedMesh::UpdateMatrices(Bone* bone, D3DXMATRIX *parentMatrix) {
    if (bone == NULL)return;

    D3DXMatrixMultiply(&bone->CombinedTransformationMatrix,
                       &bone->TransformationMatrix,
                       parentMatrix);

    if (bone->pFrameSibling)UpdateMatrices((Bone*)bone->pFrameSibling, parentMatrix);
    if (bone->pFrameFirstChild)UpdateMatrices((Bone*)bone->pFrameFirstChild, &bone->CombinedTransformationMatrix);
}

void SkinnedMesh::Render(Bone *bone) {
    if (bone == NULL)
        bone = (Bone*)m_pRootBone;

    //If there is a mesh to render...
    if (bone->pMeshContainer != NULL) {
        BoneMesh *boneMesh = (BoneMesh*)bone->pMeshContainer;

        if (boneMesh->pSkinInfo != NULL) {
            // set up bone transforms
            int numBones = boneMesh->pSkinInfo->GetNumBones();
            for (int i=0; i < numBones; i++) {
                D3DXMatrixMultiply(&boneMesh->currentBoneMatrices[i],
                                   &boneMesh->boneOffsetMatrices[i],
                                   boneMesh->boneMatrixPtrs[i]);
            }

            D3DXMATRIX view, proj, identity;
            g_pEffect->SetMatrixArray("FinalTransforms", boneMesh->currentBoneMatrices, boneMesh->pSkinInfo->GetNumBones());
            D3DXMatrixIdentity(&identity);

            //Render the mesh
            for (int i=0; i<(int)boneMesh->NumAttributeGroups; i++) {
                int mtrlIndex = boneMesh->attributeTable[i].AttribId;
                g_pDevice->SetMaterial(&(boneMesh->materials[mtrlIndex]));
                g_pDevice->SetTexture(0, boneMesh->textures[mtrlIndex]);
                g_pEffect->SetMatrix("matW", &identity);

                g_pEffect->SetTexture("texDiffuse", boneMesh->textures[mtrlIndex]);

                D3DXHANDLE hTech = g_pEffect->GetTechniqueByName("Skinning");
                g_pEffect->SetTechnique(hTech);
                g_pEffect->Begin(NULL, NULL);
                g_pEffect->BeginPass(0);

                boneMesh->MeshData.pMesh->DrawSubset(mtrlIndex);

                g_pEffect->EndPass();
                g_pEffect->End();

                //Also render all character decals with the same bone transforms
                for (int cd=0; cd<(int)boneMesh->m_decals.size(); cd++) {
                    boneMesh->m_decals[cd]->Render();
                }
            }
        }
        else if (0) {
            //Normal Static Mesh
            g_pEffect->SetMatrix("matW", &bone->CombinedTransformationMatrix);

            D3DXHANDLE hTech = g_pEffect->GetTechniqueByName("Lighting");
            g_pEffect->SetTechnique(hTech);

            //Render the mesh
            for (int i=0; i < (int)boneMesh->materials.size(); i++) {
                g_pDevice->SetMaterial(&boneMesh->materials[i]);
                g_pEffect->SetTexture("texDiffuse", boneMesh->textures[i]);

                g_pEffect->Begin(NULL, NULL);
                g_pEffect->BeginPass(0);

                boneMesh->OriginalMesh->DrawSubset(i);

                g_pEffect->EndPass();
                g_pEffect->End();
            }
        }
    }

    if (bone->pFrameSibling != NULL)Render((Bone*)bone->pFrameSibling);
    if (bone->pFrameFirstChild != NULL)Render((Bone*)bone->pFrameFirstChild);
}

void SkinnedMesh::SetupBoneMatrixPointers(Bone *bone) {
    if (bone->pMeshContainer != NULL) {
        BoneMesh *boneMesh = (BoneMesh*)bone->pMeshContainer;

        if (boneMesh->pSkinInfo != NULL) {
            int NumBones = boneMesh->pSkinInfo->GetNumBones();
            boneMesh->boneMatrixPtrs = new D3DXMATRIX*[NumBones];

            for (int i=0; i < NumBones; i++) {
                Bone *b = (Bone*)D3DXFrameFind(m_pRootBone, boneMesh->pSkinInfo->GetBoneName(i));
                if (b != NULL)boneMesh->boneMatrixPtrs[i] = &b->CombinedTransformationMatrix;
                else boneMesh->boneMatrixPtrs[i] = NULL;
            }
        }
    }

    if (bone->pFrameSibling != NULL)SetupBoneMatrixPointers((Bone*)bone->pFrameSibling);
    if (bone->pFrameFirstChild != NULL)SetupBoneMatrixPointers((Bone*)bone->pFrameFirstChild);
}

void SkinnedMesh::SetPose(D3DXMATRIX world) {
    UpdateMatrices((Bone*)m_pRootBone, &world);
}

void SkinnedMesh::GetAnimations(vector<string> &animations) {
    ID3DXAnimationSet *anim = NULL;

    for (int i=0; i<(int)m_pAnimControl->GetMaxNumAnimationSets(); i++) {
        anim = NULL;
        m_pAnimControl->GetAnimationSet(i, &anim);

        if (anim != NULL) {
            animations.push_back(anim->GetName());
            anim->Release();
        }
    }
}

ID3DXAnimationController* SkinnedMesh::GetController() {
    ID3DXAnimationController* newAnimController = NULL;

    if (m_pAnimControl != NULL) {
        m_pAnimControl->CloneAnimationController(m_pAnimControl->GetMaxNumAnimationOutputs(),
                m_pAnimControl->GetMaxNumAnimationSets(),
                m_pAnimControl->GetMaxNumTracks(),
                m_pAnimControl->GetMaxNumEvents(),
                &newAnimController);
    }

    return newAnimController;
}

D3DXFRAME* SkinnedMesh::GetBone(string name) {
    return D3DXFrameFind(m_pRootBone, name.c_str());
}

void SkinnedMesh::AddDecal(Bone* pBone) {
    if (pBone == NULL)
        pBone = (Bone*)m_pRootBone;

    //Attempt to add decal
    if (pBone->pMeshContainer != NULL) {
        BoneMesh* boneMesh = (BoneMesh*)pBone->pMeshContainer;
        boneMesh->AddDecal(g_rayOrg, g_rayDir, 0.1f);
    }

    if (pBone->pFrameSibling != NULL)
        AddDecal((Bone*)pBone->pFrameSibling);

    if (pBone->pFrameFirstChild != NULL)
        AddDecal((Bone*)pBone->pFrameFirstChild);
}
#include "character.h"

extern FaceFactory *g_pFaceFactory;
extern IDirect3DDevice9* g_pDevice;
extern ID3DXEffect *g_pEffect;
extern PhysicsEngine physicsEngine;

bool KeyDown(int vk_code);

Character::Character(char fileName[], D3DXMATRIX &world) : RagDoll(fileName, world) {
    //Create New Random face for the character
    m_pFace = NULL;
    if (g_pFaceFactory != NULL) {
        m_pFace = g_pFaceFactory->GenerateRandomFace();
    }

    //Create face controller
    m_pFaceController = new FaceController(D3DXVECTOR3(0.0f, 0.0f, 0.0f), m_pFace);

    //Face Vertex Format
    D3DVERTEXELEMENT9 faceVertexDecl[] = {
        //1st Stream: Base Mesh
        {0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        {0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0},
        {0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},

        //2nd Stream
        {1,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 1},
        {1, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   1},
        {1, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},

        //3rd Stream
        {2,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 2},
        {2, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   2},
        {2, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2},

        //4th Stream
        {3,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 3},
        {3, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   3},
        {3, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3},

        //5th Stream
        {4,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 4},
        {4, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   4},
        {4, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4},

        //Skeletal Info
        {5, 12, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 5},
        {5, 16, D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 5},

        D3DDECL_END()
    };

    //Create Face Vertex Declaration
    g_pDevice->CreateVertexDeclaration(faceVertexDecl, &m_pFaceVertexDecl);

    //Get Animation Control
    m_pAnimController = GetController();
    PlayAnimation("Walk");
    m_animation = 0;

    //Create IK Controller
    m_pIK = new InverseKinematics(this);
    m_lookAtIK = true;
    m_armIK = false;


    m_dead = false;
}

Character::~Character() {

}

void Character::Update(float deltaTime) {
    if (!m_dead) {
        m_pFaceController->Update(deltaTime);

        //Update Animation
        m_pAnimController->AdvanceTime(deltaTime * 0.7f, NULL);

        //KILL Character!
        if (KeyDown('K'))
            Kill();

        //Set IK
        if (KeyDown('L')) {
            m_lookAtIK = !m_lookAtIK;
            Sleep(300);
        }

        if (KeyDown('A')) {
            m_armIK = !m_armIK;
            Sleep(300);
        }

        //Generate a new Random Face
        if (KeyDown(VK_SPACE)) {
            delete m_pFaceController;
            delete m_pFace;
            m_pFace = g_pFaceFactory->GenerateRandomFace();
            m_pFaceController = new FaceController(D3DXVECTOR3(0.0f, 0.0f, 0.0f), m_pFace);
        }
    }
    else {
        RagDoll::Update(deltaTime);
    }
}

void Character::Render() {
    if (!m_dead) {
        //Update Keyframe Animation
        D3DXMATRIX world;
        D3DXMatrixIdentity(&world);
        SetPose(world);

        //Update IK
        if (m_lookAtIK)m_pIK->UpdateHeadIK();
        if (m_armIK)m_pIK->UpdateArmIK();

        //Switch Animation
        if (KeyDown(VK_RETURN)) {
            vector<string> animations;
            GetAnimations(animations);
            m_animation = (m_animation + 1) % (int)animations.size();
            PlayAnimation(animations[m_animation]);
            Sleep(300);
        }
    }
    else {
        //Update Ragdoll from physical representation
        UpdateSkeleton((Bone*)m_pRootBone);
    }

    //Render Character
    RenderMesh(NULL);

    //Render Eyes
    Bone* headBone = (Bone*)GetBone("Head");

    if (headBone != NULL) {
        //Rotate head matrix 90 degrees for eyes
        D3DXMATRIX rot;
        D3DXMatrixRotationZ(&rot, -D3DX_PI * 0.5f);
        m_pFaceController->m_headMatrix = rot * headBone->CombinedTransformationMatrix;
        m_pFaceController->RenderEyes();
    }
}

void Character::RenderMesh(Bone *bone) {
    if (bone == NULL)bone = (Bone*)m_pRootBone;

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

            if (strcmp(bone->Name, "Face") == 0) {
                //Set Head Matrix
                RenderFace(boneMesh);
            }
            else {
                //Render the mesh
                for (int i=0; i < (int)boneMesh->NumAttributeGroups; i++) {
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
                }
            }
        }
        /*      else        //In this example we ignore the static meshes (i.e. helmet + rifle)
                {
                    //Normal Static Mesh
                    g_pEffect->SetMatrix("matW", &bone->CombinedTransformationMatrix);

                    D3DXHANDLE hTech = g_pEffect->GetTechniqueByName("Lighting");
                    g_pEffect->SetTechnique(hTech);

                    //Render the mesh
                    for (int i=0;i < (int)boneMesh->materials.size();i++)
                    {
                        g_pDevice->SetMaterial(&boneMesh->materials[i]);
                        g_pEffect->SetTexture("texDiffuse", boneMesh->textures[i]);

                        g_pEffect->Begin(NULL, NULL);
                        g_pEffect->BeginPass(0);

                        boneMesh->OriginalMesh->DrawSubset(i);

                        g_pEffect->EndPass();
                        g_pEffect->End();
                    }
                }       */
    }

    if (bone->pFrameSibling != NULL)RenderMesh((Bone*)bone->pFrameSibling);
    if (bone->pFrameFirstChild != NULL)RenderMesh((Bone*)bone->pFrameFirstChild);
}

void Character::RenderFace(BoneMesh *pFacePlaceholder) {
    if (m_pFace == NULL || m_pFaceController == NULL || pFacePlaceholder == NULL)
        return;

    //Set Active Vertex Declaration
    g_pDevice->SetVertexDeclaration(m_pFaceVertexDecl);

    //Set Stream Sources
    m_pFace->SetStreamSources(m_pFaceController);

    //Add Bone Blending Info Stream
    DWORD vSize = D3DXGetFVFVertexSize(pFacePlaceholder->MeshData.pMesh->GetFVF());
    IDirect3DVertexBuffer9* headBuffer = NULL;
    pFacePlaceholder->MeshData.pMesh->GetVertexBuffer(&headBuffer);
    g_pDevice->SetStreamSource(5, headBuffer, 0, vSize);

    //Set Shader variables
    D3DXMATRIX view, proj, world;
    g_pDevice->GetTransform(D3DTS_VIEW, &view);
    g_pDevice->GetTransform(D3DTS_PROJECTION, &proj);
    D3DXVECTOR4 lightPos(-20.0f, 75.0f, -120.0f, 0.0f);
    D3DXMatrixIdentity(&world);

    g_pEffect->SetMatrix("matW", &world); //&pController->m_headMatrix);
    g_pEffect->SetMatrix("matVP", &(view * proj));
    g_pEffect->SetVector("lightPos", &lightPos);
    g_pEffect->SetTexture("texDiffuse", m_pFace->m_pFaceTexture);
    g_pEffect->SetVector("weights", &m_pFaceController->m_morphWeights);

    //Start Technique
    D3DXHANDLE hTech = g_pEffect->GetTechniqueByName("FaceBoneMorph");
    g_pEffect->SetTechnique(hTech);
    g_pEffect->Begin(NULL, NULL);
    g_pEffect->BeginPass(0);

    //Draw mesh
    g_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
                                    pFacePlaceholder->MeshData.pMesh->GetNumVertices(), 0,
                                    pFacePlaceholder->MeshData.pMesh->GetNumFaces());

    g_pEffect->EndPass();
    g_pEffect->End();

    //Restore vertex declaration and stream sources
    g_pDevice->SetVertexDeclaration(NULL);

    for (int i=0; i<6; i++) {
        g_pDevice->SetStreamSource(i, NULL, 0, 0);
    }
}

void Character::PlayAnimation(string name) {
    ID3DXAnimationSet *anim = NULL;

    for (int i=0; i<(int)m_pAnimController->GetMaxNumAnimationSets(); i++) {
        anim = NULL;
        m_pAnimController->GetAnimationSet(i, &anim);

        if (anim != NULL) {
            if (strcmp(anim->GetName(), name.c_str()) == 0) {
                m_pAnimController->SetTrackAnimationSet(0, anim);
                anim->Release();
                return;
            }

            anim->Release();
        }
    }
}

void Character::Kill() {
    m_dead=true;
    InitRagdoll();
    physicsEngine.m_ragdolls.push_back(this);
}
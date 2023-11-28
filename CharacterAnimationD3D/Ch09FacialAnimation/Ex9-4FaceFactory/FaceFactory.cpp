#include "faceFactory.h"

extern IDirect3DDevice9* g_pDevice;
extern ofstream g_debug;

struct FACEVERTEX {
    D3DXVECTOR3 m_position;
    D3DXVECTOR3 m_normal;
    D3DXVECTOR2 m_uv;

    static DWORD FVF;
};
DWORD FACEVERTEX::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX0;


FaceFactory::FaceFactory(string filename) {
    //Load Face Meshes
    FaceHierarchyLoader hierarchy;
    D3DXFRAME *root = NULL;
    D3DXLoadMeshHierarchyFromX(filename.c_str(), D3DXMESH_MANAGED,
                               g_pDevice, &hierarchy, NULL, &root, NULL);

    //Extract Face Meshes
    ExtractMeshes(root);

    //Destroy temporary hierarchy
    hierarchy.DestroyFrame(root);

    //Load textures
    string fNames[8] = {"resources/face01.jpg", "resources/face02.jpg", "resources/face03.jpg", "resources/face04.jpg",
                        "resources/face05.jpg", "resources/face06.jpg", "resources/face07.jpg", "resources/face08.jpg"
                       };

    for (int i=0; i<8; i++) {
        IDirect3DTexture9* faceTexture = NULL;
        D3DXCreateTextureFromFile(g_pDevice, fNames[i].c_str(), &faceTexture);
        m_faceTextures.push_back(faceTexture);
    }

}

FaceFactory::~FaceFactory() {
    if (m_pBaseMesh != NULL)     m_pBaseMesh->Release();
    if (m_pBlinkMesh != NULL)    m_pBlinkMesh->Release();
    if (m_pEyeMesh != NULL)      m_pEyeMesh->Release();

    for (int i=0; i<(int)m_emotionMeshes.size(); i++) {
        m_emotionMeshes[i]->Release();
    }

    for (int i=0; i<(int)m_speechMeshes.size(); i++) {
        m_speechMeshes[i]->Release();
    }

    for (int i=0; i<(int)m_morphMeshes.size(); i++) {
        m_morphMeshes[i]->Release();
    }
}

void FaceFactory::ExtractMeshes(D3DXFRAME *frame) {
    if (frame == NULL)
        return;

    //Extract Mesh
    if (frame->pMeshContainer != NULL) {
        ID3DXMesh *mesh = (ID3DXMesh*)frame->pMeshContainer;

        if (frame->Name != NULL) {
            if (strcmp(frame->Name, "Base") == 0) {      //Base Mesh
                m_pBaseMesh = mesh;
                mesh->AddRef();
            }
            else if (strcmp(frame->Name, "Blink") == 0) { //Blink Mesh
                m_pBlinkMesh = mesh;
                mesh->AddRef();
            }
            else if (strcmp(frame->Name, "Emotion") == 0) { //Emotion Mesh
                m_emotionMeshes.push_back(mesh);
                mesh->AddRef();
            }
            else if (strcmp(frame->Name, "Speech") == 0) { //Speech Mesh
                m_speechMeshes.push_back(mesh);
                mesh->AddRef();
            }
            else {                                      //The mesh is a morph target
                g_debug << "Face Target: " << frame->Name << "\n";
                m_morphMeshes.push_back(mesh);
                mesh->AddRef();
            }
        }
    }

    //Check siblings and children
    ExtractMeshes(frame->pFrameSibling);
    ExtractMeshes(frame->pFrameFirstChild);
}

Face* FaceFactory::GenerateRandomFace() {
    //Create random 0.0f - 1.0f morph weight for each morph target
    vector<float> morphWeights;
    for (int i=0; i<(int)m_morphMeshes.size(); i++) {
        float w = (rand()%1000) / 1000.0f;
        morphWeights.push_back(w);
    }

    //next create a new empty face
    Face *face = new Face();

    //Then clone base, blink and all emotion + speech meshes
    face->m_pBaseMesh = CreateMorphTarget(m_pBaseMesh, morphWeights);
    face->m_pBlinkMesh = CreateMorphTarget(m_pBlinkMesh, morphWeights);

    for (int i=0; i<(int)m_emotionMeshes.size(); i++) {
        face->m_emotionMeshes.push_back(CreateMorphTarget(m_emotionMeshes[i], morphWeights));
    }

    for (int i=0; i<(int)m_speechMeshes.size(); i++) {
        face->m_speechMeshes.push_back(CreateMorphTarget(m_speechMeshes[i], morphWeights));
    }

    //Set a random face texture as well
    int index = rand() % (int)m_faceTextures.size();
    m_faceTextures[index]->AddRef();
    face->m_pFaceTexture = m_faceTextures[index];

    //Return the new random face
    return face;
}

ID3DXMesh* FaceFactory::CreateMorphTarget(ID3DXMesh* mesh, vector<float> &morphWeights) {
    if (mesh == NULL || m_pBaseMesh == NULL)
        return NULL;

    //Clone mesh
    ID3DXMesh* newMesh = NULL;
    if (FAILED(mesh->CloneMeshFVF(D3DXMESH_MANAGED, mesh->GetFVF(), g_pDevice, &newMesh))) {
        g_debug << "Failed to clone mesh!\n";
        return NULL;
    }

    //Copy base mesh data
    FACEVERTEX *vDest = NULL, *vSrc = NULL, *vMorph = NULL, *vBase = NULL;
    mesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&vSrc);
    newMesh->LockVertexBuffer(0, (void**)&vDest);
    m_pBaseMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&vBase);

    for (int i=0; i < (int)mesh->GetNumVertices(); i++) {
        vDest[i].m_position = vSrc[i].m_position;
        vDest[i].m_normal = vSrc[i].m_normal;
        vDest[i].m_uv = vSrc[i].m_uv;
    }

    mesh->UnlockVertexBuffer();

    //Morph base mesh using the provided weights
    for (int m=0; m<(int)m_morphMeshes.size(); m++) {
        if (m_morphMeshes[m]->GetNumVertices() == mesh->GetNumVertices()) {
            m_morphMeshes[m]->LockVertexBuffer(D3DLOCK_READONLY, (void**)&vMorph);

            for (int i=0; i < (int)mesh->GetNumVertices(); i++) {
                vDest[i].m_position += (vMorph[i].m_position - vBase[i].m_position) * morphWeights[m];
                vDest[i].m_normal += (vMorph[i].m_normal - vBase[i].m_normal) * morphWeights[m];
            }

            m_morphMeshes[m]->UnlockVertexBuffer();
        }
    }

    newMesh->UnlockVertexBuffer();
    m_pBaseMesh->UnlockVertexBuffer();

    return newMesh;
}
#include "face.h"

#pragma warning(disable:4996)

extern IDirect3DDevice9* g_pDevice;
extern ID3DXEffect *g_pEffect;
extern ofstream g_debug;

//////////////////////////////////////////////////////////////////////////////////////////////////
//                                  FaceHierarchyLoader                                         //
//////////////////////////////////////////////////////////////////////////////////////////////////

class FaceHierarchyLoader: public ID3DXAllocateHierarchy {
public:
    STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame);
    STDMETHOD(CreateMeshContainer)(THIS_ LPCTSTR Name, CONST D3DXMESHDATA * pMeshData, CONST D3DXMATERIAL * pMaterials, CONST D3DXEFFECTINSTANCE * pEffectInstances, DWORD NumMaterials, CONST DWORD * pAdjacency, LPD3DXSKININFO pSkinInfo, LPD3DXMESHCONTAINER * ppNewMeshContainer);
    STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree);
    STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerBase);
};

HRESULT FaceHierarchyLoader::CreateFrame(LPCSTR Name, LPD3DXFRAME *ppNewFrame) {
    D3DXFRAME *newBone = new D3DXFRAME;
    memset(newBone, 0, sizeof(D3DXFRAME));

    //Copy name
    if (Name != NULL) {
        newBone->Name = new char[strlen(Name)+1];
        strcpy(newBone->Name, Name);
    }

    //Set the transformation matrices
    D3DXMatrixIdentity(&newBone->TransformationMatrix);

    //Return the new bone...
    *ppNewFrame = newBone;

    return S_OK;
}

HRESULT FaceHierarchyLoader::CreateMeshContainer(LPCSTR Name,
        CONST D3DXMESHDATA *pMeshData,
        CONST D3DXMATERIAL *pMaterials,
        CONST D3DXEFFECTINSTANCE *pEffectInstances,
        DWORD NumMaterials,
        CONST DWORD *pAdjacency,
        LPD3DXSKININFO pSkinInfo,
        LPD3DXMESHCONTAINER *ppNewMeshContainer) {
    //Add Reference so that the mesh isn't deallocated
    pMeshData->pMesh->AddRef();

    //Return a pointer to the mesh casted to a D3DXMESHCONTAINER pointer
    *ppNewMeshContainer = (D3DXMESHCONTAINER*)pMeshData->pMesh;

    return S_OK;
}

HRESULT FaceHierarchyLoader::DestroyFrame(LPD3DXFRAME pFrameToFree) {
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

HRESULT FaceHierarchyLoader::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase) {
    ID3DXMesh *m = (ID3DXMesh*)pMeshContainerBase;
    m->Release();
    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Face                                                        //
//////////////////////////////////////////////////////////////////////////////////////////////////

Face::Face(string filename) {
    m_pBaseMesh =  NULL;
    m_pBlinkMesh = NULL;
    m_pFaceTexture = NULL;

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

        D3DDECL_END()
    };

    //Create Face Vertex Declaration
    g_pDevice->CreateVertexDeclaration(faceVertexDecl, &m_pFaceVertexDecl);

    //Load Face Meshes
    FaceHierarchyLoader hierarchy;
    D3DXFRAME *root = NULL;
    D3DXLoadMeshHierarchyFromX(filename.c_str(), D3DXMESH_MANAGED,
                               g_pDevice, &hierarchy, NULL, &root, NULL);

    //Extract Face Meshes
    ExtractMeshes(root);

    //Destroy temporary hierarchy
    hierarchy.DestroyFrame(root);

    //Load texture
    D3DXCreateTextureFromFile(g_pDevice, "resources/face.jpg", &m_pFaceTexture);

    //Init Eyes
    m_eyes[0].Init(D3DXVECTOR3(-0.037f, 0.04f, -0.057f));
    m_eyes[1].Init(D3DXVECTOR3( 0.037f, 0.04f, -0.057f));
}

Face::~Face() {
    if (m_pBaseMesh != NULL)     m_pBaseMesh->Release();
    if (m_pBlinkMesh != NULL)    m_pBlinkMesh->Release();

    for (int i=0; i<(int)m_emotionMeshes.size(); i++) {
        m_emotionMeshes[i]->Release();
    }

    for (int i=0; i<(int)m_speechMeshes.size(); i++) {
        m_speechMeshes[i]->Release();
    }

    if (m_pFaceVertexDecl != NULL)   m_pFaceVertexDecl->Release();
    if (m_pFaceTexture != NULL)      m_pFaceTexture->Release();
}

void Face::ExtractMeshes(D3DXFRAME *frame) {
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
        }
    }

    //Check siblings and children
    ExtractMeshes(frame->pFrameSibling);
    ExtractMeshes(frame->pFrameFirstChild);
}

void Face::TempUpdate() {
    //Update Morph Weights
    float time = GetTickCount() * 0.002f;

    m_morphWeights = D3DXVECTOR4(sin(time * 1.0f),
                                 cos(time * 0.9f),
                                 sin(time * 0.7f),
                                 cos(time * 1.2f));

    //Get look at point from cursor location
    POINT p;
    GetCursorPos(&p);

    float x = (p.x - 400) / 800.0f;
    float y = (p.y - 300) / 600.0f;

    //Tell both Eyes to look at this point
    m_eyes[0].LookAt(D3DXVECTOR3(x, y, -1.0f));
    m_eyes[1].LookAt(D3DXVECTOR3(x, y, -1.0f));
}

void Face::Render() {
    if (m_pBaseMesh == NULL)
        return;

    //Set Active Vertex Declaration
    g_pDevice->SetVertexDeclaration(m_pFaceVertexDecl);

    //Set streams
    DWORD vSize = D3DXGetFVFVertexSize(m_pBaseMesh->GetFVF());
    IDirect3DVertexBuffer9* baseMeshBuffer = NULL;
    m_pBaseMesh->GetVertexBuffer(&baseMeshBuffer);
    g_pDevice->SetStreamSource(0, baseMeshBuffer, 0, vSize);

    //Temp: Set some random source meshes
    for (int i=0; i<4; i++) {
        IDirect3DVertexBuffer9* meshBuffer = NULL;

        if (m_morphWeights[i] >= 0.0f) {
            m_emotionMeshes[i]->GetVertexBuffer(&meshBuffer);
        }
        else {
            m_speechMeshes[i]->GetVertexBuffer(&meshBuffer);
            m_morphWeights[i] *= -1.0f;
        }

        g_pDevice->SetStreamSource(i + 1, meshBuffer, 0, vSize);
    }

    //Set Index buffer
    IDirect3DIndexBuffer9* ib = NULL;
    m_pBaseMesh->GetIndexBuffer(&ib);
    g_pDevice->SetIndices(ib);

    //Set Shader variables
    D3DXMATRIX world, view, proj;
    g_pDevice->GetTransform(D3DTS_VIEW, &view);
    g_pDevice->GetTransform(D3DTS_PROJECTION, &proj);
    D3DXMatrixIdentity(&world);
    D3DXVECTOR4 lightPos(-20.0f, 75.0f, -120.0f, 0.0f);

    g_pEffect->SetMatrix("matW", &world);
    g_pEffect->SetMatrix("matVP", &(view * proj));
    g_pEffect->SetVector("lightPos", &lightPos);
    g_pEffect->SetTexture("texDiffuse", m_pFaceTexture);
    g_pEffect->SetVector("weights", &m_morphWeights);

    //Start Technique
    D3DXHANDLE hTech = g_pEffect->GetTechniqueByName("FaceMorph");
    g_pEffect->SetTechnique(hTech);
    g_pEffect->Begin(NULL, NULL);
    g_pEffect->BeginPass(0);

    //Draw mesh
    g_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
                                    m_pBaseMesh->GetNumVertices(), 0,
                                    m_pBaseMesh->GetNumFaces());

    g_pEffect->EndPass();
    g_pEffect->End();

    //Restore vertex declaration and stream sources
    g_pDevice->SetVertexDeclaration(NULL);

    for (int i=0; i<5; i++) {
        g_pDevice->SetStreamSource(i, NULL, 0, 0);
    }

    //Render Eyes
    hTech = g_pEffect->GetTechniqueByName("Lighting");
    g_pEffect->SetTechnique(hTech);
    g_pEffect->Begin(NULL, NULL);
    g_pEffect->BeginPass(0);

    m_eyes[0].Render(g_pEffect);
    m_eyes[1].Render(g_pEffect);

    g_pEffect->EndPass();
    g_pEffect->End();
}
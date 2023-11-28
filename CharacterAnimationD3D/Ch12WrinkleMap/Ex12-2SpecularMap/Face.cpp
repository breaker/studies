#include "face.h"
#include "faceController.h"

#pragma warning(disable:4996)

extern IDirect3DDevice9* g_pDevice;
extern ID3DXEffect *g_pEffect;
extern ofstream g_debug;

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

void AddTangentBinormal(ID3DXMesh** pMesh) {
    //Get vertex declaration from mesh
    D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE];
    (*pMesh)->GetDeclaration(decl);

    //Find the end index of the declaration
    int index = 0;
    while (decl[index].Type != D3DDECLTYPE_UNUSED) {
        index++;
    }

    //Get size of last element
    int size = 0;

    switch(decl[index - 1].Type) {
    case D3DDECLTYPE_FLOAT1:
        size = 4;
        break;

    case D3DDECLTYPE_FLOAT2:
        size = 8;
        break;

    case D3DDECLTYPE_FLOAT3:
        size = 12;
        break;

    case D3DDECLTYPE_FLOAT4:
        size = 16;
        break;

    case D3DDECLTYPE_D3DCOLOR:
        size = 4;
        break;

    case D3DDECLTYPE_UBYTE4:
        size = 4;
        break;

    default:
        g_debug << "Unhandled declaration type: " << decl[index - 1].Type << "\n";
    };

    //Create Tangent Element
    D3DVERTEXELEMENT9 tangent = {
        0,
        decl[index - 1].Offset + size,
        D3DDECLTYPE_FLOAT3,
        D3DDECLMETHOD_DEFAULT,
        D3DDECLUSAGE_TANGENT,
        0
    };

    //Create BiNormal Element
    D3DVERTEXELEMENT9 binormal = {
        0,
        tangent.Offset + 12,
        D3DDECLTYPE_FLOAT3,
        D3DDECLMETHOD_DEFAULT,
        D3DDECLUSAGE_BINORMAL,
        0
    };

    //End element
    D3DVERTEXELEMENT9 endElement = D3DDECL_END();

    //Add new elements to vertex declaration
    decl[index++] = tangent;
    decl[index++] = binormal;
    decl[index] = endElement;

    //Convert mesh to the new vertex declaration
    ID3DXMesh* pNewMesh = NULL;

    if (FAILED((*pMesh)->CloneMesh((*pMesh)->GetOptions(),
                                  decl,
                                  g_pDevice,
                                  &pNewMesh))) {
        g_debug << "Failed to clone mesh\n";
        return;
    }

    //Compute the tangets & binormals
    if (FAILED(D3DXComputeTangentFrame(pNewMesh, NULL))) {
        g_debug << "Failed to compute tangents & binormals for new mesh\n";
        return;
    }

    //Release old mesh
    (*pMesh)->Release();

    //Assign new mesh to the mesh pointer
    *pMesh = pNewMesh;
}

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
    ID3DXMesh* pSrcMesh = pMeshData->pMesh;

    D3DXMESHCONTAINER* pContainer = new D3DXMESHCONTAINER();
    memset(pContainer, 0, sizeof(D3DXMESHCONTAINER));

    pSrcMesh->CloneMeshFVF(pSrcMesh->GetOptions(),
                           pSrcMesh->GetFVF(),
                           g_pDevice,
                           &pContainer->MeshData.pMesh);

    *ppNewMeshContainer = pContainer;
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
    pMeshContainerBase->MeshData.pMesh->Release();
    delete pMeshContainerBase;
    pMeshContainerBase = NULL;
    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Face                                                        //
//////////////////////////////////////////////////////////////////////////////////////////////////

Face::Face(string filename) {
    m_pBaseMesh =  NULL;
    m_pBlinkMesh = NULL;
    m_pFaceTexture = NULL;
    m_pFaceNormalMap = NULL;
    m_pFaceSpecularMap = NULL;

    //Face Vertex Format
    D3DVERTEXELEMENT9 faceVertexDecl[] = {
        //1st Stream: Base Mesh
        {0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        {0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0},
        {0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
        {0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0},
        {0, 44, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0},

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

    //Add tangents & binormal to base mesh
    AddTangentBinormal(&m_pBaseMesh);

    //Destroy temporary hierarchy
    hierarchy.DestroyFrame(root);

    //Load textures
    D3DXCreateTextureFromFile(g_pDevice, "resources/face.jpg", &m_pFaceTexture);
    D3DXCreateTextureFromFile(g_pDevice, "resources/face_normal.tga", &m_pFaceNormalMap);
    D3DXCreateTextureFromFile(g_pDevice, "resources/face_specular.tga", &m_pFaceSpecularMap);

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
    if (g_pEffect != NULL)           g_pEffect->Release();
}

void Face::ExtractMeshes(D3DXFRAME *frame) {
    if (frame == NULL)
        return;

    //Extract Mesh
    if (frame->pMeshContainer != NULL) {
        ID3DXMesh *mesh = frame->pMeshContainer->MeshData.pMesh;

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

void Face::Render(FaceController *pController) {
    if (m_pBaseMesh == NULL || pController == NULL)
        return;

    //Set Active Vertex Declaration
    g_pDevice->SetVertexDeclaration(m_pFaceVertexDecl);

    //Set streams
    D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE];
    m_pBaseMesh->GetDeclaration(decl);
    DWORD vSize = D3DXGetDeclVertexSize(decl, 0);
    IDirect3DVertexBuffer9* baseMeshBuffer = NULL;
    m_pBaseMesh->GetVertexBuffer(&baseMeshBuffer);
    g_pDevice->SetStreamSource(0, baseMeshBuffer, 0, vSize);

    //Set Blink Source
    vSize = D3DXGetFVFVertexSize(m_pBlinkMesh->GetFVF());
    IDirect3DVertexBuffer9* blinkBuffer = NULL;
    m_pBlinkMesh->GetVertexBuffer(&blinkBuffer);
    g_pDevice->SetStreamSource(1, blinkBuffer, 0, vSize);

    //Set Emotion Source
    IDirect3DVertexBuffer9* emotionBuffer = NULL;
    m_emotionMeshes[pController->m_emotionIndex]->GetVertexBuffer(&emotionBuffer);
    g_pDevice->SetStreamSource(2, emotionBuffer, 0, vSize);

    //Set Speech Sources
    for (int i=0; i<2; i++) {
        IDirect3DVertexBuffer9* speechBuffer = NULL;
        m_speechMeshes[pController->m_speechIndices[i]]->GetVertexBuffer(&speechBuffer);
        g_pDevice->SetStreamSource(3 + i, speechBuffer, 0, vSize);
    }

    //Set Index buffer
    IDirect3DIndexBuffer9* ib = NULL;
    m_pBaseMesh->GetIndexBuffer(&ib);
    g_pDevice->SetIndices(ib);

    //Set Shader variables
    D3DXMATRIX view, proj;
    g_pDevice->GetTransform(D3DTS_VIEW, &view);
    g_pDevice->GetTransform(D3DTS_PROJECTION, &proj);

    g_pEffect->SetMatrix("matW", &pController->m_headMatrix);
    g_pEffect->SetMatrix("matVP", &(view * proj));
    g_pEffect->SetTexture("texDiffuse", m_pFaceTexture);
    g_pEffect->SetTexture("texNormalMap", m_pFaceNormalMap);
    g_pEffect->SetTexture("texSpecularMap", m_pFaceSpecularMap);
    g_pEffect->SetVector("weights", &pController->m_morphWeights);

    //Start Technique
    D3DXHANDLE hTech = g_pEffect->GetTechniqueByName("FaceMorphNormalMap");
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
}
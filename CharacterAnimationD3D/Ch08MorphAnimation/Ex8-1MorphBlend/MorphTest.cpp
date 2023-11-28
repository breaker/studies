#include "morphTest.h"

extern IDirect3DDevice9* g_pDevice;
extern ofstream g_debug;
extern ID3DXEffect *g_pEffect;

bool KeyDown(int vk_code);
bool KeyUp(int vk_code);

MorphTest::MorphTest() {
    m_pTarget01 = NULL;
    m_pTarget02 = NULL;
    m_pFace = NULL;
    m_blend = 0.0f;
}

void MorphTest::Init() {
    //Load Targets
    D3DXLoadMeshFromX("resources/face01.x", D3DXMESH_MANAGED, g_pDevice, NULL, NULL, NULL, NULL, &m_pTarget01);
    D3DXLoadMeshFromX("resources/face02.x", D3DXMESH_MANAGED, g_pDevice, NULL, NULL, NULL, NULL, &m_pTarget02);

    //Create the face mesh as a clone of one of the target meshes
    m_pTarget01->CloneMeshFVF(D3DXMESH_MANAGED, m_pTarget01->GetFVF(), g_pDevice, &m_pFace);
}

void MorphTest::Update(float deltaTime) {
    bool updateMesh = false;

    if (KeyDown(VK_UP)) {
        m_blend = min(m_blend + deltaTime * 0.5f, 2.0f);
        updateMesh = true;
    }

    if (KeyDown(VK_DOWN)) {
        m_blend = max(m_blend - deltaTime * 0.5f, -1.0f);
        updateMesh = true;
    }

    //Update Mesh
    if (updateMesh) {
        BYTE *target01, *target02, *face;

        //Lock vertex buffers
        m_pTarget01->LockVertexBuffer(D3DLOCK_READONLY, (void**)&target01);
        m_pTarget02->LockVertexBuffer(D3DLOCK_READONLY, (void**)&target02);
        m_pFace->LockVertexBuffer(0, (void**)&face);

        for (int i=0; i<(int)m_pFace->GetNumVertices(); i++) {
            //Get position of the two target vertices
            D3DXVECTOR3 t1 = *((D3DXVECTOR3*)target01);
            D3DXVECTOR3 t2 = *((D3DXVECTOR3*)target02);
            D3DXVECTOR3 *f = (D3DXVECTOR3*)face;

            //Perform morphing
            *f = t2 * m_blend + t1 * (1.0f - m_blend);

            //Move to next vertex
            target01 += m_pTarget01->GetNumBytesPerVertex();
            target02 += m_pTarget01->GetNumBytesPerVertex();
            face += m_pFace->GetNumBytesPerVertex();
        }

        //Unlock all vertex buffers
        m_pTarget01->UnlockVertexBuffer();
        m_pTarget02->UnlockVertexBuffer();
        m_pFace->UnlockVertexBuffer();
    }
}

void MorphTest::Render() {
    D3DXMATRIX i, view, proj;
    g_pDevice->GetTransform(D3DTS_VIEW, &view);
    g_pDevice->GetTransform(D3DTS_PROJECTION, &proj);
    D3DXMatrixIdentity(&i);
    D3DXVECTOR4 lightPos(-20.0f, 75.0f, -120.0f, 0.0f);

    g_pEffect->SetMatrix("matW", &i);
    g_pEffect->SetMatrix("matVP", &(view * proj));
    g_pEffect->SetVector("lightPos", &lightPos);
    D3DXHANDLE hTech = g_pEffect->GetTechniqueByName("Lighting");
    g_pEffect->SetTechnique(hTech);
    g_pEffect->Begin(NULL, NULL);
    g_pEffect->BeginPass(0);

    m_pFace->DrawSubset(0);

    g_pEffect->EndPass();
    g_pEffect->End();
}


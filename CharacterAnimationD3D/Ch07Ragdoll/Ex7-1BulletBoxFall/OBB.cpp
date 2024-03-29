#include "OBB.h"
#include "physicsEngine.h"

extern IDirect3DDevice9* g_pDevice;
extern ID3DXFont *g_pFont;
extern ofstream g_debug;
extern ID3DXEffect *g_pEffect;

OBB::OBB(D3DXVECTOR3 pos, D3DXVECTOR3 size, bool dynamic) {
    //Create Mesh
    m_pMesh = NULL;
    D3DXCreateBox(g_pDevice, size.x, size.y, size.z, &m_pMesh, NULL);

    //Create Motion State
    btQuaternion q(0.0f, 0.0f, 0.0f);
    btVector3 p(pos.x, pos.y, pos.z);
    btTransform startTrans(q, p);
    btMotionState *ms = new btDefaultMotionState(startTrans);

    //Create Collision Shape
    btCollisionShape *cs = new btBoxShape(btVector3(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f));

    //Create Rigid Body
    float mass = size.x * size.y * size.z;
    btVector3 localInertia(0,0,0);
    cs->calculateLocalInertia(mass,localInertia);
    if (!dynamic)mass = 0.0f;

    m_pBody = new btRigidBody(mass, ms, cs, localInertia);
}

OBB::~OBB() {
    Release();
}

void OBB::Release() {
    if (m_pMesh != NULL)
        m_pMesh->Release();

    if (m_pBody != NULL)
        delete m_pBody;
}

void OBB::Render() {
    btMotionState *ms = m_pBody->getMotionState();
    if (ms == NULL)return;

    g_pEffect->SetMatrix("matW", &BT2DX_MATRIX(*ms));

    D3DXHANDLE hTech = g_pEffect->GetTechniqueByName("Lighting");
    g_pEffect->SetTechnique(hTech);
    g_pEffect->Begin(NULL, NULL);
    g_pEffect->BeginPass(0);

    m_pMesh->DrawSubset(0);

    g_pEffect->EndPass();
    g_pEffect->End();
}
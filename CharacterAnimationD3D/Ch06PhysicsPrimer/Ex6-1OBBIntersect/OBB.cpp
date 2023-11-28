#include "OBB.h"

extern IDirect3DDevice9* g_pDevice;
extern ID3DXFont *g_pFont;
extern ID3DXEffect *g_pEffect;
extern ofstream g_debug;

OBB::OBB() {
    m_pMesh = NULL;
    m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    D3DXQuaternionIdentity(&m_rot);
    SetSize(D3DXVECTOR3(1.0f, 1.0f, 1.0f));
}

OBB::OBB(D3DXVECTOR3 size) {
    m_pMesh = NULL;
    m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    D3DXQuaternionIdentity(&m_rot);
    SetSize(size);
}

OBB::~OBB() {
    Release();
}

void OBB::Release() {
    if (m_pMesh != NULL) {
        m_pMesh->Release();
        m_pMesh = NULL;
    }
}

void OBB::SetSize(D3DXVECTOR3 size) {
    m_size = size;
    Release();
    D3DXCreateBox(g_pDevice, m_size.x * 2.0f, m_size.y * 2.0f, m_size.z * 2.0f, &m_pMesh, NULL);
    m_radius = D3DXVec3Length(&m_size);
}

void OBB::Render() {
    if (m_pMesh != NULL) {
        D3DXMATRIX p, r, world;
        D3DXMatrixTranslation(&p, m_pos.x, m_pos.y, m_pos.z);
        D3DXMatrixRotationQuaternion(&r, &m_rot);
        D3DXMatrixMultiply(&world, &r, &p);
        g_pEffect->SetMatrix("matW", &world);

        D3DXHANDLE hTech = g_pEffect->GetTechniqueByName("Lighting");
        g_pEffect->SetTechnique(hTech);
        g_pEffect->Begin(NULL, NULL);
        g_pEffect->BeginPass(0);

        m_pMesh->DrawSubset(0);

        g_pEffect->EndPass();
        g_pEffect->End();
    }
}

bool OBB::Intersect(OBB &b) {
    //Cheap Sphere-Sphere intersection test
    float l = D3DXVec3Length(&(b.m_pos - m_pos));
    if (l > (b.m_radius + m_radius)) {
        return false;
    }

    //More expensive OBB-OBB intersection test
    D3DXMATRIX r1, r2, matB;
    D3DXMatrixRotationQuaternion(&r1, &m_rot);
    D3DXMatrixRotationQuaternion(&r2, &b.m_rot);

    D3DXMatrixInverse(&r1, NULL, &r1);
    D3DXMatrixMultiply(&matB, &r2, &r1);
    D3DXVECTOR4 vPosB;
    D3DXVec3Transform(&vPosB, &(b.m_pos - m_pos), &r1);

    D3DXVECTOR3 XAxis(matB._11,matB._21,matB._31);
    D3DXVECTOR3 YAxis(matB._12,matB._22,matB._32);
    D3DXVECTOR3 ZAxis(matB._13,matB._23,matB._33);

    //15 tests
    //1 (Ra)x
    if (fabs(vPosB.x) > m_size.x + b.m_size.x * fabs(XAxis.x) + b.m_size.y * fabs(XAxis.y) + b.m_size.z * fabs(XAxis.z))
        return false;
    //2 (Ra)y
    if (fabs(vPosB.y) > m_size.y + b.m_size.x * fabs(YAxis.x) + b.m_size.y * fabs(YAxis.y) + b.m_size.z * fabs(YAxis.z))
        return false;
    //3 (Ra)z
    if (fabs(vPosB.z) > m_size.z + b.m_size.x * fabs(ZAxis.x) + b.m_size.y * fabs(ZAxis.y) + b.m_size.z * fabs(ZAxis.z))
        return false;

    //4 (Rb)x
    if (fabs(vPosB.x*XAxis.x+vPosB.y*YAxis.x+vPosB.z*ZAxis.x) > (b.m_size.x+m_size.x*fabs(XAxis.x) + m_size.y * fabs(YAxis.x) + m_size.z*fabs(ZAxis.x)))
        return false;
    //5 (Rb)y
    if (fabs(vPosB.x*XAxis.y+vPosB.y*YAxis.y+vPosB.z*ZAxis.y) > (b.m_size.y+m_size.x*fabs(XAxis.y) + m_size.y * fabs(YAxis.y) + m_size.z*fabs(ZAxis.y)))
        return false;
    //6 (Rb)z
    if (fabs(vPosB.x*XAxis.z+vPosB.y*YAxis.z+vPosB.z*ZAxis.z) > (b.m_size.z+m_size.x*fabs(XAxis.z) + m_size.y * fabs(YAxis.z) + m_size.z*fabs(ZAxis.z)))
        return false;

    //7 (Ra)x X (Rb)x
    if (fabs(vPosB.z*YAxis.x-vPosB.y*ZAxis.x) > m_size.y*fabs(ZAxis.x) + m_size.z*fabs(YAxis.x) + b.m_size.y*fabs(XAxis.z) + b.m_size.z*fabs(XAxis.y))
        return false;
    //8 (Ra)x X (Rb)y
    if (fabs(vPosB.z*YAxis.y-vPosB.y*ZAxis.y) > m_size.y*fabs(ZAxis.y) + m_size.z*fabs(YAxis.y) + b.m_size.x*fabs(XAxis.z) + b.m_size.z*fabs(XAxis.x))
        return false;
    //9 (Ra)x X (Rb)z
    if (fabs(vPosB.z*YAxis.z-vPosB.y*ZAxis.z) > m_size.y*fabs(ZAxis.z) + m_size.z*fabs(YAxis.z) + b.m_size.x*fabs(XAxis.y) + b.m_size.y*fabs(XAxis.x))
        return false;

    //10 (Ra)y X (Rb)x
    if (fabs(vPosB.x*ZAxis.x-vPosB.z*XAxis.x) > m_size.x*fabs(ZAxis.x) + m_size.z*fabs(XAxis.x) + b.m_size.y*fabs(YAxis.z) + b.m_size.z*fabs(YAxis.y))
        return false;
    //11 (Ra)y X (Rb)y
    if (fabs(vPosB.x*ZAxis.y-vPosB.z*XAxis.y) > m_size.x*fabs(ZAxis.y) + m_size.z*fabs(XAxis.y) + b.m_size.x*fabs(YAxis.z) + b.m_size.z*fabs(YAxis.x))
        return false;
    //12 (Ra)y X (Rb)z
    if (fabs(vPosB.x*ZAxis.z-vPosB.z*XAxis.z) > m_size.x*fabs(ZAxis.z) + m_size.z*fabs(XAxis.z) + b.m_size.x*fabs(YAxis.y) + b.m_size.y*fabs(YAxis.x))
        return false;

    //13 (Ra)z X (Rb)x
    if (fabs(vPosB.y*XAxis.x-vPosB.x*YAxis.x) > m_size.x*fabs(YAxis.x) + m_size.y*fabs(XAxis.x) + b.m_size.y*fabs(ZAxis.z) + b.m_size.z*fabs(ZAxis.y))
        return false;
    //14 (Ra)z X (Rb)y
    if (fabs(vPosB.y*XAxis.y-vPosB.x*YAxis.y) > m_size.x*fabs(YAxis.y) + m_size.y*fabs(XAxis.y) + b.m_size.x*fabs(ZAxis.z) + b.m_size.z*fabs(ZAxis.x))
        return false;
    //15 (Ra)z X (Rb)z
    if (fabs(vPosB.y*XAxis.z-vPosB.x*YAxis.z) > m_size.x*fabs(YAxis.z) + m_size.y*fabs(XAxis.z) + b.m_size.x*fabs(ZAxis.y) + b.m_size.y*fabs(ZAxis.x))
        return false;

    return true;
}

bool OBB::Intersect(Ray &ray) {
    D3DXMATRIX p, r, world;
    D3DXMatrixTranslation(&p, m_pos.x, m_pos.y, m_pos.z);
    D3DXMatrixRotationQuaternion(&r, &m_rot);

    D3DXMatrixMultiply(&world, &r, &p);
    D3DXMatrixInverse(&world, NULL, &world);

    D3DXVECTOR3 org, dir;
    D3DXVec3TransformCoord(&org, &ray.m_org, &world);
    D3DXVec3TransformNormal(&dir, &ray.m_dir, &world);

    return D3DXBoxBoundProbe(&(-m_size), &m_size, &org, &dir) == TRUE;
}

bool OBB::Intersect(D3DXVECTOR3 &point) {
    //Calculate the translation & rotation matrices
    D3DXMATRIX p, r, world;
    D3DXMatrixTranslation(&p, m_pos.x, m_pos.y, m_pos.z);
    D3DXMatrixRotationQuaternion(&r, &m_rot);

    //Calculate the world matrix for the OBB
    D3DXMatrixMultiply(&world, &r, &p);

    //Calculate the inverse world matrix
    D3DXMatrixInverse(&world, NULL, &world);

    //Transform the point with the inverse world matrix
    D3DXVECTOR4 pnt;
    D3DXVec3Transform(&pnt, &point, &world);

    //Check the point just as if it was an AABB
    if (abs(pnt.x) > m_size.x)return false;
    if (abs(pnt.y) > m_size.y)return false;
    if (abs(pnt.z) > m_size.z)return false;

    return true;
}

vector<D3DXVECTOR3> OBB::GetCorners() {
    D3DXMATRIX pos, rot, world;
    D3DXMatrixTranslation(&pos, m_pos.x, m_pos.y, m_pos.z);
    D3DXMatrixRotationQuaternion(&rot, &m_rot);
    D3DXMatrixMultiply(&world, &rot, &pos);

    vector<D3DXVECTOR3> p;
    p.push_back(D3DXVECTOR3(m_size.x, m_size.y, m_size.z));
    p.push_back(D3DXVECTOR3(m_size.x, m_size.y, -m_size.z));
    p.push_back(D3DXVECTOR3(m_size.x, -m_size.y, m_size.z));
    p.push_back(D3DXVECTOR3(m_size.x, -m_size.y, -m_size.z));
    p.push_back(D3DXVECTOR3(-m_size.x, m_size.y, m_size.z));
    p.push_back(D3DXVECTOR3(-m_size.x, m_size.y, -m_size.z));
    p.push_back(D3DXVECTOR3(-m_size.x, -m_size.y, m_size.z));
    p.push_back(D3DXVECTOR3(-m_size.x, -m_size.y, -m_size.z));

    int numCorners = (int)p.size();
    for (int i=0; i<numCorners; i++) {
        D3DXVECTOR4 v4;
        D3DXVec3Transform(&v4, &p[i], &world);
        p[i] = D3DXVECTOR3(v4.x, v4.y, v4.z);
    }

    return p;
}

D3DXVECTOR3 OBB::GetContactPoint(Ray &ray) {
    D3DXMATRIX p, r, world, invWorld;
    D3DXMatrixTranslation(&p, m_pos.x, m_pos.y, m_pos.z);
    D3DXMatrixRotationQuaternion(&r, &m_rot);

    D3DXMatrixMultiply(&world, &r, &p);
    D3DXMatrixInverse(&invWorld, NULL, &world);

    D3DXVECTOR3 org, dir;
    D3DXVec3TransformCoord(&org, &ray.m_org, &invWorld);
    D3DXVec3TransformNormal(&dir, &ray.m_dir, &invWorld);

    D3DXPLANE planes[] = {D3DXPLANE(0.0f, 0.0f, -1.0f, -m_size.z),
                          D3DXPLANE(0.0f, 0.0f, 1.0f,  -m_size.z),
                          D3DXPLANE(0.0f, -1.0f, 0.0f, -m_size.y),
                          D3DXPLANE(0.0f, 1.0f, 0.0f,  -m_size.y),
                          D3DXPLANE(-1.0f, 0.0f, 0.0f, -m_size.x),
                          D3DXPLANE(1.0f, 0.0f, 0.0f,  -m_size.x)
                         };

    D3DXVECTOR3 result;
    int numPlanes = 0;
    int numIntersections = 0;

    for (int i=0; i<6; i++) {
        float d = org.x * planes[i].a +
                  org.y * planes[i].b +
                  org.z * planes[i].c;

        if (d > -planes[i].d) {
            D3DXVECTOR3 r;
            if (D3DXPlaneIntersectLine(&r, &planes[i], &org, &(org + dir * 1000.0f)) != NULL) {
                numPlanes++;

                if (abs(r.x) <= m_size.x + 0.01f &&
                        abs(r.y) <= m_size.y + 0.01f &&
                        abs(r.z) <= m_size.z + 0.01f) {
                    D3DXVec3TransformCoord(&r, &r, &world);
                    result = r;
                    numIntersections++;
                }
            }
        }
    }

    return result;
}
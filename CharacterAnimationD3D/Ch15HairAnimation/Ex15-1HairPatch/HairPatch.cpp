#include "HairPatch.h"

extern IDirect3DDevice9* g_pDevice;
extern ID3DXEffect *g_pEffect;
extern ofstream g_debug;

//Hair Vertex Format
D3DVERTEXELEMENT9 hairVertexDecl[] = {
    {0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
    {0, 12, D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0},
    {0, 16, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0},
    {0, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
    D3DDECL_END()
};

//////////////////////////////////////////////////////////////////////////////////////////////////
//                                  HairStrip                                                   //
//////////////////////////////////////////////////////////////////////////////////////////////////

HairPatch::HairPatch(ControlHair* pCH1,
                     ControlHair* pCH2,
                     ControlHair* pCH3,
                     ControlHair* pCH4) {
    m_controlHairs[0] = pCH1;
    m_controlHairs[1] = pCH2;
    m_controlHairs[2] = pCH3;
    m_controlHairs[3] = pCH4;

    CreateHairStrips(12, 0.15f, 0.2f);
}

HairPatch::~HairPatch() {
    if (m_pHairMesh != NULL)
        m_pHairMesh->Release();
}

D3DXVECTOR3 HairPatch::GetBlendedPoint(D3DXVECTOR2 pos, float prc) {
    D3DXVECTOR3 p1 = m_controlHairs[0]->GetBlendedPoint(prc);
    D3DXVECTOR3 p2 = m_controlHairs[1]->GetBlendedPoint(prc);
    D3DXVECTOR3 p3 = m_controlHairs[2]->GetBlendedPoint(prc);
    D3DXVECTOR3 p4 = m_controlHairs[3]->GetBlendedPoint(prc);

    D3DXVECTOR3 blendedX1 = p2 * pos.x + p1 * (1.0f - pos.x);
    D3DXVECTOR3 blendedX2 = p3 * pos.x + p4 * (1.0f - pos.x);

    return blendedX2 * pos.y + blendedX1 * (1.0f - pos.y);
}

HairVertex HairPatch::GetBlendedVertex(D3DXVECTOR2 pos, float prc, bool oddVertex) {
    //Create a new hair vertex
    HairVertex hv;
    memset(&hv, 0, sizeof(HairVertex));

    //Set vertex data
    hv.position = D3DXVECTOR3(pos.x, pos.y, m_controlHairs[0]->GetSegmentPercent(prc));
    hv.normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
    hv.uv = D3DXVECTOR2(oddVertex ? 0.0f : 1.0f, min(prc, 0.99f));

    //Get segment indices
    pair<int, int> indices = m_controlHairs[0]->GetBlendIndices(prc);
    hv.blendindices[0] = indices.first;
    hv.blendindices[1] = indices.second;

    return hv;
}

vector<D3DXVECTOR2> HairPatch::GetStripPlacements(float sizePerHairStrip) {
    //Place hair strips at random
    vector<D3DXVECTOR2> strips;
    for (int i=0; i<200; i++) {
        //Create random hair position
        D3DXVECTOR2 hairPos = D3DXVECTOR2((rand()%1000) / 1000.0f,
                                          (rand()%1000) / 1000.0f);

        //Check that this hair isnt to close to another hair
        bool valid = true;
        for (int h=0; h<(int)strips.size() && valid; h++) {
            if (D3DXVec2Length(&(hairPos - strips[h])) < sizePerHairStrip)
                valid = false;
        }

        //Add hair if valid
        if (valid)
            strips.push_back(hairPos);
    }

    //order strips for correct alpha blending
    for (int i=0; i<(int)strips.size(); i++) {
        for (int j=i+1; j<(int)strips.size(); j++) {
            if (strips[j].y < strips[i].y) {
                D3DXVECTOR2 temp = strips[i];
                strips[i] = strips[j];
                strips[j] = temp;
            }
        }
    }

    return strips;
}

void HairPatch::CreateHairStrips(int numSegments, float sizePerHairStrip, float stripSize) {
    //Get random hair strip positions
    vector<D3DXVECTOR2> strips = GetStripPlacements(sizePerHairStrip);

    //Create a mesh containing all strips
    int numFacesPerStrip = numSegments * 2;
    int numVertsPerStrip = (numSegments + 1) * 2;

    D3DXCreateMesh(numFacesPerStrip * (int)strips.size(),
                   numVertsPerStrip * (int)strips.size(),
                   D3DXMESH_MANAGED,
                   hairVertexDecl,
                   g_pDevice,
                   &m_pHairMesh);

    HairVertex *vb = NULL;
    WORD *ib = NULL;
    m_pHairMesh->LockVertexBuffer(0, (void**)&vb);
    m_pHairMesh->LockIndexBuffer(0, (void**)&ib);

    int vIndex = 0;
    int iIndex = 0;

    //Create hair strips
    for (int i=0; i<(int)strips.size(); i++) {
        int startVertIndex = vIndex;

        //Angle the strip randomly
        float rAngle = ((rand()%1000) / 1000.0f) * 2.0f - 1.0f;
        D3DXVECTOR2 rOffset = D3DXVECTOR2(cos(rAngle), sin(rAngle)) * stripSize;

        //Create vertices
        for (int s=0; s<=numSegments; s++) {
            float prc = s / (float)numSegments;
            vb[vIndex++] = GetBlendedVertex(strips[i] + rOffset, prc, false);
            vb[vIndex++] = GetBlendedVertex(strips[i] - rOffset, prc, true);
        }

        //Create indices
        for (int s=0; s<numSegments; s++) {
            //Tri 1
            ib[iIndex++] = startVertIndex + s * 2 + 0;
            ib[iIndex++] = startVertIndex + s * 2 + 3;
            ib[iIndex++] = startVertIndex + s * 2 + 1;

            //Tri 2
            ib[iIndex++] = startVertIndex + s * 2 + 0;
            ib[iIndex++] = startVertIndex + s * 2 + 2;
            ib[iIndex++] = startVertIndex + s * 2 + 3;
        }
    }

    m_pHairMesh->UnlockVertexBuffer();
    m_pHairMesh->UnlockIndexBuffer();
}

void HairPatch::Render() {
    //Create and control hair table
    vector<D3DXVECTOR3> hairTable;
    for (int h=0; h<4; h++) {
        for (int i=0; i<(int)m_controlHairs[h]->m_points.size(); i++)
            hairTable.push_back(m_controlHairs[h]->m_points[i]);
    }

    //Upload hair table to GPU
    g_pEffect->SetFloatArray("ControlHairTable", (FLOAT*)&hairTable[0], (int)hairTable.size() * 3);
    g_pEffect->CommitChanges();

    //Render hair
    if (m_pHairMesh != NULL) {
        m_pHairMesh->DrawSubset(0);
    }
}

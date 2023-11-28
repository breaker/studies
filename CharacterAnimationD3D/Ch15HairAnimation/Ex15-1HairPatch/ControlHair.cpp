#include "ControlHair.h"

extern IDirect3DDevice9* g_pDevice;
extern ID3DXEffect *g_pEffect;
extern ofstream g_debug;

//////////////////////////////////////////////////////////////////////////////////////////////////
//                                  ControlHairs                                                //
//////////////////////////////////////////////////////////////////////////////////////////////////

ControlHair::ControlHair() {
}

ControlHair* ControlHair::CreateTestHair(D3DXVECTOR3 startPos) {
    ControlHair* newHair = new ControlHair();

    for (int y=0; y<4; y++) {
        newHair->m_points.push_back(startPos + D3DXVECTOR3( ((rand()%100) / 100.0f) - 0.5f,
                                    (float)y,
                                    ((rand()%100) / 100.0f) - 0.5f));

        newHair->m_destPoints.push_back(newHair->m_points[y]);
    }

    return newHair;
}

float ControlHair::GetSegmentPercent(float prc) {
    float step = 1.0f / (float)(m_points.size() - 1);
    int numSteps = (int)(prc / step);
    return (prc - numSteps * step) / step;
}

pair<int, int> ControlHair::GetBlendIndices(float prc) {
    if (prc <= 0.0f) return pair<int, int>(0, 0);
    if (prc >= 1.0f) return pair<int, int>((int)m_points.size() - 1, (int)m_points.size() - 1);

    int index1 = (int)(prc * (m_points.size() - 1));
    int index2 = min(index1 + 1, (int)m_points.size() - 1);

    return pair<int, int>(index1, index2);
}

D3DXVECTOR3 ControlHair::GetBlendedPoint(float prc) {
    pair<int, int> indices = GetBlendIndices(prc);

    int index0 = max(indices.first - 1, 0);
    int index1 = indices.first;
    int index2 = indices.second;
    int index3 = min(indices.second + 1, (int)m_points.size() - 1);

    float step = 1.0f / (float)(m_points.size() - 1);
    prc = (prc - index1 * step) / step;

    //cubic interpolation
    D3DXVECTOR3 P = (m_points[index3] - m_points[index2]) - (m_points[index0] - m_points[index1]);
    D3DXVECTOR3 Q = (m_points[index0] - m_points[index1]) - P;
    D3DXVECTOR3 R = m_points[index2] - m_points[index0];
    D3DXVECTOR3 S = m_points[index1];

    return (P * prc * prc * prc) +
           (Q * prc * prc) +
           (R * prc) +
           S;
}

void ControlHair::Update(float deltaTime) {
    for (int i=1; i<(int)m_points.size(); i++) {
        D3DXVECTOR3 diff = m_destPoints[i] - m_points[i];
        float len = D3DXVec3Length(&diff);

        if (len < 0.1f) {
            m_destPoints[i] = m_points[0] + D3DXVECTOR3( ((rand()%100) / 100.0f) - 0.5f,
                              m_points[i].y,
                              ((rand()%100) / 100.0f) - 0.5f);
        }

        D3DXVec3Normalize(&diff, &diff);
        m_points[i] += diff * deltaTime;
    }
}

void ControlHair::Render() {
    if (m_points.empty())
        return;

    //Create vertices
    vector<LineVertex> verts;
    for (int i=0; i<(int)m_points.size(); i++) {
        verts.push_back(LineVertex(m_points[i],0xffff0000));
    }

    //Render line
    g_pDevice->SetRenderState(D3DRS_LIGHTING, false);
    g_pDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
    g_pDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, (int)verts.size() - 1, &verts[0], sizeof(LineVertex));
}
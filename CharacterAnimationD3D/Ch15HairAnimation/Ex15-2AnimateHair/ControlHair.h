//////////////////////////////////////////////////////////////////////////
//                  Character Animation with Direct3D                   //
//                         Author: C. Granberg                          //
//                             2008 - 2009                              //
//////////////////////////////////////////////////////////////////////////

#ifndef _CONTROL_HAIR_
#define _CONTROL_HAIR_

#include <windows.h>
#include <d3dx9.h>
#include <vector>
#include "BoundingSphere.h"

using namespace std;

struct LineVertex {
    LineVertex(D3DXVECTOR3 pos, D3DCOLOR col) {
        position = pos;
        color = col;
    }
    D3DXVECTOR3 position;
    D3DCOLOR color;
};

class ControlHair {
public:
    ControlHair();

    void AddPoint(D3DXVECTOR3 point);

    float GetSegmentPercent(float prc);
    pair<int, int> GetBlendIndices(float prc);
    D3DXVECTOR3 GetBlendedPoint(float prc);

    void Render();

    void UpdateSimulation(float deltaTime, vector<BoundingSphere> &headSpheres);

public:
    vector<D3DXVECTOR3> m_points;
    vector<D3DXVECTOR3> m_originalPoints;
    vector<D3DXVECTOR3> m_velocities;
};

#endif
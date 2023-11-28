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

    static ControlHair* CreateTestHair(D3DXVECTOR3 startPos);

    float GetSegmentPercent(float prc);
    pair<int, int> GetBlendIndices(float prc);
    D3DXVECTOR3 GetBlendedPoint(float prc);

    void Update(float deltaTime);
    void Render();

public:
    vector<D3DXVECTOR3> m_points;
    vector<D3DXVECTOR3> m_destPoints;
};

#endif
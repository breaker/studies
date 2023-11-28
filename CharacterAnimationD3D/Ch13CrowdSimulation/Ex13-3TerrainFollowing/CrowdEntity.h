//////////////////////////////////////////////////////////////////////////
//                  Character Animation with Direct3D                   //
//                         Author: C. Granberg                          //
//                             2008 - 2009                              //
//////////////////////////////////////////////////////////////////////////

#ifndef _CROWDENTITY_
#define _CROWDENTITY_

#include <d3dx9.h>
#include "SkinnedMesh.h"

using namespace std;

class CrowdEntity {
    friend class Crowd;
public:
    CrowdEntity(Crowd *pCrowd);
    ~CrowdEntity();
    void Update(float deltaTime);
    void Render();
    D3DXVECTOR3 GetRandomLocation();
    D3DXVECTOR3 GetRandomGoal();

public:
    static SkinnedMesh* sm_pSoldierMesh;

    Crowd* m_pCrowd;
    D3DXVECTOR3 m_position;
    D3DXVECTOR3 m_velocity;
    D3DXVECTOR3 m_goal;

    ID3DXAnimationController* m_pAnimController;
};

#endif
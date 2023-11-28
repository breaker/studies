//////////////////////////////////////////////////////////////////////////
//                  Character Animation with Direct3D                   //
//                         Author: C. Granberg                          //
//                             2008 - 2009                              //
//////////////////////////////////////////////////////////////////////////

#ifndef _CROWD_
#define _CROWD_

#include <d3dx9.h>
#include <vector>
#include "CrowdEntity.h"
#include "Mesh.h"

using namespace std;

class Obstacle {
public:
    Obstacle(D3DXVECTOR3 pos, float radius);
    D3DXVECTOR3 GetForce(CrowdEntity* pEntity);
    void Render();

public:
    static ID3DXMesh* sm_cylinder;
    D3DXVECTOR3 m_position;
    float m_radius;
};

class Crowd {
public:
    Crowd(int numEntities);
    ~Crowd();
    void Update(float deltaTime);
    void Render();
    void GetNeighbors(CrowdEntity* pEntity, float radius, vector<CrowdEntity*> &neighbors);
    void SetEntityGroundPos(D3DXVECTOR3 &pos);

public:
    vector<CrowdEntity*> m_entities;
    vector<Obstacle*> m_obstacles;
    Mesh* m_pFloor;
};

#endif
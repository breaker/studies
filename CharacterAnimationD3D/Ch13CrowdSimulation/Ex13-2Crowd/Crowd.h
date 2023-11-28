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

using namespace std;

class Crowd {
public:
    Crowd(int numEntities);
    ~Crowd();
    void Update(float deltaTime);
    void Render();
    void GetNeighbors(CrowdEntity* pEntity, float radius, vector<CrowdEntity*> &neighbors);

private:
    vector<CrowdEntity*> m_entities;
};

#endif
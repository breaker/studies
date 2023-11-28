//////////////////////////////////////////////////////////////////////////
//                  Character Animation with Direct3D                   //
//                         Author: C. Granberg                          //
//                             2008 - 2009                              //
//////////////////////////////////////////////////////////////////////////

#ifndef _FLOCK_
#define _FLOCK_

#include <d3dx9.h>
#include <vector>
#include "boid.h"

using namespace std;

class Flock {
public:
    Flock(int numBoids);
    ~Flock();
    void Update(float deltaTime);
    void Render(bool shadow);
    void GetNeighbors(Boid* pBoid, float radius, vector<Boid*> &neighbors);

private:
    vector<Boid*> m_boids;
};

#endif
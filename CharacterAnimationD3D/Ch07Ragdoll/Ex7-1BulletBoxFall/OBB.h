//////////////////////////////////////////////////////////////////////////
//                  Character Animation with Direct3D                   //
//                         Author: C. Granberg                          //
//                             2008 - 2009                              //
//////////////////////////////////////////////////////////////////////////

#ifndef ORIENTED_BBOX
#define ORIENTED_BBOX

#include <d3dx9.h>
#include <vector>
#include "btBulletDynamicsCommon.h"

using namespace std;

class OBB {
public:
    OBB(D3DXVECTOR3 pos, D3DXVECTOR3 size, bool dynamic=true);
    ~OBB();
    void Release();
    void Render();

public:
    btRigidBody *m_pBody;

private:
    ID3DXMesh *m_pMesh;
};

#endif
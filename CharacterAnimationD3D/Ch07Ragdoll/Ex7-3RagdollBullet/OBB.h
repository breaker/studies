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
    OBB(D3DXVECTOR3 pos, D3DXVECTOR3 size, D3DXQUATERNION rot, bool dynamic=true);
    void Init(D3DXVECTOR3 pos, D3DXVECTOR3 size, D3DXQUATERNION rot, bool dynamic=true);
    ~OBB();
    void Release();
    void Update(float deltaTime);
    void Render();

public:
    btRigidBody *m_pBody;
    D3DXVECTOR3 m_size;

private:
    ID3DXMesh *m_pMesh;
};

#endif
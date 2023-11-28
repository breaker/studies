//////////////////////////////////////////////////////////////////////////
//                  Character Animation with Direct3D                   //
//                         Author: C. Granberg                          //
//                             2008 - 2009                              //
//////////////////////////////////////////////////////////////////////////

#ifndef _EYE_
#define _EYE_

#include <d3dx9.h>
#include <fstream>

using namespace std;

class Eye {
public:
    Eye();
    void Init(D3DXVECTOR3 position);
    void Render(ID3DXEffect *pEffect);
    void LookAt(D3DXVECTOR3 focus);

private:
    D3DXVECTOR3 m_position;
    D3DXVECTOR3 m_lookAt;
    D3DXMATRIX m_rotation;
};

#endif
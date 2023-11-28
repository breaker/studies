//////////////////////////////////////////////////////////////////////////
//                  Character Animation with Direct3D                   //
//                         Author: C. Granberg                          //
//                             2008 - 2009                              //
//////////////////////////////////////////////////////////////////////////

#ifndef _EYE_
#define _EYE_

#include <d3dx9.h>
#include <fstream>
#include <vector>

using namespace std;

void LoadEyeResources();

class Eye {
public:
    Eye();
    void Init(D3DXVECTOR3 position, int textureIndex);
    void Render(D3DXMATRIX &headMatrix);
    void LookAt(D3DXVECTOR3 focus);

private:
    D3DXVECTOR3 m_position;
    D3DXVECTOR3 m_lookAt;
    D3DXMATRIX m_rotation;
    int m_textureIndex;
};

#endif
//////////////////////////////////////////////////////////////////////////
//                  Character Animation with Direct3D                   //
//                         Author: C. Granberg                          //
//                             2008 - 2009                              //
//////////////////////////////////////////////////////////////////////////

#ifndef CHARACTER_DECAL
#define CHARACTER_DECAL

#include <windows.h>
#include <d3dx9.h>
#include <map>

using namespace std;

class CharacterDecal {
public:
    CharacterDecal(ID3DXMesh* pDecalMesh);
    ~CharacterDecal();
    void Render();

public:
    ID3DXMesh* m_pDecalMesh;
    static IDirect3DTexture9* sm_pDecalTexture;
};

#endif
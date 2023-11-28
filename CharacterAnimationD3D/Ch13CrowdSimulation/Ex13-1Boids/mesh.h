//////////////////////////////////////////////////////////////////////////
//                  Character Animation with Direct3D                   //
//                         Author: C. Granberg                          //
//                             2008 - 2009                              //
//////////////////////////////////////////////////////////////////////////

#ifndef _MESH_
#define _MESH_

#include <d3dx9.h>
#include <vector>

using namespace std;

class Mesh {
public:
    Mesh();
    Mesh(char fName[]);
    ~Mesh();
    HRESULT Load(char fName[]);
    void Render();
    void Release();

private:

    ID3DXMesh *m_pMesh;
    vector<IDirect3DTexture9*> m_textures;
    vector<D3DMATERIAL9> m_materials;
    D3DMATERIAL9 m_white;
};

#endif
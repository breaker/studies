//////////////////////////////////////////////////////////////////////////
//                  Character Animation with Direct3D                   //
//                         Author: C. Granberg                          //
//                             2008 - 2009                              //
//////////////////////////////////////////////////////////////////////////

#ifndef MTEST
#define MTEST

#include <d3dx9.h>
#include <fstream>

using namespace std;

class MorphTest {
    friend class Application;
public:
    MorphTest();
    void Init();
    void Update(float deltaTime);
    void Render();

private:
    ID3DXMesh* m_pBaseMesh;
    ID3DXMesh* m_pTargets[4];

    IDirect3DVertexDeclaration9 *m_pDecl;

    D3DXVECTOR4 m_oldWeights, m_newWeights;
    float m_prc;
};

#endif
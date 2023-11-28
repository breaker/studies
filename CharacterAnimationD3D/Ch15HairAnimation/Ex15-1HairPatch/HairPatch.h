//////////////////////////////////////////////////////////////////////////
//                  Character Animation with Direct3D                   //
//                         Author: C. Granberg                          //
//                             2008 - 2009                              //
//////////////////////////////////////////////////////////////////////////

#ifndef HAIR_STRIP
#define HAIR_STRIP

#include <windows.h>
#include <d3dx9.h>
#include "ControlHair.h"

using namespace std;

struct HairVertex {
    D3DXVECTOR3 position;
    byte        blendindices[4];
    D3DXVECTOR3 normal;
    D3DXVECTOR2 uv;
};

class HairPatch {
public:
    HairPatch(ControlHair* pCH1,
              ControlHair* pCH2,
              ControlHair* pCH3,
              ControlHair* pCH4);
    ~HairPatch();

    D3DXVECTOR3 GetBlendedPoint(D3DXVECTOR2 pos, float prc);
    HairVertex GetBlendedVertex(D3DXVECTOR2 pos, float prc, bool oddVertex);

    vector<D3DXVECTOR2> GetStripPlacements(float sizePerHairStrip);
    void CreateHairStrips(int numSegments, float sizePerHairStrip, float stripSize);

    void Render();

public:
    ID3DXMesh* m_pHairMesh;
    ControlHair* m_controlHairs[4];

};

#endif
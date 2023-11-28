//////////////////////////////////////////////////////////////////////////
//                  Character Animation with Direct3D                   //
//                         Author: C. Granberg                          //
//                             2008 - 2009                              //
//////////////////////////////////////////////////////////////////////////

#ifndef _HAIR_
#define _HAIR_

#include <windows.h>
#include <d3dx9.h>
#include "HairPatch.h"

using namespace std;

class Hair {
public:
    Hair();
    ~Hair();

    void LoadHair(const char* hairFile);

    void CreatePatch(int index1, int index2, int index3, int index4);

    void Update(float deltaTime);
    void Render(D3DXVECTOR3 &camPos);

    int GetNumVertices();
    int GetNumFaces();

public:
    vector<ControlHair*> m_controlHairs;
    vector<HairPatch*> m_hairPatches;
    IDirect3DTexture9* m_pHairTexture;
    vector<BoundingSphere> m_headSpheres;
};

#endif
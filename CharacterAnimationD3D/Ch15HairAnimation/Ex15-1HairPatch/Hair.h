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
    void Update(float deltaTime);
    void Render();

public:
    vector<ControlHair*> m_controlHairs;
    vector<HairPatch*> m_hairPatches;
    IDirect3DTexture9* m_pHairTexture;
};

#endif
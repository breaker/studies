//////////////////////////////////////////////////////////////////////////
//                  Character Animation with Direct3D                   //
//                         Author: C. Granberg                          //
//                             2008 - 2009                              //
//////////////////////////////////////////////////////////////////////////

#ifndef _CHARACTER_
#define _CHARACTER_

#include <windows.h>
#include <d3dx9.h>
#include <string>
#include <vector>
#include "skinnedmesh.h"
#include "face.h"
#include "faceController.h"
#include "faceFactory.h"
#include "ragdoll.h"
#include "inverseKinematics.h"

using namespace std;

class Character : public RagDoll {
public:
    Character(char fileName[], D3DXMATRIX &world);
    ~Character();
    void Update(float deltaTime);
    void Render();
    void RenderMesh(Bone *bone);
    void RenderFace(BoneMesh *pFacePlaceholder);
    void PlayAnimation(string name);
    void Kill();

public:
    bool m_lookAtIK, m_armIK;
    bool m_dead;

private:
    Face *m_pFace;
    FaceController *m_pFaceController;
    ID3DXAnimationController* m_pAnimController;
    InverseKinematics *m_pIK;
    IDirect3DVertexDeclaration9 *m_pFaceVertexDecl;
    int m_animation;
};

#endif
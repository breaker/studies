//////////////////////////////////////////////////////////////////////////
//                  Character Animation with Direct3D                   //
//                         Author: C. Granberg                          //
//                             2008 - 2009                              //
//////////////////////////////////////////////////////////////////////////

#ifndef _IK_
#define _IK_

#include <d3dx9.h>

struct Bone;
class SkinnedMesh;

class InverseKinematics {
public:
    InverseKinematics(SkinnedMesh* pSkinnedMesh);
    void UpdateHeadIK();
    void ApplyLookAtIK(D3DXVECTOR3 &lookAtTarget, float maxAngle);

private:
    SkinnedMesh *m_pSkinnedMesh;
    Bone* m_pHeadBone;
    D3DXVECTOR3 m_headForward;
};

#endif
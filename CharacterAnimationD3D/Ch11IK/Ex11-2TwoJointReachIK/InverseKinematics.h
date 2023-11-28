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
    void UpdateArmIK();
    void ApplyLookAtIK(D3DXVECTOR3 &lookAtTarget, float maxAngle);
    void ApplyArmIK(D3DXVECTOR3 &hingeAxis, D3DXVECTOR3 &target);

private:
    SkinnedMesh *m_pSkinnedMesh;
    D3DXVECTOR3 m_headForward;

    Bone* m_pHeadBone;
    Bone* m_pShoulderBone;
    Bone* m_pElbowBone;
    Bone* m_pHandBone;
};

#endif
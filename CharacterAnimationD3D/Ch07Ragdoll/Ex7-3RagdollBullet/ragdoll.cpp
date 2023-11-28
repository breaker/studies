#include "ragdoll.h"
#include <fstream>

extern IDirect3DDevice9* g_pDevice;
extern PhysicsEngine physicsEngine;
extern ofstream g_debug;

bool KeyDown(int vk_code);

RagDoll::RagDoll(char fileName[], D3DXMATRIX &world) : SkinnedMesh() {
    SkinnedMesh::Load(fileName);
    SetPose(world);

    Bone* Head_End = (Bone*)D3DXFrameFind(m_pRootBone, "Head_End");
    Bone* Head = (Bone*)D3DXFrameFind(m_pRootBone, "Head");
    Bone* Neck = (Bone*)D3DXFrameFind(m_pRootBone, "Neck");
    Bone* Pelvis = (Bone*)D3DXFrameFind(m_pRootBone, "Pelvis");
    Bone* Spine = (Bone*)D3DXFrameFind(m_pRootBone, "Spine");
    Bone* R_Shoulder = (Bone*)D3DXFrameFind(m_pRootBone, "Shoulder_Right");
    Bone* L_Shoulder = (Bone*)D3DXFrameFind(m_pRootBone, "Shoulder_Left");
    Bone* U_R_Arm = (Bone*)D3DXFrameFind(m_pRootBone, "Upper_Arm_Right");
    Bone* U_L_Arm = (Bone*)D3DXFrameFind(m_pRootBone, "Upper_Arm_Left");
    Bone* L_R_Arm = (Bone*)D3DXFrameFind(m_pRootBone, "Lower_Arm_Right");
    Bone* L_L_Arm = (Bone*)D3DXFrameFind(m_pRootBone, "Lower_Arm_Left");
    Bone* R_Hand = (Bone*)D3DXFrameFind(m_pRootBone, "Hand_Right");
    Bone* L_Hand = (Bone*)D3DXFrameFind(m_pRootBone, "Hand_Left");

    Bone* R_Pelvis = (Bone*)D3DXFrameFind(m_pRootBone, "Pelvis_Right");
    Bone* L_Pelvis = (Bone*)D3DXFrameFind(m_pRootBone, "Pelvis_Left");
    Bone* R_Thigh = (Bone*)D3DXFrameFind(m_pRootBone, "Thigh_Right");
    Bone* L_Thigh = (Bone*)D3DXFrameFind(m_pRootBone, "Thigh_Left");
    Bone* R_Calf = (Bone*)D3DXFrameFind(m_pRootBone, "Calf_Right");
    Bone* L_Calf = (Bone*)D3DXFrameFind(m_pRootBone, "Calf_Left");
    Bone* R_Foot = (Bone*)D3DXFrameFind(m_pRootBone, "Foot_Right");
    Bone* L_Foot = (Bone*)D3DXFrameFind(m_pRootBone, "Foot_Left");
    Bone* R_Foot_End = (Bone*)D3DXFrameFind(m_pRootBone, "Foot_Right_End");
    Bone* L_Foot_End = (Bone*)D3DXFrameFind(m_pRootBone, "Foot_Left_End");

    D3DXQUATERNION q, q2;
    D3DXQuaternionIdentity(&q);
    D3DXQuaternionRotationAxis(&q2, &D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DX_PI * -0.5f);

    //Head
    OBB* o01 = CreateBoneBox(Head, Head_End, D3DXVECTOR3(0.3f, 0.2f, 0.2f), q);

    //Right Arm
    OBB* o03 = CreateBoneBox(U_R_Arm, L_R_Arm, D3DXVECTOR3(0.3f, 0.12f, 0.12f), q);
    OBB* o04 = CreateBoneBox(L_R_Arm, R_Hand, D3DXVECTOR3(0.3f, 0.12f, 0.12f), q);

    //Left Arm
    OBB* o06 = CreateBoneBox(U_L_Arm, L_L_Arm, D3DXVECTOR3(0.3f, 0.12f, 0.12f), q);
    OBB* o07 = CreateBoneBox(L_L_Arm, L_Hand, D3DXVECTOR3(0.3f, 0.12f, 0.12f), q);

    //Spine
    OBB* o08 = CreateBoneBox(Spine, Neck, D3DXVECTOR3(0.5f, 0.35f, 0.15f), q);

    //Right Leg
    OBB* o09 = CreateBoneBox(R_Thigh, R_Calf, D3DXVECTOR3(0.4f, 0.15f, 0.15f), q);
    OBB* o10 = CreateBoneBox(R_Calf, R_Foot, D3DXVECTOR3(0.4f, 0.14f, 0.14f), q);
    OBB* o11 = CreateBoneBox(R_Foot, R_Foot_End, D3DXVECTOR3(0.15f, 0.06f, 0.12f), q2);

    //Left Leg
    OBB* o12 = CreateBoneBox(L_Thigh, L_Calf, D3DXVECTOR3(0.4f, 0.15f, 0.15f), q);
    OBB* o13 = CreateBoneBox(L_Calf, L_Foot, D3DXVECTOR3(0.4f, 0.14f, 0.14f), q);
    OBB* o14 = CreateBoneBox(L_Foot, L_Foot_End, D3DXVECTOR3(0.15f, 0.06f, 0.12f), q2);

    //Constraints
    CreateTwistCone(Neck, o01, o08, D3DX_PI * 0.15f, D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.5f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.5f));

    CreateTwistCone(L_Pelvis, o08, o12, D3DX_PI * 0.5f, D3DXVECTOR3(0.3f, D3DX_PI * 0.5f, -D3DX_PI * 0.5f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.5f));
    CreateHinge(L_Calf, o12, o13, 0.0f, -2.0f, D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.5f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.5f));
    CreateHinge(L_Foot, o13, o14, 1.5f, 1.0f, D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.5f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.5f));

    CreateTwistCone(R_Pelvis, o08, o09, D3DX_PI * 0.5f, D3DXVECTOR3(0.3f, D3DX_PI * 0.5f, -D3DX_PI * 0.5f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.5f));
    CreateHinge(R_Calf, o09, o10, 0.0f, -2.0f, D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.5f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.5f));
    CreateHinge(R_Foot, o10, o11, 1.5f, 1.0f, D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.5f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.5f));

    CreateTwistCone(U_L_Arm, o08, o06, D3DX_PI * 0.6f, D3DXVECTOR3(0.0f, D3DX_PI * 0.75f, D3DX_PI * 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
    CreateHinge(L_L_Arm, o06, o07, 2.0f, 0.0f, D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.5f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.5f));

    CreateTwistCone(U_R_Arm, o08, o03, D3DX_PI * 0.6f, D3DXVECTOR3(0.0f, D3DX_PI * 0.75f, D3DX_PI * 0.0f), D3DXVECTOR3(D3DX_PI, 0.0f, 0.0f));
    CreateHinge(L_R_Arm, o03, o04, 2.0f, 0.0f, D3DXVECTOR3(0.0f, 0.0f, -D3DX_PI * 0.5f), D3DXVECTOR3(0.0f, 0.0f, -D3DX_PI * 0.5f));


}

RagDoll::~RagDoll() {
    Release();
}

void RagDoll::Release() {

}

void RagDoll::Update(float deltaTime) {
    for (int i=0; i<(int)m_boxes.size(); i++) {
        m_boxes[i]->Update(deltaTime);
    }
}

void RagDoll::Render() {
    for (int i=0; i<(int)m_boxes.size(); i++) {
        m_boxes[i]->Render();
    }
}

OBB* RagDoll::CreateBoneBox(Bone* parent, Bone *bone, D3DXVECTOR3 size, D3DXQUATERNION rot) {
    if (bone == NULL || parent == NULL)
        return NULL;

    //Get Bone Starting point
    D3DXMATRIX &parentMat = parent->CombinedTransformationMatrix;
    D3DXVECTOR3 parentPos(parentMat(3, 0), parentMat(3, 1), parentMat(3, 2));

    //Get Bone End point
    D3DXMATRIX &boneMat = bone->CombinedTransformationMatrix;
    D3DXVECTOR3 bonePos(boneMat(3, 0), boneMat(3, 1), boneMat(3, 2));

    //Extract the rotation from the bone
    D3DXQUATERNION q;
    D3DXVECTOR3 p, s;
    D3DXMatrixDecompose(&s, &q, &p, &parentMat);

    //Offset rotation (in some cases only)
    q *= rot;
    D3DXQuaternionNormalize(&q, &q);

    //Calculate the middle point
    p = (parentPos + bonePos) * 0.5f;

    //Create new OBB
    OBB *obb = new OBB(p, size, q, true);

    //Add the OBB to the physics engine
    physicsEngine.GetWorld()->addRigidBody(obb->m_pBody);
    m_boxes.push_back(obb);

    parent->m_pObb = obb;

    return obb;
}

void RagDoll::CreateHinge(Bone* parent, OBB* A, OBB* B,
                          float upperLimit, float lowerLimit,
                          D3DXVECTOR3 hingeAxisA, D3DXVECTOR3 hingeAxisB,
                          bool ignoreCollisions) {
    if (parent == NULL || A == NULL || B == NULL)
        return;

    D3DXMATRIX &parentMat = parent->CombinedTransformationMatrix;
    btVector3 hingePos(parentMat(3, 0), parentMat(3, 1), parentMat(3, 2));
    D3DXVECTOR3 hingePosDX = D3DXVECTOR3(parentMat(3, 0), parentMat(3, 1), parentMat(3, 2));

    btRigidBody *a = A->m_pBody;
    btRigidBody *b = B->m_pBody;

    btTransform aTrans, bTrans;
    a->getMotionState()->getWorldTransform(aTrans);
    b->getMotionState()->getWorldTransform(bTrans);
    D3DXMATRIX worldA = BT2DX_MATRIX(aTrans);
    D3DXMATRIX worldB = BT2DX_MATRIX(bTrans);

    D3DXVECTOR3 offA, offB;
    D3DXMatrixInverse(&worldA, NULL, &worldA);
    D3DXMatrixInverse(&worldB, NULL, &worldB);

    D3DXVec3TransformCoord(&offA, &hingePosDX, &worldA);
    D3DXVec3TransformCoord(&offB, &hingePosDX, &worldB);

    btVector3 offsetA(offA.x, offA.y, offA.z);
    btVector3 offsetB(offB.x, offB.y, offB.z);

    aTrans.setIdentity();
    bTrans.setIdentity();
    aTrans.setOrigin(offsetA);
    bTrans.setOrigin(offsetB);
    aTrans.getBasis().setEulerZYX(hingeAxisA.x, hingeAxisA.y, hingeAxisA.z);
    bTrans.getBasis().setEulerZYX(hingeAxisB.x, hingeAxisB.y, hingeAxisB.z);

    btHingeConstraint *hingeC = new btHingeConstraint(*b, *a, bTrans, aTrans);
    hingeC->setLimit(lowerLimit, upperLimit);
    physicsEngine.GetWorld()->addConstraint(hingeC, ignoreCollisions);
}

void RagDoll::CreateTwistCone(Bone* parent, OBB* A, OBB* B, float limit,
                              D3DXVECTOR3 hingeAxisA, D3DXVECTOR3 hingeAxisB,
                              bool ignoreCollisions) {
    if (parent == NULL || A == NULL || B == NULL)
        return;

    //Extract the constraint position
    D3DXMATRIX &parentMat = parent->CombinedTransformationMatrix;
    btVector3 hingePos(parentMat(3, 0), parentMat(3, 1), parentMat(3, 2));
    D3DXVECTOR3 hingePosDX(parentMat(3, 0), parentMat(3, 1), parentMat(3, 2));

    //Get references to the two rigid bodies we want to connect
    btRigidBody *a = A->m_pBody;
    btRigidBody *b = B->m_pBody;

    //Get world matrix from the two rigid bodies
    btTransform aTrans, bTrans;
    a->getMotionState()->getWorldTransform(aTrans);
    b->getMotionState()->getWorldTransform(bTrans);
    D3DXMATRIX worldA = BT2DX_MATRIX(aTrans);
    D3DXMATRIX worldB = BT2DX_MATRIX(bTrans);

    //Calculate Pivot point for both rigid bodies
    D3DXMatrixInverse(&worldA, NULL, &worldA);
    D3DXMatrixInverse(&worldB, NULL, &worldB);

    D3DXVECTOR3 offA, offB;
    D3DXVec3TransformCoord(&offA, &hingePosDX, &worldA);
    D3DXVec3TransformCoord(&offB, &hingePosDX, &worldB);
    btVector3 offsetA(offA.x, offA.y, offA.z);
    btVector3 offsetB(offB.x, offB.y, offB.z);

    //Set constraint axis
    aTrans.setIdentity();
    bTrans.setIdentity();
    aTrans.setOrigin(offsetA);
    bTrans.setOrigin(offsetB);
    aTrans.getBasis().setEulerZYX(hingeAxisA.x, hingeAxisA.y, hingeAxisA.z);
    bTrans.getBasis().setEulerZYX(hingeAxisB.x, hingeAxisB.y, hingeAxisB.z);

    //Create new twist cone constraint
    btConeTwistConstraint *twistC = new btConeTwistConstraint(*a, *b, aTrans, bTrans);

    //Set Constraint limits
    twistC->setLimit(limit, limit, 0.05f);

    //Add constraint to the physics engine
    physicsEngine.GetWorld()->addConstraint(twistC, true);
}
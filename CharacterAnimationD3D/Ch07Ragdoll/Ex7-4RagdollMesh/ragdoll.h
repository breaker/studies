//////////////////////////////////////////////////////////////////////////
//                  Character Animation with Direct3D                   //
//                         Author: C. Granberg                          //
//                             2008 - 2009                              //
//////////////////////////////////////////////////////////////////////////

#ifndef RAG_DOLL
#define RAG_DOLL

#include <windows.h>
#include <d3dx9.h>
#include <string>
#include <vector>
#include "skinnedmesh.h"
#include "physicsEngine.h"

using namespace std;

class RagDoll : public SkinnedMesh {
public:
    RagDoll(char fileName[], D3DXMATRIX &world);
    ~RagDoll();
    void InitBones(Bone *bone);
    void Release();
    void Update(float deltaTime);
    void Render();
    void UpdateSkeleton(Bone* bone);
    OBB* CreateBoneBox(Bone* parent, Bone *bone, D3DXVECTOR3 size, D3DXQUATERNION rot);
    void CreateHinge(Bone* parent, OBB* A, OBB *B, float upperLimit, float lowerLimit, D3DXVECTOR3 hingeAxisA, D3DXVECTOR3 hingeAxisB, bool ignoreCollisions=true);
    void CreateTwistCone(Bone* parent, OBB* A, OBB *B, float limit, D3DXVECTOR3 hingeAxisA, D3DXVECTOR3 hingeAxisB, bool ignoreCollisions=true);

private:
    vector<OBB*> m_boxes;
};

#endif
#include "InverseKinematics.h"
#include "skinnedMesh.h"

// Setup forward vectors and get bone pointers for our example
InverseKinematics::InverseKinematics(SkinnedMesh* pSkinnedMesh) {
    m_pSkinnedMesh = pSkinnedMesh;

    // Find the head bone
    m_pHeadBone = (Bone*)m_pSkinnedMesh->GetBone("Head");

    // Exit if there is no head bone
    if (m_pHeadBone != NULL) {
        // Calculate the local forward vector for the head bone

        // Remove translation
        D3DXMATRIX headMatrix = m_pHeadBone->CombinedTransformationMatrix;
        headMatrix._41 = 0.0f;
        headMatrix._42 = 0.0f;
        headMatrix._43 = 0.0f;
        headMatrix._44 = 1.0f;

        D3DXMATRIX toHeadSpace;
        if (D3DXMatrixInverse(&toHeadSpace, NULL, &headMatrix) == NULL)
            return;

        // Our model is looking towards -z in the content
        D3DXVECTOR4 vec;
        D3DXVec3Transform(&vec, &D3DXVECTOR3(0, 0, -1), &toHeadSpace);
        m_headForward = D3DXVECTOR3(vec.x, vec.y, vec.z);
    }
}

// Head IK test function
void InverseKinematics::UpdateHeadIK() {
    // Exit if there is no head bone
    if (m_pHeadBone == NULL)
        return;

    //Get look at point from cursor location
    POINT p;
    GetCursorPos(&p);
    p.x = max(min(p.x, 800), 0);
    p.y = max(min(p.y, 600), 0);
    float x = (p.x - 400) / 800.0f;
    float y = (p.y - 300) / 600.0f;
    D3DXVECTOR3 lookAt(x, 1.5f - y, -1.0f);

    // Restrict head movement to 60 degrees
    ApplyLookAtIK(lookAt, D3DXToRadian(60.0f));
}

// Uses shortest arc rotation algorithm to turn the head bone towards a position
void InverseKinematics::ApplyLookAtIK(D3DXVECTOR3 &lookAtTarget, float maxAngle) {
    // Start by transforming to local space
    D3DXMATRIX mtxToLocal;
    D3DXMatrixInverse(&mtxToLocal, NULL, &m_pHeadBone->CombinedTransformationMatrix);

    // Footnote: the matrix inverse could be avoided if we would ignore to update the local transform
    D3DXVECTOR3 localLookAt;
    D3DXVec3TransformCoord(&localLookAt, &lookAtTarget, &mtxToLocal );

    // Normalize local look at target
    D3DXVec3Normalize(&localLookAt, &localLookAt);

    // Get rotation angle and angle
    D3DXVECTOR3 localRotationAxis;
    D3DXVec3Cross(&localRotationAxis, &m_headForward, &localLookAt);
    D3DXVec3Normalize(&localRotationAxis, &localRotationAxis);

    float localAngle = acosf(D3DXVec3Dot(&m_headForward, &localLookAt));

    // Limit angle
    localAngle = min( localAngle, maxAngle );

    // Apply the transformation to the bone
    D3DXMATRIX rotation;
    D3DXMatrixRotationAxis(&rotation, &localRotationAxis, localAngle);
    m_pHeadBone->CombinedTransformationMatrix = rotation * m_pHeadBone->CombinedTransformationMatrix;

    // Update changes to child bones
    if (m_pHeadBone->pFrameFirstChild) {
        m_pSkinnedMesh->UpdateMatrices((Bone*)m_pHeadBone->pFrameFirstChild, &m_pHeadBone->CombinedTransformationMatrix);
    }
}
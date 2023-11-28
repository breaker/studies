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

    // Hand IK bones
    m_pShoulderBone = (Bone*)m_pSkinnedMesh->GetBone("Bone20");
    m_pElbowBone = (Bone*)m_pSkinnedMesh->GetBone("Bone21");
    m_pHandBone = (Bone*)m_pSkinnedMesh->GetBone("Bone27");
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

// Hand IK test function
void InverseKinematics::UpdateArmIK() {
    // Exit if bones are missing
    if (m_pShoulderBone == NULL || m_pElbowBone == NULL || m_pHandBone == NULL)
        return;

    //Get look at point from cursor location
    POINT p;
    GetCursorPos(&p);
    p.x = max(min(p.x, 800), 0);
    p.y = max(min(p.y, 600), 0);
    float x = (p.x - 400) / 800.0f;
    float y = (p.y - 300) / 600.0f;
    D3DXVECTOR3 target(x, 1.5f - y, -0.25f);

    // Grab a point located at (-0.5, 1.5, 0.0)
    // The elbow joint axis (0, 0, -1) in content
    ApplyArmIK(D3DXVECTOR3(0, 0, -1), target);
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
    if (m_pHeadBone->pFrameFirstChild)
        m_pSkinnedMesh->UpdateMatrices((Bone*)m_pHeadBone->pFrameFirstChild, &m_pHeadBone->CombinedTransformationMatrix);
}

// Two joints IK useful for arm and legs.
void InverseKinematics::ApplyArmIK(D3DXVECTOR3 &hingeAxis, D3DXVECTOR3 &target) {
    // The two joints algorithm works in two steps first we bend the middle(joint) bone so that the distance from
    // start bone to target position will be correct.
    // We blindly assume that the user given hingeAxis always will be perpendicular to both start and joint vectors.
    // - This will always be hold for elbows and knees for a human character.
    // After that we use the "look-at-IK" algorithm to turn the start bone so that we grab the target

    // Setup some vectors and positions
    D3DXVECTOR3 startPosition = D3DXVECTOR3(m_pShoulderBone->CombinedTransformationMatrix._41, m_pShoulderBone->CombinedTransformationMatrix._42, m_pShoulderBone->CombinedTransformationMatrix._43);
    D3DXVECTOR3 jointPosition = D3DXVECTOR3(m_pElbowBone->CombinedTransformationMatrix._41, m_pElbowBone->CombinedTransformationMatrix._42, m_pElbowBone->CombinedTransformationMatrix._43);
    D3DXVECTOR3 endPosition = D3DXVECTOR3(m_pHandBone->CombinedTransformationMatrix._41, m_pHandBone->CombinedTransformationMatrix._42, m_pHandBone->CombinedTransformationMatrix._43);

    D3DXVECTOR3 startToTarget = target - startPosition;
    D3DXVECTOR3 startToJoint = jointPosition - startPosition;
    D3DXVECTOR3 jointToEnd = endPosition - jointPosition;

    float distStartToTarget = D3DXVec3Length(&startToTarget);
    float distStartToJoint = D3DXVec3Length(&startToJoint);
    float distJointToEnd = D3DXVec3Length(&jointToEnd);

    // Calculate joint bone rotation
    // Calcualte current angle and wanted angle
    float wantedJointAngle = 0.0f;

    if (distStartToTarget >= distStartToJoint + distJointToEnd) {
        // Target out of reach
        wantedJointAngle = D3DXToRadian(180.0f);
    }
    else {
        //Calculate wanted joint angle (Using the Law of Cosines)
        float cosAngle = (distStartToJoint * distStartToJoint + distJointToEnd * distJointToEnd - distStartToTarget * distStartToTarget) / (2.0f * distStartToJoint * distJointToEnd);
        wantedJointAngle = acosf(cosAngle);
    }

    //Normalize vectors
    D3DXVECTOR3 nmlStartToJoint = startToJoint;
    D3DXVECTOR3 nmlJointToEnd = jointToEnd;
    D3DXVec3Normalize(&nmlStartToJoint, &nmlStartToJoint);
    D3DXVec3Normalize(&nmlJointToEnd, &nmlJointToEnd);

    //Calculate the current joint angle
    float currentJointAngle = acosf(D3DXVec3Dot(&(-nmlStartToJoint), &nmlJointToEnd));

    //Calculate rotation matrix
    float diffJointAngle = wantedJointAngle - currentJointAngle;
    D3DXMATRIX rotation;
    D3DXMatrixRotationAxis(&rotation, &hingeAxis, diffJointAngle);

    //Apply elbow transformation
    m_pElbowBone->TransformationMatrix = rotation * m_pElbowBone->TransformationMatrix;

    // Calcuate new end position
    // Calculate this in world position and transform it later to start bones local space
    D3DXMATRIX tempMatrix = m_pElbowBone->CombinedTransformationMatrix;
    tempMatrix._41 = 0.0f;
    tempMatrix._42 = 0.0f;
    tempMatrix._43 = 0.0f;
    tempMatrix._44 = 1.0f;

    D3DXVECTOR3 worldHingeAxis;
    D3DXVECTOR3 newJointToEnd;
    D3DXVec3TransformCoord(&worldHingeAxis, &hingeAxis, &tempMatrix);
    D3DXMatrixRotationAxis(&rotation, &worldHingeAxis, diffJointAngle);
    D3DXVec3TransformCoord(&newJointToEnd, &jointToEnd, &rotation);

    D3DXVECTOR3 newEndPosition;
    D3DXVec3Add(&newEndPosition, &newJointToEnd, &jointPosition);

    // Calculate start bone rotation
    D3DXMATRIX mtxToLocal;
    D3DXMatrixInverse(&mtxToLocal, NULL, &m_pShoulderBone->CombinedTransformationMatrix);

    D3DXVECTOR3 localNewEnd;
    D3DXVECTOR3 localTarget;
    D3DXVec3TransformCoord(&localNewEnd, &newEndPosition, &mtxToLocal);
    D3DXVec3TransformCoord(&localTarget, &target, &mtxToLocal);
    D3DXVec3Normalize(&localNewEnd, &localNewEnd);
    D3DXVec3Normalize(&localTarget, &localTarget);

    D3DXVECTOR3 localAxis;
    D3DXVec3Cross(&localAxis, &localNewEnd, &localTarget);
    if (D3DXVec3Length(&localAxis) == 0.0f)
        return;

    D3DXVec3Normalize(&localAxis, &localAxis);
    float localAngle = acosf(D3DXVec3Dot(&localNewEnd, &localTarget));

    // Apply the rotation that makes the bone turn
    D3DXMatrixRotationAxis(&rotation, &localAxis, localAngle);
    m_pShoulderBone->CombinedTransformationMatrix = rotation * m_pShoulderBone->CombinedTransformationMatrix;
    m_pShoulderBone->TransformationMatrix = rotation * m_pShoulderBone->TransformationMatrix;

    // Update matrices of child bones.
    if (m_pShoulderBone->pFrameFirstChild)
        m_pSkinnedMesh->UpdateMatrices((Bone*)m_pShoulderBone->pFrameFirstChild,
                                       &m_pShoulderBone->CombinedTransformationMatrix);
}
// Camera.cpp
//

#include "StdAfx.h"
#include <SGL/Camera.h>

SGL_NS_BEGIN

////////////////////////////////////////////////////////////////////////////////
// class Camera
////////////////////////////////////////////////////////////////////////////////

void Camera::Walk(float units)
{
    // move only on xz plane for land object
    if (m_CameraType == CAMERA_LAND)
        m_Pos += D3DXVECTOR3(m_Look.x, 0.0f, m_Look.z) * units;

    if (m_CameraType == CAMERA_AIR)
        m_Pos += m_Look * units;
}

void Camera::Strafe(float units)
{
    // move only on xz plane for land object
    if (m_CameraType == CAMERA_LAND)
        m_Pos += D3DXVECTOR3(m_Right.x, 0.0f, m_Right.z) * units;

    if (m_CameraType == CAMERA_AIR)
        m_Pos += m_Right * units;
}

void Camera::Fly(float units)
{
    // move only on y-axis for land object
    if (m_CameraType == CAMERA_LAND)
        m_Pos.y += units;

    if (m_CameraType == CAMERA_AIR)
        m_Pos += m_Up * units;
}

void Camera::Pitch(float angle)
{
    D3DXMATRIX T;
    D3DXMatrixRotationAxis(&T, &m_Right, angle);

    // rotate m_Up and m_Look around m_Right vector
    D3DXVec3TransformCoord(&m_Up,&m_Up, &T);
    D3DXVec3TransformCoord(&m_Look,&m_Look, &T);
}

void Camera::Yaw(float angle)
{
    D3DXMATRIX T;

    // rotate around world y (0, 1, 0) always for land object
    if (m_CameraType == CAMERA_LAND)
        D3DXMatrixRotationY(&T, angle);

    // rotate around own up vector for aircraft
    if (m_CameraType == CAMERA_AIR)
        D3DXMatrixRotationAxis(&T, &m_Up, angle);

    // rotate m_Right and m_Look around m_Up or y-axis
    D3DXVec3TransformCoord(&m_Right,&m_Right, &T);
    D3DXVec3TransformCoord(&m_Look,&m_Look, &T);
}

void Camera::Roll(float angle)
{
    // only roll for aircraft type
    if (m_CameraType == CAMERA_AIR) {
        D3DXMATRIX T;
        D3DXMatrixRotationAxis(&T, &m_Look,	angle);

        // rotate m_Up and m_Right around m_Look vector
        D3DXVec3TransformCoord(&m_Right,&m_Right, &T);
        D3DXVec3TransformCoord(&m_Up,&m_Up, &T);
    }
}

void Camera::GetViewMatrix(__out D3DXMATRIX* V)
{
    // Keep camera's axes orthogonal to eachother
    D3DXVec3Normalize(&m_Look, &m_Look);

    D3DXVec3Cross(&m_Up, &m_Look, &m_Right);
    D3DXVec3Normalize(&m_Up, &m_Up);

    D3DXVec3Cross(&m_Right, &m_Up, &m_Look);
    D3DXVec3Normalize(&m_Right, &m_Right);

    // Build the view matrix:
    float x = -D3DXVec3Dot(&m_Right, &m_Pos);
    float y = -D3DXVec3Dot(&m_Up, &m_Pos);
    float z = -D3DXVec3Dot(&m_Look, &m_Pos);

    (*V)(0, 0) = m_Right.x;
    (*V)(0, 1) = m_Up.x;
    (*V)(0, 2) = m_Look.x;
    (*V)(0, 3) = 0.0f;
    (*V)(1, 0) = m_Right.y;
    (*V)(1, 1) = m_Up.y;
    (*V)(1, 2) = m_Look.y;
    (*V)(1, 3) = 0.0f;
    (*V)(2, 0) = m_Right.z;
    (*V)(2, 1) = m_Up.z;
    (*V)(2, 2) = m_Look.z;
    (*V)(2, 3) = 0.0f;
    (*V)(3, 0) = x;
    (*V)(3, 1) = y;
    (*V)(3, 2) = z;
    (*V)(3, 3) = 1.0f;
}

SGL_NS_END

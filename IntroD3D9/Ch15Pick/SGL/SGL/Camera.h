// Camera.h
//

#pragma once

#include <SGL/Depends.h>
#include <SGL/Defs.h>

SGL_NS_BEGIN

////////////////////////////////////////////////////////////////////////////////
// class Camera
////////////////////////////////////////////////////////////////////////////////

class SGL_API Camera {
public:
    enum CAMERA_TYPE { CAMERA_LAND, CAMERA_AIR };

    Camera() {
        Init(CAMERA_AIR);
    }

    Camera(CAMERA_TYPE cameraType) {
        Init(cameraType);
    }

    ~Camera() {};

private:
    void Init(CAMERA_TYPE cameraType) {
        m_CameraType = cameraType;

        m_Pos   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        m_Right = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
        m_Up    = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
        m_Look  = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
    }

public:
    void Strafe(float units);   // left/right (X)
    void Fly(float units);      // up/down (Y)
    void Walk(float units);     // forward/backward (Z)

    void Pitch(float angle);    // rotate on right vector (+X)
    void Yaw(float angle);      // rotate on up vector (+Y)
    void Roll(float angle);     // rotate on look vector (+Z)

    void GetViewMatrix(__out D3DXMATRIX* V);

    void SetCameraType(CAMERA_TYPE cameraType) {
        m_CameraType = cameraType;
    }

    void GetPosition(__out D3DXVECTOR3* pos) const {
        *pos = m_Pos;
    }

    void SetPosition(const D3DXVECTOR3& pos) {
        m_Pos = pos;
    }

    void GetRight(__out D3DXVECTOR3* right) const {
        *right = m_Right;
    }

    void GetUp(__out D3DXVECTOR3* up) const {
        *up = m_Up;
    }

    void GetLook(__out D3DXVECTOR3* look) const {
        *look = m_Look;
    }

private:
    CAMERA_TYPE m_CameraType;
    D3DXVECTOR3 m_Right;
    D3DXVECTOR3 m_Up;
    D3DXVECTOR3 m_Look;
    D3DXVECTOR3 m_Pos;
};

SGL_NS_END

// RenderFog.h
//

// ˵��:
// ����ʹ��Ч���ļ��ĵ�������, Զ�������
// W/S: ǰ��
// A/D: ƫ�� (Yaw)
// UP/DOWN: ���� (Pitch)
// LEFT/RIGHT: ����ƽ��

#pragma once

////////////////////////////////////////////////////////////////////////////////
// �����ʽ FogVertex
////////////////////////////////////////////////////////////////////////////////

struct FogVertex {
    FogVertex(float x, float y, float z, float u0, float v0, float u1, float v1, float u2, float v2) :
        _x(x), _y(y), _z(z), _u0(u0), _v0(v0), _u1(u1), _v1(v1), _u2(u2), _v2(v2) {}

    float _x, _y, _z;
    float _u0, _v0;
    float _u1, _v1;
    float _u2, _v2;

    static const DWORD FVF;
};

////////////////////////////////////////////////////////////////////////////////
// ��Ⱦ�� RenderFog
////////////////////////////////////////////////////////////////////////////////

class RenderFog : public SGL::Render {
public:
    RenderFog() : m_Camera(SGL::Camera::CAMERA_AIR) {}
    BOOL Init(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType);
    BOOL InitFont();
    void DumpMesh(LPCTSTR fname, ID3DXMesh* mesh);

    virtual void Draw(float timeDelta);

private:
    CComPtr<ID3DXFont> m_Font;

    static LPCTSTR HeightMapFile;
    static LPCTSTR FogEffectFile;

    std::auto_ptr<SGL::Terrain> m_Terrain;
    CComPtr<ID3DXEffect>        m_FogEffect;

    D3DXHANDLE  m_FogTechHandle;

public:
    SGL::Camera m_Camera;
};

////////////////////////////////////////////////////////////////////////////////
// ���봦���� FogInput
////////////////////////////////////////////////////////////////////////////////

class FogInput : public SGL::Input {
public:
    virtual void ProcessInput(float timeDelta);
};

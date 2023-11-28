// Render.h
//

#ifndef __SGL_RENDER_H__
#define __SGL_RENDER_H__

#include <SGL/Depends.h>
#include <SGL/Defs.h>

#include <SGL/Utils.h>

_SGL_NS_BEGIN

////////////////////////////////////////////////////////////////////////////////
// D3DXCOLOR 颜色常量
////////////////////////////////////////////////////////////////////////////////

const D3DXCOLOR WHITE(D3DCOLOR_XRGB(255, 255, 255));
const D3DXCOLOR BLACK(D3DCOLOR_XRGB(0, 0, 0));
const D3DXCOLOR RED(D3DCOLOR_XRGB(255, 0, 0));
const D3DXCOLOR GREEN(D3DCOLOR_XRGB(0, 255, 0));
const D3DXCOLOR BLUE(D3DCOLOR_XRGB(0, 0, 255));
const D3DXCOLOR YELLOW(D3DCOLOR_XRGB(255, 255, 0));
const D3DXCOLOR CYAN(D3DCOLOR_XRGB( 0, 255, 255));
const D3DXCOLOR MAGENTA(D3DCOLOR_XRGB(255, 0, 255));

////////////////////////////////////////////////////////////////////////////////
// 材质和光照初始化
////////////////////////////////////////////////////////////////////////////////

inline void InitMaterial(__out D3DMATERIAL9* mtrl, const D3DXCOLOR& amb, const D3DXCOLOR& dif, const D3DXCOLOR& spc, const D3DXCOLOR& em, float pwr)
{
    _ASSERTE(mtrl != NULL);
    mtrl->Ambient   = amb;
    mtrl->Diffuse   = dif;
    mtrl->Specular  = spc;
    mtrl->Emissive  = em;
    mtrl->Power     = pwr;
}

inline D3DMATERIAL9 InitMaterial(const D3DXCOLOR& amb, const D3DXCOLOR& dif, const D3DXCOLOR& spc, const D3DXCOLOR& em, float pwr)
{
    D3DMATERIAL9 mtrl;
    InitMaterial(&mtrl, amb, dif, spc, em, pwr);
    return mtrl;
}

inline void InitDirLight(__out D3DLIGHT9* light, const D3DXVECTOR3& dir, const D3DXCOLOR& clr)
{
    _ASSERTE(light != NULL);
    ZeroObj(light);
    light->Type         = D3DLIGHT_DIRECTIONAL;
    light->Ambient      = clr * 0.6f;
    light->Diffuse      = clr;
    light->Specular     = clr * 0.6f;
    light->Direction    = dir;
}

inline void InitPointLight(__out D3DLIGHT9* light, const D3DXVECTOR3& pos, const D3DXCOLOR& clr)
{
    _ASSERTE(light != NULL);
    ZeroObj(light);
    light->Type         = D3DLIGHT_POINT;
    light->Ambient      = clr * 0.6f;
    light->Diffuse      = clr;
    light->Specular     = clr * 0.6f;
    light->Position     = pos;
    light->Range        = 1000.0f;
    light->Falloff      = 1.0f;
    light->Attenuation0 = 1.0f;
    light->Attenuation1 = 0.0f;
    light->Attenuation2 = 0.0f;
}

inline void InitSpotLight(__out D3DLIGHT9* light, const D3DXVECTOR3& pos, const D3DXVECTOR3& dir, const D3DXCOLOR& clr)
{
    _ASSERTE(light != NULL);
    ZeroObj(light);
    light->Type         = D3DLIGHT_SPOT;
    light->Ambient      = clr * 0.0f;
    light->Diffuse      = clr;
    light->Specular     = clr * 0.6f;
    light->Position     = pos;
    light->Direction    = dir;
    light->Range        = 1000.0f;
    light->Falloff      = 1.0f;
    light->Attenuation0 = 1.0f;
    light->Attenuation1 = 0.0f;
    light->Attenuation2 = 0.0f;
    light->Theta        = 0.4f;
    light->Phi          = 0.9f;
}

////////////////////////////////////////////////////////////////////////////////
// D3DMATERIAL9 材质常量
////////////////////////////////////////////////////////////////////////////////

const D3DMATERIAL9 WHITE_MTRL   = InitMaterial(WHITE, WHITE, WHITE, BLACK, 2.0f);
const D3DMATERIAL9 RED_MTRL     = InitMaterial(RED, RED, RED, BLACK, 2.0f);
const D3DMATERIAL9 GREEN_MTRL   = InitMaterial(GREEN, GREEN, GREEN, BLACK, 2.0f);
const D3DMATERIAL9 BLUE_MTRL    = InitMaterial(BLUE, BLUE, BLUE, BLACK, 2.0f);
const D3DMATERIAL9 YELLOW_MTRL  = InitMaterial(YELLOW, YELLOW, YELLOW, BLACK, 2.0f);

////////////////////////////////////////////////////////////////////////////////
// 渲染类 Render
////////////////////////////////////////////////////////////////////////////////

class SGL_API Render : private Uncopyable {
public:
    Render() : m_Width(0), m_Height(0), m_D3DDev(NULL), m_hWnd(NULL) {}
    virtual ~Render() {}

    HRESULT Init(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType,
                 D3DFORMAT bkBufFmt = D3DFMT_A8R8G8B8, D3DMULTISAMPLE_TYPE sampleType = D3DMULTISAMPLE_NONE, D3DFORMAT depthStencilFmt = D3DFMT_D24S8);

    virtual void Display(float timeDelta) {}

    HWND GetWindow() const {
        return m_hWnd;
    }

    UINT Width() const {
        return m_Width;
    }

    UINT Height() const {
        return m_Height;
    }

protected:
    CComPtr<IDirect3DDevice9>   m_D3DDev;
    UINT    m_Width;
    UINT    m_Height;
    HWND    m_hWnd;
};

_SGL_NS_END

#endif  // __SGL_RENDER_H__

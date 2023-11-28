// SGLRender.h
//

#ifndef __SGLRENDER_H__
#define __SGLRENDER_H__

#include <SGL/SGLDepends.h>
#include <SGL/SGLDefs.h>

#include <SGL/SGLUtils.h>

_SGL_NS_BEGIN

////////////////////////////////////////////////////////////////////////////////
// D3DXCOLOR ��ɫ����
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
// ���ʺ͹��ճ�ʼ������
////////////////////////////////////////////////////////////////////////////////

inline void InitMaterial(__out D3DMATERIAL9& mtrl, const D3DXCOLOR& amb, const D3DXCOLOR& dif, const D3DXCOLOR& spc, const D3DXCOLOR& em, float pwr)
{
    mtrl.Ambient    = amb;
    mtrl.Diffuse    = dif;
    mtrl.Specular   = spc;
    mtrl.Emissive   = em;
    mtrl.Power      = pwr;
}

inline D3DMATERIAL9 InitMaterial(const D3DXCOLOR& amb, const D3DXCOLOR& dif, const D3DXCOLOR& spc, const D3DXCOLOR& em, float pwr)
{
    D3DMATERIAL9 mtrl;
    InitMaterial(mtrl, amb, dif, spc, em, pwr);
    return mtrl;
}

inline void InitDirectionalLight(__out D3DLIGHT9& light, const D3DXVECTOR3& dir, const D3DXCOLOR& clr)
{
    ZeroMemory(&light, sizeof(light));
    light.Type      = D3DLIGHT_DIRECTIONAL;
    light.Ambient   = clr * 0.6f;
    light.Diffuse   = clr;
    light.Specular  = clr * 0.6f;
    light.Direction = dir;
}

inline void InitPointLight(__out D3DLIGHT9& light, const D3DXVECTOR3& pos, const D3DXCOLOR& clr)
{
    ZeroMemory(&light, sizeof(light));
    light.Type          = D3DLIGHT_POINT;
    light.Ambient       = clr * 0.6f;
    light.Diffuse       = clr;
    light.Specular      = clr * 0.6f;
    light.Position      = pos;
    light.Range         = 1000.0f;
    light.Falloff       = 1.0f;
    light.Attenuation0  = 1.0f;
    light.Attenuation1  = 0.0f;
    light.Attenuation2  = 0.0f;
}

inline void InitSpotLight(__out D3DLIGHT9& light, const D3DXVECTOR3& pos, const D3DXVECTOR3& dir, const D3DXCOLOR& clr)
{
    ZeroMemory(&light, sizeof(light));
    light.Type          = D3DLIGHT_SPOT;
    light.Ambient       = clr * 0.0f;
    light.Diffuse       = clr;
    light.Specular      = clr * 0.6f;
    light.Position      = pos;
    light.Direction     = dir;
    light.Range         = 1000.0f;
    light.Falloff       = 1.0f;
    light.Attenuation0  = 1.0f;
    light.Attenuation1  = 0.0f;
    light.Attenuation2  = 0.0f;
    light.Theta         = 0.4f;
    light.Phi           = 0.9f;
}

////////////////////////////////////////////////////////////////////////////////
// D3DMATERIAL9 ���ʳ���
////////////////////////////////////////////////////////////////////////////////

const D3DMATERIAL9 WHITE_MTRL   = InitMaterial(WHITE, WHITE, WHITE, BLACK, 2.0f);
const D3DMATERIAL9 RED_MTRL     = InitMaterial(RED, RED, RED, BLACK, 2.0f);
const D3DMATERIAL9 GREEN_MTRL   = InitMaterial(GREEN, GREEN, GREEN, BLACK, 2.0f);
const D3DMATERIAL9 BLUE_MTRL    = InitMaterial(BLUE, BLUE, BLUE, BLACK, 2.0f);
const D3DMATERIAL9 YELLOW_MTRL  = InitMaterial(YELLOW, YELLOW, YELLOW, BLACK, 2.0f);

////////////////////////////////////////////////////////////////////////////////
// ��Ⱦ�쳣�� SGLRenderExcept
////////////////////////////////////////////////////////////////////////////////

class SGL_API SGLRenderExcept : public SGLExcept {
public:
    explicit SGLRenderExcept(DWORD err = SGL_ERR::ERR_UNKNOWN, const char* msg = "") : SGLExcept(err, msg) {}
    explicit SGLRenderExcept(const char* msg) : SGLExcept(msg) {}

    static const char* ERR_CREATE_D3DDEV_FAILED;
};

////////////////////////////////////////////////////////////////////////////////
// ��Ⱦ�� SGLRender
////////////////////////////////////////////////////////////////////////////////

class SGLGameRun;

class SGL_API SGLRender : private Uncopyable {
public:
    SGLRender(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType, D3DFORMAT bkBufFmt = D3DFMT_A8R8G8B8, D3DMULTISAMPLE_TYPE sampleType = D3DMULTISAMPLE_NONE, D3DFORMAT depthStencilFmt = D3DFMT_D24S8);
    virtual ~SGLRender() {};

    virtual void render(float timeDelta) {};

public:
    UINT getWidth() const {
        return width_;
    }
    UINT getHeight() const {
        return height_;
    }

private:
    BOOL createDev(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType, D3DFORMAT bkBufFmt, D3DMULTISAMPLE_TYPE sampleType, D3DFORMAT depthStencilFmt);

protected:
    CComPtr<IDirect3DDevice9>   d3ddev_;
    UINT    width_;
    UINT    height_;
};

_SGL_NS_END

#endif  // __SGLRENDER_H__

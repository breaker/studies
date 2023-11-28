// RenderMirror.cpp
//

#include "StdAfx.h"
#include "Mirror.h"
#include "RenderMirror.h"

////////////////////////////////////////////////////////////////////////////////
// 渲染类 RenderMirror
////////////////////////////////////////////////////////////////////////////////

LPCTSTR RenderMirror::TEX_FLOOR = _T("checker.jpg");
LPCTSTR RenderMirror::TEX_WALL = _T("brick0.jpg");
LPCTSTR RenderMirror::TEX_MIRROR = _T("ice.bmp");

D3DMATERIAL9 RenderMirror::FloorMtrl    = SGL::WHITE_MTRL;
D3DMATERIAL9 RenderMirror::WallMtrl     = SGL::WHITE_MTRL;
D3DMATERIAL9 RenderMirror::MirrorMtrl   = SGL::WHITE_MTRL;
D3DMATERIAL9 RenderMirror::TeapotMtrl   = SGL::YELLOW_MTRL;

BOOL RenderMirror::Init(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType)
{
    HRESULT hr = Render::Init(width, height, hwnd, windowed, devType);
    SGL_FAILED_DO(hr, MYTRACE_DXERR("Render::Init", hr); return FALSE);

    // Make walls have low specular reflectance - 20%.
    WallMtrl.Specular = SGL::WHITE * 0.2f;

    // Create the teapot.
    ID3DXMesh* teapot;
    D3DXCreateTeapot(m_D3DDev, &teapot, NULL);
    m_Teapot.Attach(teapot);

    // Create and specify geometry.  For this sample we draw a floor
    // and a wall with a mirror on it.  We put the floor, wall, and
    // mirror geometry in one vertex buffer.
    //
    //   |----|----|----|
    //   |Wall|Mirr|Wall|
    //   |    | or |    |
    //   /--------------/
    //  /   Floor      /
    // /--------------/
    //
    IDirect3DVertexBuffer9* vb;
    m_D3DDev->CreateVertexBuffer(24 * sizeof(Vertex), 0, Vertex::FVF, D3DPOOL_MANAGED, &vb, NULL);
    m_BackVB.Attach(vb);

    Vertex* v;
    vb->Lock(0, 0, (void**) &v, 0);

    // floor
    v[0] = Vertex(-7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
    v[1] = Vertex(-7.5f, 0.0f,   0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
    v[2] = Vertex( 7.5f, 0.0f,   0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

    v[3] = Vertex(-7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
    v[4] = Vertex( 7.5f, 0.0f,   0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
    v[5] = Vertex( 7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

    // wall
    v[6]  = Vertex(-7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
    v[7]  = Vertex(-7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
    v[8]  = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

    v[9]  = Vertex(-7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
    v[10] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
    v[11] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

    // Note: We leave gap in middle of walls for mirror
    v[12] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
    v[13] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
    v[14] = Vertex(7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

    v[15] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
    v[16] = Vertex(7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
    v[17] = Vertex(7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

    // mirror
    v[18] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
    v[19] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
    v[20] = Vertex( 2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

    v[21] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
    v[22] = Vertex( 2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
    v[23] = Vertex( 2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

    vb->Unlock();

    // Load Textures, set filters.
    IDirect3DTexture9 *floorTex, *wallTex, *mirrorTex;
    D3DXCreateTextureFromFile(m_D3DDev, TEX_FLOOR, &floorTex);
    m_FloorTex.Attach(floorTex);
    D3DXCreateTextureFromFile(m_D3DDev, TEX_WALL, &wallTex);
    m_WallTex.Attach(wallTex);
    D3DXCreateTextureFromFile(m_D3DDev, TEX_MIRROR, &mirrorTex);
    m_MirrorTex.Attach(mirrorTex);

    m_D3DDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_D3DDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_D3DDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

    // Lights.
    D3DXVECTOR3 lightDir(0.707f, -0.707f, 0.707f);
    D3DXCOLOR color(1.0f, 1.0f, 1.0f, 1.0f);
    D3DLIGHT9 light;
    SGL::InitDirLight(&light, lightDir, color);

    m_D3DDev->SetLight(0, &light);
    m_D3DDev->LightEnable(0, TRUE);

    m_D3DDev->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
    m_D3DDev->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

    // Set Camera.
    D3DXVECTOR3 pos(-10.0f, 3.0f, -15.0f);
    D3DXVECTOR3 target(0.0, 0.0f, 0.0f);
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

    D3DXMATRIX V;
    D3DXMatrixLookAtLH(&V, &pos, &target, &up);

    m_D3DDev->SetTransform(D3DTS_VIEW, &V);

    // Set projection matrix.
    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI / 4.0f, (float) width / (float) height, 1.0f, 1000.0f);
    m_D3DDev->SetTransform(D3DTS_PROJECTION, &proj);

    return TRUE;
}

void RenderMirror::Display(float timeDelta)
{
    D3DXVECTOR3 pos(cosf(m_ViewAngle) * m_ViewRadius, 3.0f, sinf(m_ViewAngle) * m_ViewRadius);
    D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
    D3DXMATRIX V;
    D3DXMatrixLookAtLH(&V, &pos, &target, &up);
    m_D3DDev->SetTransform(D3DTS_VIEW, &V);

    // Render

    m_D3DDev->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_ARGB(0xff, 0, 0, 0), 1.0f, 0);
    m_D3DDev->BeginScene();

    DrawScene();
    DrawMirror();

    m_D3DDev->EndScene();
    m_D3DDev->Present(NULL, NULL, NULL, NULL);
}

void RenderMirror::DrawScene()
{
    // draw teapot
    m_D3DDev->SetMaterial(&TeapotMtrl);
    m_D3DDev->SetTexture(0, 0);
    D3DXMATRIX W;
    D3DXMatrixTranslation(&W, m_TeapotPos.x, m_TeapotPos.y, m_TeapotPos.z);

    m_D3DDev->SetTransform(D3DTS_WORLD, &W);
    m_Teapot->DrawSubset(0);

    // ???
    D3DXMATRIX I;
    D3DXMatrixIdentity(&I);
    m_D3DDev->SetTransform(D3DTS_WORLD, &I);

    m_D3DDev->SetStreamSource(0, m_BackVB, 0, sizeof(Vertex));
    m_D3DDev->SetFVF(Vertex::FVF);

    // draw the floor
    m_D3DDev->SetMaterial(&FloorMtrl);
    m_D3DDev->SetTexture(0, m_FloorTex);
    m_D3DDev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

    // draw the walls
    m_D3DDev->SetMaterial(&WallMtrl);
    m_D3DDev->SetTexture(0, m_WallTex);
    m_D3DDev->DrawPrimitive(D3DPT_TRIANGLELIST, 6, 4);

    // draw the mirror
    m_D3DDev->SetMaterial(&MirrorMtrl);
    m_D3DDev->SetTexture(0, m_MirrorTex);
    m_D3DDev->DrawPrimitive(D3DPT_TRIANGLELIST, 18, 2);
}

void RenderMirror::DrawMirror()
{
    // Draw Mirror quad to stencil buffer ONLY.  In this way
    // only the stencil bits that correspond to the mirror will
    // be on.  Therefore, the reflected teapot can only be rendered
    // where the stencil bits are turned on, and thus on the mirror 
    // only.
    //

    m_D3DDev->SetRenderState(D3DRS_STENCILENABLE, TRUE);
    m_D3DDev->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
    m_D3DDev->SetRenderState(D3DRS_STENCILREF, 0x1);
    m_D3DDev->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
    m_D3DDev->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
    m_D3DDev->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
    m_D3DDev->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
    m_D3DDev->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);

    // disable writes to the depth and back buffers
    m_D3DDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_D3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_D3DDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
    m_D3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

    // draw the mirror to the stencil buffer
    m_D3DDev->SetStreamSource(0, m_BackVB, 0, sizeof(Vertex));
    m_D3DDev->SetFVF(Vertex::FVF);
    m_D3DDev->SetMaterial(&MirrorMtrl);
    m_D3DDev->SetTexture(0, m_MirrorTex);
    D3DXMATRIX I;
    D3DXMatrixIdentity(&I);
    m_D3DDev->SetTransform(D3DTS_WORLD, &I);
    m_D3DDev->DrawPrimitive(D3DPT_TRIANGLELIST, 18, 2);

    // re-enable depth writes
    m_D3DDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    // only draw reflected teapot to the pixels where the mirror
    // was drawn to.
    m_D3DDev->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
    m_D3DDev->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);

    // position reflection
    D3DXMATRIX W, T, R;
    D3DXPLANE plane(0.0f, 0.0f, 1.0f, 0.0f); // xy plane
    D3DXMatrixReflect(&R, &plane);
    D3DXMatrixTranslation(&T, m_TeapotPos.x, m_TeapotPos.y, m_TeapotPos.z); 
    W = T * R;

    // clear depth buffer and blend the reflected teapot with the mirror
    m_D3DDev->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
    m_D3DDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
    m_D3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

    // Finally, draw the reflected teapot
    m_D3DDev->SetTransform(D3DTS_WORLD, &W);
    m_D3DDev->SetMaterial(&TeapotMtrl);
    m_D3DDev->SetTexture(0, 0);

    m_D3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
    m_Teapot->DrawSubset(0);

    // Restore render states.
    m_D3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_D3DDev->SetRenderState(D3DRS_STENCILENABLE, FALSE);
    m_D3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

////////////////////////////////////////////////////////////////////////////////
// 输入处理类 MirrorInput
////////////////////////////////////////////////////////////////////////////////

void MirrorInput::ProcessInput(float timeDelta)
{
    RenderMirror* render = (RenderMirror*) m_Main->GetRender();

    // Update the scene

    D3DXVECTOR3& teapotPos = render->m_TeapotPos;
    if (SGL::KEY_DOWN(VK_LEFT))
        teapotPos.x -= 3.0f * timeDelta;
    if (SGL::KEY_DOWN(VK_RIGHT))
        teapotPos.x += 3.0f * timeDelta;

    float& radius = render->m_ViewRadius;
    if (SGL::KEY_DOWN(VK_UP))
        radius -= 2.0f * timeDelta;
    if (SGL::KEY_DOWN(VK_DOWN))
        radius += 2.0f * timeDelta;

    float& angle = render->m_ViewAngle;
    if (SGL::KEY_DOWN('A'))
        angle -= 0.5f * timeDelta;
    if (SGL::KEY_DOWN('S'))
        angle += 0.5f * timeDelta;

    Input::ProcessInput(timeDelta);
}

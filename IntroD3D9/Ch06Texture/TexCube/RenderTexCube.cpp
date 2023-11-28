// RenderTexCube.cpp
//

#include "StdAfx.h"
#include "TexCube.h"
#include "RenderTexCube.h"

////////////////////////////////////////////////////////////////////////////////
// 名字空间
////////////////////////////////////////////////////////////////////////////////

using SGL::KEY_DOWN;

////////////////////////////////////////////////////////////////////////////////
// 立方体类 Cube
////////////////////////////////////////////////////////////////////////////////

Cube::Cube(IDirect3DDevice9* dev) : d3ddev_(dev)
{
    // 创建顶点缓冲区
    IDirect3DVertexBuffer9* vb;
    d3ddev_->CreateVertexBuffer(24 * sizeof(Vertex), D3DUSAGE_WRITEONLY, Vertex::FVF, D3DPOOL_MANAGED, &vb, NULL);

    Vertex* v;
    vb->Lock(0, 0, (void**) &v, 0);

    // fill in the front face vertex data
    v[0] = Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
    v[1] = Vertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
    v[2] = Vertex( 1.0f,  1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
    v[3] = Vertex( 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

    // fill in the back face vertex data
    v[4] = Vertex(-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
    v[5] = Vertex( 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    v[6] = Vertex( 1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
    v[7] = Vertex(-1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);

    // fill in the top face vertex data
    v[8]  = Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
    v[9]  = Vertex(-1.0f, 1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
    v[10] = Vertex( 1.0f, 1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
    v[11] = Vertex( 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

    // fill in the bottom face vertex data
    v[12] = Vertex(-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);
    v[13] = Vertex( 1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
    v[14] = Vertex( 1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f);
    v[15] = Vertex(-1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);

    // fill in the left face vertex data
    v[16] = Vertex(-1.0f, -1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    v[17] = Vertex(-1.0f,  1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    v[18] = Vertex(-1.0f,  1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    v[19] = Vertex(-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    // fill in the right face vertex data
    v[20] = Vertex( 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    v[21] = Vertex( 1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    v[22] = Vertex( 1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    v[23] = Vertex( 1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    vb->Unlock();
    vb_.Attach(vb);

    // 创建索引缓冲区 (三角型图元)
    IDirect3DIndexBuffer9* ib;
    d3ddev_->CreateIndexBuffer(36 * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &ib, NULL);

    WORD* i = NULL;
    ib->Lock(0, 0, (void**) &i, 0);

    // fill in the front face index data
    i[0] = 0;
    i[1] = 1;
    i[2] = 2;
    i[3] = 0;
    i[4] = 2;
    i[5] = 3;

    // fill in the back face index data
    i[6] = 4;
    i[7]  = 5;
    i[8]  = 6;
    i[9] = 4;
    i[10] = 6;
    i[11] = 7;

    // fill in the top face index data
    i[12] = 8;
    i[13] =  9;
    i[14] = 10;
    i[15] = 8;
    i[16] = 10;
    i[17] = 11;

    // fill in the bottom face index data
    i[18] = 12;
    i[19] = 13;
    i[20] = 14;
    i[21] = 12;
    i[22] = 14;
    i[23] = 15;

    // fill in the left face index data
    i[24] = 16;
    i[25] = 17;
    i[26] = 18;
    i[27] = 16;
    i[28] = 18;
    i[29] = 19;

    // fill in the right face index data
    i[30] = 20;
    i[31] = 21;
    i[32] = 22;
    i[33] = 20;
    i[34] = 22;
    i[35] = 23;

    ib->Unlock();
    ib_.Attach(ib);
}

void Cube::draw(D3DXMATRIX* world, D3DMATERIAL9* mtrl, IDirect3DTexture9* tex)
{
    if (world != NULL)
        d3ddev_->SetTransform(D3DTS_WORLD, world);
    if (mtrl != NULL)
        d3ddev_->SetMaterial(mtrl);
    if (tex != NULL)
        d3ddev_->SetTexture(0, tex);

    d3ddev_->SetStreamSource(0, vb_, 0, sizeof(Vertex));
    d3ddev_->SetIndices(ib_);
    d3ddev_->SetFVF(Vertex::FVF);
    d3ddev_->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);
}

////////////////////////////////////////////////////////////////////////////////
// 渲染异常类 RenderTexCubeErr
////////////////////////////////////////////////////////////////////////////////

const char* RenderTexCubeErr::ERR_CREATE_TEXTURE = "create texture failed";

////////////////////////////////////////////////////////////////////////////////
// 渲染类 RenderTexCube
////////////////////////////////////////////////////////////////////////////////

LPCTSTR RenderTexCube::TEXTURE_FILE = _T("crate.jpg");

RenderTexCube::RenderTexCube(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType)
    : SGLRender(width, height, hwnd, windowed, devType), cube_(new Cube(d3ddev_))
{
    // Set a directional light.
    D3DLIGHT9 light;
    ZeroMemory(&light, sizeof(light));
    light.Type      = D3DLIGHT_DIRECTIONAL;
    light.Ambient   = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
    light.Diffuse   = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    light.Specular  = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
    light.Direction = D3DXVECTOR3(1.0f, -1.0f, 0.0f);
    d3ddev_->SetLight(0, &light);
    d3ddev_->LightEnable(0, TRUE);

    d3ddev_->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
    d3ddev_->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

    // Create texture.
    IDirect3DTexture9* tex;
    HRESULT hr = D3DXCreateTextureFromFile(d3ddev_, TEXTURE_FILE, &tex);
    SGL_FAILED_DO(hr, SGL_DXTRACE1("D3DXCreateTextureFromFile", hr); throw RenderTexCubeErr(RenderTexCubeErr::ERR_CREATE_TEXTURE));
    tex_.Attach(tex);

    // Set Texture Filter States.
    d3ddev_->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR); // 放大过滤器
    d3ddev_->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR); // 缩小过滤器
    d3ddev_->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR); // 多级渐进纹理过滤器

    // Set the projection matrix.
    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.5f, (float) width / (float) height, 1.0f, 1000.0f);
    d3ddev_->SetTransform(D3DTS_PROJECTION, &proj);
}

void RenderTexCube::render(float timeDelta)
{
    // Update the scene: update camera position.
    static float angle  = (3.0f * D3DX_PI) / 2.0f;
    static float height = 2.0f;

    if (KEY_DOWN(VK_LEFT))
        angle -= 0.5f * timeDelta;

    if (KEY_DOWN(VK_RIGHT))
        angle += 0.5f * timeDelta;

    if (KEY_DOWN(VK_UP))
        height += 5.0f * timeDelta;

    if (KEY_DOWN(VK_DOWN))
        height -= 5.0f * timeDelta;

    D3DXVECTOR3 pos(cosf(angle) * 3.0f, height, sinf(angle) * 3.0f);
    D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
    D3DXMATRIX V;
    D3DXMatrixLookAtLH(&V, &pos, &target, &up);
    d3ddev_->SetTransform(D3DTS_VIEW, &V);

    // Draw the scene:
    d3ddev_->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
    d3ddev_->BeginScene();

    d3ddev_->SetMaterial(&SGL::WHITE_MTRL);
    d3ddev_->SetTexture(0, tex_);

    cube_->draw(NULL, NULL, NULL);

    d3ddev_->EndScene();
    d3ddev_->Present(NULL, NULL, NULL, NULL);
}

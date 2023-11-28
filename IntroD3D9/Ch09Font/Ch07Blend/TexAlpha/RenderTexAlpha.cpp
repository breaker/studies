// RenderTexAlpha.cpp
//

#include "StdAfx.h"
#include "TexAlpha.h"
#include "RenderTexAlpha.h"


////////////////////////////////////////////////////////////////////////////////
// 立方体类 Cube
////////////////////////////////////////////////////////////////////////////////

Cube::Cube(IDirect3DDevice9* dev)
{
    // save a ptr to the device
    m_D3DDev = dev;

    IDirect3DVertexBuffer9* vb;
    m_D3DDev->CreateVertexBuffer(24 * sizeof(Vertex), D3DUSAGE_WRITEONLY, Vertex::FVF, D3DPOOL_MANAGED, &vb, NULL);
    m_VBuf.Attach(vb);

    Vertex* v;
    vb->Lock(0, 0, (void**)&v, 0);

    // build box

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

    IDirect3DIndexBuffer9* ib;
    m_D3DDev->CreateIndexBuffer(36 * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &ib, NULL);
    m_IBuf.Attach(ib);

    WORD* i = 0;
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
}

BOOL Cube::Draw(D3DXMATRIX* world, D3DMATERIAL9* mtrl, IDirect3DTexture9* tex)
{
    if (world != NULL)
        m_D3DDev->SetTransform(D3DTS_WORLD, world);
    if (mtrl != NULL)
        m_D3DDev->SetMaterial(mtrl);
    if (tex != NULL)
        m_D3DDev->SetTexture(0, tex);

    m_D3DDev->SetStreamSource(0, m_VBuf, 0, sizeof(Vertex));
    m_D3DDev->SetIndices(m_IBuf);
    m_D3DDev->SetFVF(Vertex::FVF);
    m_D3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// 渲染类 RenderTexAlpha
////////////////////////////////////////////////////////////////////////////////

LPCTSTR RenderTexAlpha::CRATE_TEXTURE_FILE = _T("cratewalpha.dds");

LPCTSTR RenderTexAlpha::BACK_TEXTURE_FILE = _T("lobbyxpos.jpg");

BOOL RenderTexAlpha::Init(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType)
{
    HRESULT hr = Render::Init(width, height, hwnd, windowed, devType);
    SGL_FAILED_DO(hr, MYTRACE_DXERR("Render::Init", hr); return FALSE);

    // Create the BackDrop quad.
    //

    IDirect3DVertexBuffer9* backVb;
    m_D3DDev->CreateVertexBuffer(6 * sizeof(Vertex), D3DUSAGE_WRITEONLY, Vertex::FVF, D3DPOOL_MANAGED, &backVb, NULL);
    m_BackVBuf.Attach(backVb);

    Vertex* v;
    backVb->Lock(0, 0, (void**)&v, 0);

    // quad built from two triangles, note texture coordinates:
    v[0] = Vertex(-10.0f, -10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
    v[1] = Vertex(-10.0f,  10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
    v[2] = Vertex( 10.0f,  10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

    v[3] = Vertex(-10.0f, -10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
    v[4] = Vertex( 10.0f,  10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
    v[5] = Vertex( 10.0f, -10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

    backVb->Unlock();

    // Create the cube.
    m_Cube.reset(new Cube(m_D3DDev));

    // Load the textures and set filters.
    //

    IDirect3DTexture9* crateTex;
    D3DXCreateTextureFromFile(m_D3DDev, CRATE_TEXTURE_FILE, &crateTex);
    m_CrateTex.Attach(crateTex);

    IDirect3DTexture9* backTex;
    D3DXCreateTextureFromFile(m_D3DDev, BACK_TEXTURE_FILE, &backTex);
    m_BackTex.Attach(backTex);

    m_D3DDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_D3DDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_D3DDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

    // set alpha blending stuff
    // use alpha channel in texture for alpha
    m_D3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_D3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

    // set blending factors so that alpha component determines transparency
    m_D3DDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_D3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    // disable lighting
    m_D3DDev->SetRenderState(D3DRS_LIGHTING, FALSE);

    // set camera
    D3DXVECTOR3 pos(0.0f, 0.f, -2.5f);
    D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

    D3DXMATRIX V;
    D3DXMatrixLookAtLH(&V, &pos, &target, &up);
    m_D3DDev->SetTransform(D3DTS_VIEW, &V);

    // Set projection matrix
    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.5f, (float) width / (float) height, 1.0f, 1000.0f);
    m_D3DDev->SetTransform(D3DTS_PROJECTION, &proj);

    return TRUE;
}

void RenderTexAlpha::Display(float timeDelta)
{
    // Update: Rotate the cube.
    D3DXMATRIX xRot;
    D3DXMatrixRotationX(&xRot, D3DX_PI * 0.2f);

    static float y = 0.0f;
    D3DXMATRIX yRot;
    D3DXMatrixRotationY(&yRot, y);
    y += timeDelta;

    if (y >= 2 * D3DX_PI)
        y = 0.0f;

    D3DXMATRIX worldMat = xRot * yRot;

    // Render

    m_D3DDev->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0xff, 0, 0, 0xff), 1.0f, 0);
    m_D3DDev->BeginScene();

    // draw back drop
    D3DXMATRIX I;
    D3DXMatrixIdentity(&I);
    m_D3DDev->SetStreamSource(0, m_BackVBuf, 0, sizeof(Vertex));
    m_D3DDev->SetFVF(Vertex::FVF);
    m_D3DDev->SetTexture(0, m_BackTex);
    m_D3DDev->SetTransform(D3DTS_WORLD, &I);
    m_D3DDev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

    // draw cube
    m_D3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_Cube->Draw(&worldMat, NULL, m_CrateTex);
    m_D3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    m_D3DDev->EndScene();
    m_D3DDev->Present(NULL, NULL, NULL, NULL);
}

////////////////////////////////////////////////////////////////////////////////
// 输入处理类 TexAlphaInput
////////////////////////////////////////////////////////////////////////////////

void TexAlphaInput::ProcessInput(float timeDelta)
{
    Input::ProcessInput(timeDelta);
}

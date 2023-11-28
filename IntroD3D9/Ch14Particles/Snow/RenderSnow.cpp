// RenderSnow.cpp
//

#include "StdAfx.h"
#include "Snow.h"
#include "RenderSnow.h"

////////////////////////////////////////////////////////////////////////////////
// class BasicScene
////////////////////////////////////////////////////////////////////////////////

LPCTSTR BasicScene::TextureFile = _T("desert.bmp");

BOOL BasicScene::Init() {
    IDirect3DVertexBuffer9* floor;
    m_D3DDev->CreateVertexBuffer(6 * sizeof(Vertex), 0, Vertex::FVF, D3DPOOL_MANAGED, &floor, NULL);
    m_Floor.Attach(floor);

    Vertex* v = 0;
    floor->Lock(0, 0, (void**) &v, 0);

    v[0] = Vertex(-20.0f, -2.5f, -20.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
    v[1] = Vertex(-20.0f, -2.5f,  20.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
    v[2] = Vertex( 20.0f, -2.5f,  20.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

    v[3] = Vertex(-20.0f, -2.5f, -20.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
    v[4] = Vertex( 20.0f, -2.5f,  20.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
    v[5] = Vertex( 20.0f, -2.5f, -20.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

    floor->Unlock();

    ID3DXMesh* pillar;
    D3DXCreateCylinder(m_D3DDev, 0.5f, 0.5f, 5.0f, 20, 20, &pillar, NULL);
    m_Pillar.Attach(pillar);

    IDirect3DTexture9* tex;
    HRESULT hr = D3DXCreateTextureFromFile(m_D3DDev, TextureFile, &tex);
    SGL_FAILED_DO(hr, MYTRACE_DX("D3DXCreateTextureFromFile", hr); return FALSE);
    m_FloorTex.Attach(tex);

    // Pre-Render Setup
    m_D3DDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_D3DDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_D3DDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

    D3DXVECTOR3 dir(0.707f, -0.707f, 0.707f);
    D3DXCOLOR clr(1.0f, 1.0f, 1.0f, 1.0f);
    D3DLIGHT9 light;
    SGL::InitDirLight(&light, dir, clr);

    m_D3DDev->SetLight(0, &light);
    m_D3DDev->LightEnable(0, TRUE);
    m_D3DDev->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
    m_D3DDev->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

    return TRUE;
}

void BasicScene::Draw(float scale) {
    // Render
    D3DXMATRIX T, R, P, S;

    D3DXMatrixScaling(&S, scale, scale, scale);

    // used to rotate cylinders to be parallel with world's y-axis
    D3DXMatrixRotationX(&R, -D3DX_PI * 0.5f);

    // draw floor
    D3DXMatrixIdentity(&T);
    T = T * S;
    m_D3DDev->SetTransform(D3DTS_WORLD, &T);
    m_D3DDev->SetMaterial(&SGL::WHITE_MTRL);
    m_D3DDev->SetTexture(0, m_FloorTex);
    m_D3DDev->SetStreamSource(0, m_Floor, 0, sizeof(Vertex));
    m_D3DDev->SetFVF(Vertex::FVF);
    m_D3DDev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

    // draw pillars
    m_D3DDev->SetMaterial(&SGL::BLUE_MTRL);
    m_D3DDev->SetTexture(0, NULL);
    for (int i = 0; i < 5; ++i) {
        D3DXMatrixTranslation(&T, -5.0f, 0.0f, -15.0f + (i * 7.5f));
        P = R * T * S;
        m_D3DDev->SetTransform(D3DTS_WORLD, &P);
        m_Pillar->DrawSubset(0);

        D3DXMatrixTranslation(&T, 5.0f, 0.0f, -15.0f + (i * 7.5f));
        P = R * T * S;
        m_D3DDev->SetTransform(D3DTS_WORLD, &P);
        m_Pillar->DrawSubset(0);
    }
}

////////////////////////////////////////////////////////////////////////////////
// 渲染类 RenderSnow
////////////////////////////////////////////////////////////////////////////////

LPCTSTR RenderSnow::MeshDumpFile = _T("MeshDump.txt");
LPCTSTR RenderSnow::TextureFile = _T("snowflake.dds");

BOOL RenderSnow::Init(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType) {
    HRESULT hr = Render::Init(width, height, hwnd, windowed, devType);
    SGL_FAILED_DO(hr, MYTRACE_DX("Render::Init", hr); return FALSE);

    // seed random number generator
    srand((unsigned) time(NULL));

    // Create Snow System.
    SGL::BoundingBox boundingBox;
    boundingBox.min = D3DXVECTOR3(-10.0f, -10.0f, -10.0f);
    boundingBox.max = D3DXVECTOR3( 10.0f,  10.0f,  10.0f);
    m_Snow.reset(new SGL::Snow(boundingBox, 5000));
    if (!m_Snow->Init(m_D3DDev, TextureFile))
        return FALSE;

    // Setup a basic scene.
    m_BasicScene.reset(new BasicScene(m_D3DDev));
    if (!m_BasicScene->Init())
        return FALSE;

    // Set projection matrix.
    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI / 4.0f, (float) width / (float) height, 1.0f, 5000.0f);
    m_D3DDev->SetTransform(D3DTS_PROJECTION, &proj);

    if (!InitFont())
        return FALSE;

    return TRUE;
}

BOOL RenderSnow::InitFont() {
    // ID3DXFont 内部使用 GDI (DrawText) 绘制文字
    D3DXFONT_DESC fd;
    SGL::ZeroObj(&fd);
    fd.Height           = 20;   // in logical units
    fd.Width            = 7;    // in logical units
    fd.Weight           = 500;  // boldness, range 0(light) - 1000(bold)
    fd.MipLevels        = D3DX_DEFAULT;
    fd.Italic           = FALSE;
    fd.CharSet          = DEFAULT_CHARSET;
    fd.OutputPrecision  = 0;
    fd.Quality          = 0;
    fd.PitchAndFamily   = 0;
    _tcscpy_s(fd.FaceName, _countof(fd.FaceName), _T("Times New Roman"));

    // Create an ID3DXFont based on D3DXFONT_DESC.
    ID3DXFont* font;
    HRESULT hr = D3DXCreateFontIndirect(m_D3DDev, &fd, &font);
    SGL_FAILED_DO(hr, MYTRACE_DX("D3DXCreateFontIndirect", hr); return FALSE);
    m_Font.Attach(font);

    return TRUE;
}

void RenderSnow::DumpMesh(LPCTSTR fname, ID3DXMesh* mesh) {
    MeshDump dump;
    if (!dump.Init(fname))
        return;
    dump.DumpVertex(mesh);
    dump.DumpIndex(mesh);
    dump.DumpAttributeBuffer(mesh);
    dump.DumpAttributeTable(mesh);
    dump.DumpAdjacencyBuffer(mesh);
}

void RenderSnow::Draw(float timeDelta) {
    // Update the view matrix representing the cameras
    // new position/orientation.
    D3DXMATRIX V;
    m_Camera.GetViewMatrix(&V);
    m_D3DDev->SetTransform(D3DTS_VIEW, &V);

    m_Snow->Update(timeDelta);

    // Render
    m_D3DDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0xff, 0x40, 0x40, 0x40), 1.0f, 0);
    m_D3DDev->BeginScene();

    D3DXMATRIX I;
    D3DXMatrixIdentity(&I);
    m_D3DDev->SetTransform(D3DTS_WORLD, &I);

    m_BasicScene->Draw(1.0f);

    // order important, render snow last.
    m_D3DDev->SetTransform(D3DTS_WORLD, &I);
    m_Snow->Render();

    // Draw FPS text
    RECT rect = {10, 10, m_Width, m_Height};
    m_Font->DrawText(NULL, m_FPS.CalcFPSStr(timeDelta), -1, &rect, DT_TOP | DT_LEFT, D3DCOLOR_ARGB(0xff, 0, 0x80, 0));

    m_D3DDev->EndScene();
    m_D3DDev->Present(NULL, NULL, NULL, NULL);
}

////////////////////////////////////////////////////////////////////////////////
// 输入处理类 SnowInput
////////////////////////////////////////////////////////////////////////////////

void SnowInput::ProcessInput(float timeDelta) {
    RenderSnow* render = (RenderSnow*) m_Main->GetRender();
    SGL::Camera& camera = render->m_Camera;

    // Update: Update the camera.

    if (SGL::KEY_DOWN('W'))
        camera.Walk(4.0f * timeDelta);

    if (SGL::KEY_DOWN('S'))
        camera.Walk(-4.0f * timeDelta);

    if (SGL::KEY_DOWN('A'))
        camera.Yaw(-1.0f * timeDelta);

    if (SGL::KEY_DOWN('D'))
        camera.Yaw(1.0f * timeDelta);

    if (SGL::KEY_DOWN(VK_UP))
        camera.Pitch(-1.0f * timeDelta);

    if (SGL::KEY_DOWN(VK_DOWN))
        camera.Pitch(1.0f * timeDelta);

    if (SGL::KEY_DOWN(VK_LEFT))
        camera.Strafe(-4.0f * timeDelta);

    if (SGL::KEY_DOWN(VK_RIGHT))
        camera.Strafe(4.0f * timeDelta);

    Input::ProcessInput(timeDelta);
}

////////////////////////////////////////////////////////////////////////////////
// class MeshDump
////////////////////////////////////////////////////////////////////////////////

BOOL MeshDump::Init(LPCTSTR fname) {
    errno_t err = _tfopen_s(&m_DumpStream, fname, _T("w"));
    return (err == 0);
}

void MeshDump::Clean() {
    if (m_DumpStream != NULL) {
        fclose(m_DumpStream);
        m_DumpStream = NULL;
    }
}

void MeshDump::DumpVertex(ID3DXBaseMesh* mesh) {
    _fputts(_T("Vertex:\n---------\n"), m_DumpStream);
    Vertex* v = NULL;
    DWORD FVF = mesh->GetFVF();
    mesh->LockVertexBuffer(0, (void**) &v);
    size_t sz = Vertex::Size(FVF);
    v->DumpFVF(m_DumpStream, FVF);
    for (DWORD i = 0; i < mesh->GetNumVertices(); ++i) {
        _ftprintf_s(m_DumpStream, _T("Vertex %d:"), i);
        Vertex::Data(v, i, sz)->Dump(m_DumpStream, FVF);
    }

    mesh->UnlockVertexBuffer();
    _fputts(_T("\n"), m_DumpStream);
}

void MeshDump::DumpIndex(ID3DXBaseMesh* mesh) {
    _fputts(_T("Index:\n--------\n"), m_DumpStream);
    WORD* idx = NULL;
    mesh->LockIndexBuffer(0, (void**) &idx);
    if (idx == NULL)
        return;
    for (DWORD i = 0; i < mesh->GetNumFaces(); ++i)
        _ftprintf_s(m_DumpStream, _T("Triangle %d: %d, %d, %d\n"), i, idx[i * 3], idx[i * 3 + 1], idx[i * 3 + 2]);
    mesh->UnlockIndexBuffer();
    _fputts(_T("\n"), m_DumpStream);
}

// 基类 ID3DXBaseMesh 没有 LockAttributeBuffer/UnlockAttributeBuffer 方法
void MeshDump::DumpAttributeBuffer(ID3DXMesh* mesh) {
    _fputts(_T("Attribute Buffer:\n-----------------\n"), m_DumpStream);
    DWORD* attrBuf = NULL;
    mesh->LockAttributeBuffer(0, &attrBuf);
    // an attribute for each face
    for (DWORD i = 0; i < mesh->GetNumFaces(); ++i)
        _ftprintf_s(m_DumpStream, _T("Triangle lives in subset %d: %d\n"), i, attrBuf[i]);
    mesh->UnlockAttributeBuffer();
    _fputts(_T("\n"), m_DumpStream);
}

void MeshDump::DumpAdjacencyBuffer(ID3DXBaseMesh* mesh) {
    _fputts(_T("Adjacency Buffer:\n-----------------\n"), m_DumpStream);
    // three enttries per face
    std::vector<DWORD> adjBuf(mesh->GetNumFaces() * 3);
    mesh->GenerateAdjacency(0.0f, &adjBuf[0]);
    for (DWORD i = 0; i < mesh->GetNumFaces(); ++i)
        _ftprintf_s(m_DumpStream, _T("Triangle's adjacent to triangle %d: %d, %d, %d\n"), i, adjBuf[i * 3], adjBuf[i * 3 + 1], adjBuf[i * 3 + 2]);
    _fputts(_T("\n"), m_DumpStream);
}

void MeshDump::DumpAttributeTable(ID3DXBaseMesh* mesh) {
    _fputts(_T("Attribute Table:\n----------------\n"), m_DumpStream);
    // number of entries in the attribute table
    DWORD numEntries = 0;
    mesh->GetAttributeTable(0, &numEntries);
    if (numEntries == 0)
        return;
    std::vector<D3DXATTRIBUTERANGE> table(numEntries);
    mesh->GetAttributeTable(&table[0], &numEntries);
    for (DWORD i = 0; i < numEntries; ++i)
        _ftprintf_s(m_DumpStream,
                    _T("Entry %d\n-----------\n")
                    _T("Subset ID:    %d\n")
                    _T("Face Start:   %d\n")
                    _T("Face Count:   %d\n")
                    _T("Vertex Start: %d\n")
                    _T("Vertex Count: %d\n\n"),
                    i, table[i].AttribId, table[i].FaceStart, table[i].FaceCount, table[i].VertexStart, table[i].VertexCount);

    _fputts(_T("\n"), m_DumpStream);
}

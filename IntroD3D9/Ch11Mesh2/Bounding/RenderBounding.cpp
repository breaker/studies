// RenderBounding.cpp
//

#include "StdAfx.h"
#include "Bounding.h"
#include "RenderBounding.h"

////////////////////////////////////////////////////////////////////////////////
// 渲染类 RenderBounding
////////////////////////////////////////////////////////////////////////////////

LPCTSTR RenderBounding::MeshDumpFile = _T("MeshDump.txt");

BOOL RenderBounding::Init(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType)
{
    HRESULT hr = Render::Init(width, height, hwnd, windowed, devType);
    SGL_FAILED_DO(hr, MYTRACE_DX("Render::Init", hr); return FALSE);

    // Set texture filters.
    m_D3DDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_D3DDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_D3DDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

    // Set Lights.
    D3DXVECTOR3 dir(1.0f, -1.0f, 1.0f);
    D3DXCOLOR clr(1.0f, 1.0f, 1.0f, 1.0f);
    D3DLIGHT9 light;
    SGL::InitDirLight(&light, dir, clr);

    m_D3DDev->SetLight(0, &light);
    m_D3DDev->LightEnable(0, TRUE);
    m_D3DDev->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
    m_D3DDev->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

    // Set camera.
    D3DXVECTOR3 pos(4.0f, 12.0f, -20.0f);
    D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

    D3DXMATRIX V;
    D3DXMatrixLookAtLH(&V, &pos, &target, &up);
    m_D3DDev->SetTransform(D3DTS_VIEW, &V);

    // Set projection matrix.
    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(&proj, SGL_PIBY2, (float) width / (float) height, 1.0f, 1000.0f);
    m_D3DDev->SetTransform(D3DTS_PROJECTION, &proj);

    if (!InitFont())
        return FALSE;

    return TRUE;
}

BOOL RenderBounding::InitFont()
{
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

    // Create an ID3DXFont based on 'fd'.
    ID3DXFont* font;
    HRESULT hr = D3DXCreateFontIndirect(m_D3DDev, &fd, &font);
    SGL_FAILED_DO(hr, MYTRACE_DX("D3DXCreateFontIndirect", hr); return FALSE);
    m_Font.Attach(font);

    return TRUE;
}

BOOL RenderBounding::LoadXFile(LPCTSTR fname)
{
    // Load the Bounding data.
    ID3DXBuffer* adjBuf = NULL;
    ID3DXBuffer* mtrlBuf = NULL;
    DWORD numMtrls = 0;
    ID3DXMesh* mesh = NULL;

    HRESULT hr = D3DXLoadMeshFromX(fname, D3DXMESH_MANAGED, m_D3DDev, &adjBuf, &mtrlBuf, NULL, &numMtrls, &mesh);
    SGL_FAILED_DO(hr, MYTRACE_DX("D3DXLoadMeshFromX", hr); return FALSE);
    m_Mesh.Attach(mesh);

    // Extract the materials, and load textures.
    m_Mtrls.clear();
    m_Tex.clear();
    if (mtrlBuf != NULL && numMtrls != 0) {
        D3DXMATERIAL* mtrls = (D3DXMATERIAL*) mtrlBuf->GetBufferPointer();
        for (DWORD i = 0; i < numMtrls; i++) {
            // the MatD3D property doesn't have an ambient value set
            // when its loaded, so set it now:
            mtrls[i].MatD3D.Ambient = mtrls[i].MatD3D.Diffuse;

            // save the ith material
            m_Mtrls.push_back(mtrls[i].MatD3D);

            CComPtr<IDirect3DTexture9> spTex(NULL);
            // check if the ith material has an associative texture
            if (mtrls[i].pTextureFilename != NULL) {
                // yes, load the texture for the ith subset
                IDirect3DTexture9* tex = NULL;
                D3DXCreateTextureFromFileA(m_D3DDev, mtrls[i].pTextureFilename, &tex);
                // save the loaded texture
                spTex.Attach(tex);
                m_Tex.push_back(spTex);
            }
            else
                // no texture for the ith subset
                m_Tex.push_back(spTex);
        }
    }
    SGL::Release(mtrlBuf);  // done w/ buffer

    // Optimize the mesh.
    hr = mesh->OptimizeInplace(D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE, (DWORD*) adjBuf->GetBufferPointer(), NULL, NULL, NULL);
    SGL::Release(adjBuf);
    SGL_FAILED_DO(hr, MYTRACE_DX("ID3DXMesh::OptimizeInplace", hr); return FALSE);

    SGL::BoundingSphere bdSphere;
    bdSphere.ComputeBoundingSphere(mesh);
    SGL::BoundingBox bdBox;
    bdBox.ComputeBoundingBox(mesh);

    ID3DXMesh *sphereMesh, *boxMesh;
    D3DXCreateSphere(m_D3DDev, bdSphere.radius, 20, 20, &sphereMesh, NULL);
    m_SphereMesh.Attach(sphereMesh);
    D3DXCreateBox(m_D3DDev, bdBox.max.x - bdBox.min.x, bdBox.max.y - bdBox.min.y, bdBox.max.z - bdBox.min.z, &boxMesh, NULL);
    m_BoxMesh.Attach(boxMesh);

    // Dump the Mesh Data to file.
    // 发现有些 X 文件没有关联纹理文件, 但 FVF 中却有 D3DFVF_TEX1, 如测试中的 bigship1.x
    DumpMesh(MeshDumpFile, m_Mesh);

    return TRUE;
}

void RenderBounding::DumpMesh(LPCTSTR fname, ID3DXMesh* mesh)
{
    MeshDump dump;
    if (!dump.Init(fname))
        return;
    dump.DumpVertex(mesh);
    dump.DumpIndex(mesh);
    dump.DumpAttributeBuffer(mesh);
    dump.DumpAttributeTable(mesh);
    dump.DumpAdjacencyBuffer(mesh);
}

void RenderBounding::Draw(float timeDelta)
{
    // Update: Rotate the mesh.
    static float y = 0.0f;
    D3DXMATRIX yRot;
    D3DXMatrixRotationY(&yRot, y);
    y += timeDelta;

    if (y >= SGL_2PI)
        y = 0.0f;

    D3DXMATRIX World = yRot;
    m_D3DDev->SetTransform(D3DTS_WORLD, &World);

    // Render
    m_D3DDev->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0xff, 0x40, 0x40, 0x40), 1.0f, 0);
    m_D3DDev->BeginScene();

    // draw the mesh
    for (DWORD i = 0; i < m_Mtrls.size(); i++) {
        m_D3DDev->SetMaterial(&m_Mtrls[i]);
        m_D3DDev->SetTexture(0, m_Tex[i]);
        m_Mesh->DrawSubset(i);
    }

    if (m_SphereMesh != NULL && m_BoxMesh != NULL) {
        // Draw bounding volume in blue and at 10% opacity
        D3DMATERIAL9 blue = SGL::BLUE_MTRL;
        blue.Diffuse.a = 0.10f; // 10% opacity

        m_D3DDev->SetMaterial(&blue);
        m_D3DDev->SetTexture(0, NULL);  // disable texture

        m_D3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        m_D3DDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        m_D3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

        if (m_BoundingType == BOUNDING_SPHERE)
            m_SphereMesh->DrawSubset(0);
        else
            m_BoxMesh->DrawSubset(0);

        m_D3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    }

    RECT rect = {10, 10, m_Width, m_Height};
    m_Font->DrawText(NULL, m_FPS.CalcFPSStr(timeDelta), -1, &rect, DT_TOP | DT_LEFT, D3DCOLOR_ARGB(0xff, 0, 0x80, 0));

    m_D3DDev->EndScene();
    m_D3DDev->Present(NULL, NULL, NULL, NULL);
}

////////////////////////////////////////////////////////////////////////////////
// 输入处理类 BoundingInput
////////////////////////////////////////////////////////////////////////////////

void BoundingInput::ProcessInput(float timeDelta)
{
    RenderBounding* render = (RenderBounding*) m_Main->GetRender();

    static float keyDuration = 0;
    keyDuration += timeDelta;
    const float ONEKEY_DURATION = 0.5f; // 小于这个持续时间都算一次 KEY_DOWN

    // 因为 KEY_DOWN 所用的 GetAsyncKeyState 对击键响应过于灵敏, 即使快速击键都会被 KEY_DOWN 检测出多次, 所以用击键持续时间控制
    // 原示例为了避免这个问题, 在窗口过程中处理 WM_KEYDOWN, 而没放到主循环中
    if (SGL::KEY_DOWN(VK_SPACE) && keyDuration > ONEKEY_DURATION) {
        render->m_BoundingType = (render->m_BoundingType == RenderBounding::BOUNDING_BOX ? RenderBounding::BOUNDING_SPHERE : RenderBounding::BOUNDING_BOX);
        keyDuration = 0;
    }

    Input::ProcessInput(timeDelta);
}


////////////////////////////////////////////////////////////////////////////////
// class MeshDump
////////////////////////////////////////////////////////////////////////////////

BOOL MeshDump::Init(LPCTSTR fname)
{
    errno_t err = _tfopen_s(&m_DumpStream, fname, _T("w"));
    return (err == 0);
}

void MeshDump::Clean()
{
    if (m_DumpStream != NULL) {
        fclose(m_DumpStream);
        m_DumpStream = NULL;
    }
}

void MeshDump::DumpVertex(ID3DXBaseMesh* mesh)
{
    _fputts(_T("Vertex:\n---------\n"), m_DumpStream);
    Vertex* v = NULL;
    DWORD FVF = mesh->GetFVF();
    mesh->LockVertexBuffer(0, (void**) &v);
    size_t sz = Vertex::Size(FVF);
    v->DumpFVF(m_DumpStream, FVF);
    for (DWORD i = 0; i < mesh->GetNumVertices(); i++) {
        _ftprintf_s(m_DumpStream, _T("Vertex %d:"), i);
        Vertex::Data(v, i, sz)->Dump(m_DumpStream, FVF);
    }

    mesh->UnlockVertexBuffer();
    _fputts(_T("\n"), m_DumpStream);
}

void MeshDump::DumpIndex(ID3DXBaseMesh* mesh)
{
    _fputts(_T("Index:\n--------\n"), m_DumpStream);
    WORD* idx = NULL;
    mesh->LockIndexBuffer(0, (void**) &idx);
    if (idx == NULL)
        return;
    for (DWORD i = 0; i < mesh->GetNumFaces(); i++)
        _ftprintf_s(m_DumpStream, _T("Triangle %d: %d, %d, %d\n"), i, idx[i * 3], idx[i * 3 + 1], idx[i * 3 + 2]);
    mesh->UnlockIndexBuffer();
    _fputts(_T("\n"), m_DumpStream);
}

// 基类 ID3DXBaseMesh 没有 LockAttributeBuffer/UnlockAttributeBuffer 方法
void MeshDump::DumpAttributeBuffer(ID3DXMesh* mesh)
{
    _fputts(_T("Attribute Buffer:\n-----------------\n"), m_DumpStream);
    DWORD* attrBuf = NULL;
    mesh->LockAttributeBuffer(0, &attrBuf);
    // an attribute for each face
    for (DWORD i = 0; i < mesh->GetNumFaces(); i++)
        _ftprintf_s(m_DumpStream, _T("Triangle lives in subset %d: %d\n"), i, attrBuf[i]);
    mesh->UnlockAttributeBuffer();
    _fputts(_T("\n"), m_DumpStream);
}

void MeshDump::DumpAdjacencyBuffer(ID3DXBaseMesh* mesh)
{
    _fputts(_T("Adjacency Buffer:\n-----------------\n"), m_DumpStream);
    // three enttries per face
    std::vector<DWORD> adjBuf(mesh->GetNumFaces() * 3);
    mesh->GenerateAdjacency(0.0f, &adjBuf[0]);
    for (DWORD i = 0; i < mesh->GetNumFaces(); i++)
        _ftprintf_s(m_DumpStream, _T("Triangle's adjacent to triangle %d: %d, %d, %d\n"), i, adjBuf[i * 3], adjBuf[i * 3 + 1], adjBuf[i * 3 + 2]);
    _fputts(_T("\n"), m_DumpStream);
}

void MeshDump::DumpAttributeTable(ID3DXBaseMesh* mesh)
{
    _fputts(_T("Attribute Table:\n----------------\n"), m_DumpStream);
    // number of entries in the attribute table
    DWORD numEntries = 0;
    mesh->GetAttributeTable(0, &numEntries);
    if (numEntries == 0)
        return;
    std::vector<D3DXATTRIBUTERANGE> table(numEntries);
    mesh->GetAttributeTable(&table[0], &numEntries);
    for (DWORD i = 0; i < numEntries; i++)
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

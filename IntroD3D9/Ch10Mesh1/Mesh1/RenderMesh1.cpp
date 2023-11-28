// RenderMesh1.cpp
//

#include "StdAfx.h"
#include "Mesh1.h"
#include "RenderMesh1.h"

////////////////////////////////////////////////////////////////////////////////
// 渲染类 RenderMesh1
////////////////////////////////////////////////////////////////////////////////

LPCTSTR RenderMesh1::TEXTURE_FILE[NumSubsets] = {
    _T("brick0.jpg"),
    _T("brick1.jpg"),
    _T("checker.jpg"),
};

BOOL RenderMesh1::InitFont()
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

BOOL RenderMesh1::Init(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType)
{
    HRESULT hr = Render::Init(width, height, hwnd, windowed, devType);
    SGL_FAILED_DO(hr, MYTRACE_DX("Render::Init", hr); return FALSE);

    // We are going to fill the empty mesh with the geometry of a box,
    // so we need 12 triangles and 24 vetices.
    ID3DXMesh* mesh;
    hr = D3DXCreateMeshFVF(12, 24, D3DXMESH_MANAGED, Vertex::FVF, m_D3DDev, &mesh);
    SGL_FAILED_DO(hr, MYTRACE_DX("D3DXCreateMeshFVF", hr); return FALSE);
    m_CubeMesh.Attach(mesh);

    // Fill in vertices of a box
    Vertex* v = 0;
    mesh->LockVertexBuffer(0, (void**) &v);

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

    mesh->UnlockVertexBuffer();

    // Define the triangles of the box
    WORD* i = 0;
    mesh->LockIndexBuffer(0, (void**) &i);

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

    mesh->UnlockIndexBuffer();

    // Specify the subset each triangle belongs to, in this example
    // we will use three subsets, the first two faces of the cube specified
    // will be in subset 0, the next two faces will be in subset 1 and
    // the the last two faces will be in subset 2.
    DWORD* attrBuf = 0;
    mesh->LockAttributeBuffer(0, &attrBuf);

    // 按顶点分为 subset
    for(int a = 0; a < 4; a++)
        attrBuf[a] = 0;

    for(int b = 4; b < 8; b++)
        attrBuf[b] = 1;

    for(int c = 8; c < 12; c++)
        attrBuf[c] = 2;

    mesh->UnlockAttributeBuffer();

    // Optimize the mesh to generate an attribute table.
    std::vector<DWORD> adjBuf(mesh->GetNumFaces() * 3);
    mesh->GenerateAdjacency(0.0f, &adjBuf[0]);

    hr = mesh->OptimizeInplace(D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE, &adjBuf[0], NULL, NULL, NULL);

    // Dump the Mesh Data to file.
    MeshDump dump;
    dump.Init(_T("MeshDump.txt"));
    dump.DumpVertex(mesh);
    dump.DumpIndex(mesh);
    dump.DumpAttributeTable(mesh);
    dump.DumpAttributeBuffer(mesh);
    dump.DumpAdjacencyBuffer(mesh);

    // Load the textures and set filters.
    IDirect3DTexture9* tex[NumSubsets];
    for (int i = 0; i < NumSubsets; i++) {
        hr = D3DXCreateTextureFromFile(m_D3DDev, TEXTURE_FILE[i], &tex[i]);
        SGL_FAILED_DO(hr, MYTRACE_DX("D3DXCreateTextureFromFile", hr); return FALSE);
        m_CubeTex[i].Attach(tex[i]);
    }

    m_D3DDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_D3DDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_D3DDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

    SGL_FALSE_DO(InitFont(), return FALSE);

    // Disable lighting.
    m_D3DDev->SetRenderState(D3DRS_LIGHTING, FALSE);

    // Set camera.
    D3DXVECTOR3 pos(0.0f, 0.f, -4.0f);
    D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

    D3DXMATRIX V;
    D3DXMatrixLookAtLH(&V, &pos, &target, &up);
    m_D3DDev->SetTransform(D3DTS_VIEW, &V);

    // Set projection matrix.
    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.5f, (float) width / (float) height, 1.0f, 1000.0f);
    m_D3DDev->SetTransform(D3DTS_PROJECTION, &proj);

    return TRUE;
}

void RenderMesh1::Draw(float timeDelta)
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

    D3DXMATRIX World = xRot * yRot;
    m_D3DDev->SetTransform(D3DTS_WORLD, &World);

    // Render
    m_D3DDev->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0xff, 0, 0, 0), 1.0f, 0);
    m_D3DDev->BeginScene();

    for (int i = 0; i < NumSubsets; i++) {
        m_D3DDev->SetTexture(0, m_CubeTex[i]);
        m_CubeMesh->DrawSubset(i);
    }

    RECT rect = {10, 10, m_Width, m_Height};
    m_Font->DrawText(NULL, m_FPS.CalcFPSStr(timeDelta), -1, &rect, DT_TOP | DT_LEFT, D3DCOLOR_ARGB(0xff, 0, 0x80, 0));

    m_D3DDev->EndScene();
    m_D3DDev->Present(NULL, NULL, NULL, NULL);
}

////////////////////////////////////////////////////////////////////////////////
// 输入处理类 Mesh1Input
////////////////////////////////////////////////////////////////////////////////

void Mesh1Input::ProcessInput(float timeDelta)
{
    RenderMesh1* render = (RenderMesh1*) m_Main->GetRender();
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

void MeshDump::DumpVertex(ID3DXMesh* mesh)
{
    _fputts(_T("Vertices:\n---------\n"), m_DumpStream);
    Vertex* v = NULL;
    mesh->LockVertexBuffer(0, (void**) &v);
    for (DWORD i = 0; i < mesh->GetNumVertices(); i++)
        _ftprintf_s(m_DumpStream, _T("Vertex %d: %f, %f, %f, %f, %f, %f, %f, %f\n"), i, v[i].x, v[i].y, v[i].z, v[i].nx, v[i].ny, v[i].nz, v[i].u, v[i].v);
    mesh->UnlockVertexBuffer();
    _fputts(_T("\n"), m_DumpStream);
}

void MeshDump::DumpIndex(ID3DXMesh* mesh)
{
    _fputts(_T("Indices:\n--------\n"), m_DumpStream);
    WORD* indices = NULL;
    mesh->LockIndexBuffer(0, (void**) &indices);
    for (DWORD i = 0; i < mesh->GetNumFaces(); i++)
        _ftprintf_s(m_DumpStream, _T("Triangle %d: %d, %d, %d\n"), i, indices[i * 3], indices[i * 3 + 1], indices[i * 3 + 2]);
    mesh->UnlockIndexBuffer();
    _fputts(_T("\n"), m_DumpStream);
}

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

void MeshDump::DumpAdjacencyBuffer(ID3DXMesh* mesh)
{
    _fputts(_T("Adjacency Buffer:\n-----------------\n"), m_DumpStream);
    // three enttries per face
    std::vector<DWORD> adjBuf(mesh->GetNumFaces() * 3);
    mesh->GenerateAdjacency(0.0f, &adjBuf[0]);
    for (DWORD i = 0; i < mesh->GetNumFaces(); i++)
        _ftprintf_s(m_DumpStream, _T("Triangle's adjacent to triangle %d: %d, %d, %d\n"), i, adjBuf[i * 3], adjBuf[i * 3 + 1], adjBuf[i * 3 + 2]);
    _fputts(_T("\n"), m_DumpStream);
}

void MeshDump::DumpAttributeTable(ID3DXMesh* mesh)
{
    _fputts(_T("Attribute Table:\n----------------\n"), m_DumpStream);
    // number of entries in the attribute table
    DWORD numEntries = 0;
    mesh->GetAttributeTable(0, &numEntries);
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

// RenderCube.h
//

#ifndef __RENDERCUBE_H__
#define __RENDERCUBE_H__

// FVF 顶点类型, 只包含位置
struct Vertex {
    Vertex(float x, float y, float z) : x_(x), y_(y), z_(z) {}
    float x_;
    float y_;
    float z_;
    static const DWORD FVF = D3DFVF_XYZ;
};

////////////////////////////////////////////////////////////////////////////////
// 绘制立方体异常类 RenderCubeExcept
////////////////////////////////////////////////////////////////////////////////

class RenderCubeExcept : public SGLExcept {
public:
    explicit RenderCubeExcept(DWORD err = SGL::SGL_ERR::ERR_UNKNOWN, const char* msg = "") : SGLExcept(err, msg) {}
    explicit RenderCubeExcept(const char* msg) : SGLExcept(msg) {}

    static const char* ERR_INIT_VERTEX_FAILED;
    static const char* ERR_INIT_COORD_FAILED;
};

////////////////////////////////////////////////////////////////////////////////
// 绘制立方体类 RenderCube
////////////////////////////////////////////////////////////////////////////////

class RenderCube : public SGLRender {
public:
    RenderCube(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType);

    virtual void render(float timeDelta);

private:
    BOOL initVertex();
    BOOL initCamera();

private:
    CComPtr<IDirect3DVertexBuffer9> vertex_buf_;
    CComPtr<IDirect3DIndexBuffer9>  index_buf_;
};


#endif  // __RENDERCUBE_H__

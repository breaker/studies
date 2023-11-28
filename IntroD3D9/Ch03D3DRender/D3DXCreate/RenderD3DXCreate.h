// RenderD3DXCreate.h
//

#ifndef __RENDERD3DXCREATE_H__
#define __RENDERD3DXCREATE_H__

// FVF 顶点类型, 只包含位置
struct Vertex {
    Vertex(float x, float y, float z) : x_(x), y_(y), z_(z) {}
    float x_;
    float y_;
    float z_;
    static const DWORD FVF = D3DFVF_XYZ;
};

////////////////////////////////////////////////////////////////////////////////
// 绘制多个物体类 RenderD3DXCreate
////////////////////////////////////////////////////////////////////////////////

class RenderD3DXCreate : public SGLRender {
public:
    RenderD3DXCreate(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType);

    virtual void render(float timeDelta);

private:
    BOOL initMesh();
    BOOL initCamera();
    BOOL initRender();
    void setCamera(float timeDelta);

private:
    CComPtr<ID3DXMesh>  objs_[5];
    D3DXMATRIX          objWorldM_[5];  // objs_ 对应的世界坐标变换矩阵
};

#endif  // __RENDERD3DXCREATE_H__

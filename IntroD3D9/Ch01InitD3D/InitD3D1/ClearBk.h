// ClearBk.h
//

#ifndef __CLEARBK_H__
#define __CLEARBK_H__

class ClearBk : public SGLRender {
public:
    ClearBk(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType) : SGLRender(width, height, hwnd, windowed, devType) {}

    virtual void render(float timeDelta);
};


#endif  // __CLEARBK_H__

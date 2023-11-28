// gameutils.cpp
//

#include "pch.h"
#include "utils.h"
#include "gameutils.h"

namespace gu
{

using std::wstring;

////////////////////////////////////////////////////////////////////////////////
// Globals
////////////////////////////////////////////////////////////////////////////////

// User-set game settings
wstring g_GameName;
int     g_GameWidth;
int     g_GameHeight;
wstring g_DataDir;

BOOL g_GameOver = FALSE;

// Direct3D variables
LPDIRECT3D9         g_D3D;
LPDIRECT3DDEVICE9   g_D3DDev;
LPDIRECT3DSURFACE9  g_BackBuffer;
LPD3DXSPRITE        g_SpriteObj;

// DirectInput variables
LPDIRECTINPUT8          g_DInput;
LPDIRECTINPUTDEVICE8    g_DIMouse;
LPDIRECTINPUTDEVICE8    g_DIKeyboard;
DIMOUSESTATE    g_MouseState;
char            g_Keys[256];
XINPUT_GAMEPAD  g_Controllers[4];

////////////////////////////////////////////////////////////////////////////////
// Game Utils Functions
////////////////////////////////////////////////////////////////////////////////

BOOL Direct3D_Init(HWND window, int width, int height, BOOL fullscreen)
{
    // initialize Direct3D
    g_D3D = Direct3DCreate9(D3D_SDK_VERSION);
    if (!g_D3D)
        return FALSE;

    // set Direct3D presentation parameters
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.hDeviceWindow = window;
    d3dpp.Windowed = (!fullscreen);
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.EnableAutoDepthStencil = 1;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
    d3dpp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferCount = 1;
    d3dpp.BackBufferWidth = width;
    d3dpp.BackBufferHeight = height;

    // create Direct3D device
    g_D3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_D3DDev);
    if (!g_D3DDev) return FALSE;

    // get a pointer to the back buffer surface
    g_D3DDev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &g_BackBuffer);

    // create sprite object
    D3DXCreateSprite(g_D3DDev, &g_SpriteObj);
    return 1;
}

void Direct3D_Shutdown()
{
    Release(g_SpriteObj);
    Release(g_D3DDev);
    Release(g_D3D);
}

void DrawSurface(LPDIRECT3DSURFACE9 dest, float x, float y, LPDIRECT3DSURFACE9 source)
{
    // get width/height from source surface
    D3DSURFACE_DESC desc;
    source->GetDesc(&desc);

    // create rects for drawing
    RECT source_rect = {0, 0, (long) desc.Width, (long) desc.Height };
    RECT dest_rect = {(long) x, (long) y, (long) x + desc.Width, (long) y + desc.Height};

    // draw the source surface onto the dest
    g_D3DDev->StretchRect(source, &source_rect, dest, &dest_rect, D3DTEXF_NONE);
}

LPDIRECT3DSURFACE9 LoadSurface(const wstring& fname)
{
    LPDIRECT3DSURFACE9 image = NULL;

    // get width and height from bitmap file
    D3DXIMAGE_INFO info;
    HRESULT result = D3DXGetImageInfoFromFile(fname.c_str(), &info);
    if (result != D3D_OK) return NULL;

    // create surface
    result = g_D3DDev->CreateOffscreenPlainSurface(
                 info.Width,        // width of the surface
                 info.Height,       // height of the surface
                 D3DFMT_X8R8G8B8,   // surface format
                 D3DPOOL_DEFAULT,   // memory pool to use
                 &image,            // pointer to the surface
                 NULL);             // reserved (always NULL)
    if (result != D3D_OK) return NULL;

    // load surface from file into newly created surface
    result = D3DXLoadSurfaceFromFile(
                 image,                     // destination surface
                 NULL,                      // destination palette
                 NULL,                      // destination rectangle
                 fname.c_str(),             // source filename
                 NULL,                      // source rectangle
                 D3DX_DEFAULT,              // controls how image is filtered
                 D3DCOLOR_XRGB(0, 0, 0),    // for transparency (0 for none)
                 NULL);                     // source image info (usually NULL)

    // make sure file was loaded okay
    if (result != D3D_OK) return NULL;
    return image;
}

LPDIRECT3DTEXTURE9 LoadTexture(const wstring& fname, D3DCOLOR transcolor)
{
    LPDIRECT3DTEXTURE9 texture = NULL;

    // get width and height from bitmap file
    D3DXIMAGE_INFO info;
    HRESULT result = D3DXGetImageInfoFromFile(fname.c_str(), &info);
    if (result != D3D_OK) return NULL;

    // create the new texture by loading a bitmap image file
    D3DXCreateTextureFromFileEx(
        g_D3DDev,           // Direct3D device object
        fname.c_str(),      // bitmap filename
        info.Width,         // bitmap image width
        info.Height,        // bitmap image height
        1,                  // mip-map levels (1 for no chain)
        D3DPOOL_DEFAULT,    // the type of surface (standard)
        D3DFMT_UNKNOWN,     // surface format (default)
        D3DPOOL_DEFAULT,    // memory class for the texture
        D3DX_DEFAULT,       // image filter
        D3DX_DEFAULT,       // mip filter
        transcolor,         // color key for transparency
        &info,              // bitmap file info (from loaded file)
        NULL,               // color palette
        &texture);          // destination texture

    // make sure the bitmap textre was loaded correctly
    if (result != D3D_OK) return NULL;
    return texture;
}

void Sprite_Draw_Frame(LPDIRECT3DTEXTURE9 texture, int destx, int desty, int framenum, int framew, int frameh, int columns)
{
    D3DXVECTOR3 position((float) destx, (float) desty, 0);
    D3DCOLOR white = D3DCOLOR_XRGB(255, 255, 255);

    RECT rect;
    rect.left = (framenum % columns) * framew;
    rect.top = (framenum / columns) * frameh;
    rect.right = rect.left + framew;
    rect.bottom = rect.top + frameh;

    g_SpriteObj->Draw(texture, &rect, NULL, &position, white);
}

void Sprite_Animate(int &frame, int startframe, int endframe, int direction, int &starttime, int delay)
{
    if ((int) GetTickCount() > starttime + delay) {
        starttime = GetTickCount();

        frame += direction;
        if (frame > endframe) frame = startframe;
        if (frame < startframe) frame = endframe;
    }
}

void Sprite_Transform_Draw(LPDIRECT3DTEXTURE9 image, int x, int y, int width, int height, int frame, int columns, float rotation, float scaleW, float scaleH, D3DCOLOR color)
{
    // create a scale vector
    D3DXVECTOR2 scale(scaleW, scaleH);

    // create a translate vector
    D3DXVECTOR2 trans((float) x, (float) y);

    // set center by dividing width and height by two
    D3DXVECTOR2 center((float) (width * scaleW) / 2, (float) (height * scaleH) / 2);

    // create 2D transformation matrix
    D3DXMATRIX mat;
    D3DXMatrixTransformation2D(&mat, NULL, 0, &scale, &center, rotation, &trans);

    // tell sprite object to use the transform
    g_SpriteObj->SetTransform(&mat);

    // calculate frame location in source image
    int fx = (frame % columns) * width;
    int fy = (frame / columns) * height;
    RECT srcRect = {fx, fy, fx + width, fy + height};

    // draw the sprite frame
    g_SpriteObj->Draw(image, &srcRect, NULL, NULL, color);

    // added in chapter 14
    D3DXMatrixIdentity(&mat);
    g_SpriteObj->SetTransform(&mat);
}

void Sprite_Transform_Draw(LPDIRECT3DTEXTURE9 image, int x, int y, int width, int height, int frame, int columns, float rotation, float scaling, D3DCOLOR color)
{
    Sprite_Transform_Draw(image, x, y, width, height, frame, columns, rotation, scaling, scaling, color);
}

// bounding  box collision detection
int Collision(SPRITE sprite1, SPRITE sprite2)
{
    RECT rect1;
    rect1.left = (long) sprite1.x;
    rect1.top = (long) sprite1.y;
    rect1.right = (long) (sprite1.x + sprite1.width * sprite1.scaling);
    rect1.bottom = (long) (sprite1.y + sprite1.height * sprite1.scaling);

    RECT rect2;
    rect2.left = (long) sprite2.x;
    rect2.top = (long) sprite2.y;
    rect2.right = (long) (sprite2.x + sprite2.width * sprite2.scaling);
    rect2.bottom = (long) (sprite2.y + sprite2.height * sprite2.scaling);

    RECT dest; // ignored
    return IntersectRect(&dest, &rect1, &rect2);
}

BOOL CollisionD(SPRITE sprite1, SPRITE sprite2)
{
    double radius1, radius2;

    // calculate radius 1
    if (sprite1.width > sprite1.height)
        radius1 = (sprite1.width * sprite1.scaling) / 2.0;
    else
        radius1 = (sprite1.height * sprite1.scaling) / 2.0;

    // center point 1
    double x1 = sprite1.x + radius1;
    double y1 = sprite1.y + radius1;
    D3DXVECTOR2 vector1((float) x1, (float) y1);

    // calculate radius 2
    if (sprite2.width > sprite2.height)
        radius2 = (sprite2.width * sprite2.scaling) / 2.0;
    else
        radius2 = (sprite2.height * sprite2.scaling) / 2.0;

    // center point 2
    double x2 = sprite2.x + radius2;
    double y2 = sprite2.y + radius2;
    D3DXVECTOR2 vector2((float) x2, (float) y2);

    // calculate distance
    double deltax = vector1.x - vector2.x;
    double deltay = vector2.y - vector1.y;
    double dist = sqrt((deltax * deltax) + (deltay * deltay));

    // return distance comparison
    return (dist < radius1 + radius2);
}

BOOL DirectInput_Init(HWND hwnd)
{
    // initialize DirectInput object
    DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**) &g_DInput, NULL);

    // initialize the keyboard
    g_DInput->CreateDevice(GUID_SysKeyboard, &g_DIKeyboard, NULL);
    g_DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
    g_DIKeyboard->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
    g_DIKeyboard->Acquire();

    // initialize the mouse
    g_DInput->CreateDevice(GUID_SysMouse, &g_DIMouse, NULL);
    g_DIMouse->SetDataFormat(&c_dfDIMouse);
    g_DIMouse->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
    g_DIMouse->Acquire();
    g_D3DDev->ShowCursor(FALSE);

    return TRUE;
}

void DirectInput_Update()
{
    // update mouse
    g_DIMouse->Poll();
    if (!SUCCEEDED(g_DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &g_MouseState))) {
        // mouse device lose, try to re-acquire
        g_DIMouse->Acquire();
    }

    // update keyboard
    g_DIKeyboard->Poll();
    if (!SUCCEEDED(g_DIKeyboard->GetDeviceState(256, (LPVOID) &g_Keys))) {
        // keyboard device lost, try to re-acquire
        g_DIKeyboard->Acquire();
    }

    // update controllers
    for (int i = 0; i < 4; i++) {
        ZeroMemory(&g_Controllers[i], sizeof(XINPUT_STATE));

        // get the state of the controller
        XINPUT_STATE state;
        DWORD result = XInputGetState(i, &state);

        // store state in global controllers array
        if (result == 0)
            g_Controllers[i] = state.Gamepad;
    }
}

int Mouse_X()
{
    return g_MouseState.lX;
}

int Mouse_Y()
{
    return g_MouseState.lY;
}

int Mouse_Button(int button)
{
    return g_MouseState.rgbButtons[button] & 0x80;
}

BOOL Key_Down(int key)
{
    return (BOOL) (g_Keys[key] & 0x80);
}

void DirectInput_Shutdown()
{
    if (g_DIKeyboard) {
        g_DIKeyboard->Unacquire();
        g_DIKeyboard->Release();
        g_DIKeyboard = NULL;
    }
    if (g_DIMouse) {
        g_DIMouse->Unacquire();
        g_DIMouse->Release();
        g_DIMouse = NULL;
    }
}

BOOL XInput_Controller_Found()
{
    XINPUT_CAPABILITIES caps;
    ZeroMemory(&caps, sizeof(XINPUT_CAPABILITIES));
    XInputGetCapabilities(0, XINPUT_FLAG_GAMEPAD, &caps);
    if (caps.Type != 0) return FALSE;

    return TRUE;
}

void XInput_Vibrate(int contNum, int amount)
{
    XINPUT_VIBRATION vibration;
    ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
    vibration.wLeftMotorSpeed = amount;
    vibration.wRightMotorSpeed = amount;
    XInputSetState(contNum, &vibration);
}

LPD3DXFONT MakeFont(const wstring& name, int size)
{
    LPD3DXFONT font = NULL;

    D3DXFONT_DESC desc = {
        size,                   // height
        0,                      // width
        0,                      // weight
        0,                      // miplevels
        FALSE,                  // italic
        DEFAULT_CHARSET,        // charset
        OUT_TT_PRECIS,          // output precision
        CLIP_DEFAULT_PRECIS,    // quality
        DEFAULT_PITCH,          // pitch and family
        L""                     // font name
    };

    wcscpy_s(desc.FaceName, LF_FACESIZE, name.c_str());
    D3DXCreateFontIndirect(g_D3DDev, &desc, &font);

    return font;
}

void FontPrint(LPD3DXFONT font, int x, int y, const wstring& text, D3DCOLOR color)
{
    // figure out the text boundary
    RECT rect = { x, y, 0, 0 };
    font->DrawText(NULL, text.c_str(), (INT) text.length(), &rect, DT_CALCRECT, color);

    // print the text
    font->DrawText(g_SpriteObj, text.c_str(), (INT) text.length(), &rect, DT_LEFT, color);
}

// DirectSound code

CSoundManager* g_DSound = NULL;

BOOL DirectSound_Init(HWND hwnd)
{
    // create DirectSound manager object
    g_DSound = new CSoundManager();

    // initialize DirectSound
    HRESULT result;
    result = g_DSound->Initialize(hwnd, DSSCL_PRIORITY);
    if (result != DS_OK) return FALSE;

    // set the primary buffer format
    result = g_DSound->SetPrimaryBufferFormat(2, 22050, 16);
    if (result != DS_OK) return FALSE;

    // return success
    return TRUE;
}

void DirectSound_Shutdown()
{
    if (g_DSound) delete g_DSound;
}

CSound* LoadSound(const wstring& fname)
{
    HRESULT result;

    // create local reference to wave data
    CSound* wave = NULL;

    // attempt to load the wave file
    result = g_DSound->Create(&wave, fname.c_str());
    if (result != DS_OK)
        wave = NULL;

    // return the wave
    return wave;
}

void PlaySound(CSound* sound)
{
    sound->Play();
}

void LoopSound(CSound* sound)
{
    sound->Play(0, DSBPLAY_LOOPING);
}

void StopSound(CSound* sound)
{
    sound->Stop();
}

// Mesh loading and rendering code

void SetCamera(float posx, float posy, float posz, float lookx, float looky, float lookz)
{
    float fov = D3DX_PI / 4.0;
    float aspectRatio = (float) g_GameWidth / (float) g_GameHeight;
    float nearRange = 1.0;
    float farRange = 2000.0;
    D3DXVECTOR3 updir = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
    D3DXVECTOR3 position = D3DXVECTOR3(posx, posy, posz);
    D3DXVECTOR3 target = D3DXVECTOR3(lookx, looky, lookz);

    // set the perspective
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH(&matProj, fov, aspectRatio, nearRange, farRange);
    g_D3DDev->SetTransform(D3DTS_PROJECTION, &matProj);

    // set up the camera view matrix
    D3DXMATRIX matView;
    D3DXMatrixLookAtLH(&matView, &position, &target, &updir);
    g_D3DDev->SetTransform(D3DTS_VIEW, &matView);
}

void SplitPath(const wstring& inputPath, _Out_ wstring* pathOnly, _Out_ wstring* fnameOnly)
{
    wstring fullPath(inputPath);
    std::replace(fullPath.begin(), fullPath.end(), L'\\', L'/');
    wstring::size_type lastSlashPos = fullPath.find_last_of(L'/');

    // check for there being no path element in the input
    if (lastSlashPos == wstring::npos) {
        *pathOnly = L"";
        *fnameOnly = fullPath;
    }
    else {
        if (pathOnly)
            *pathOnly = fullPath.substr(0, lastSlashPos);
        if (fnameOnly)
            *fnameOnly = fullPath.substr(lastSlashPos + 1, fullPath.size() - lastSlashPos - 1);
    }
}

BOOL DoesFileExist(const wstring &fname)
{
    return (_waccess(fname.c_str(), 0) != -1);
}

BOOL FindFile(_Out_ wstring* fname)
{
    if (!fname)
        return FALSE;

    // look for file using original filename and path
    if (DoesFileExist(*fname))
        return TRUE;

    // since the file was not found, try removing the path
    wstring pathOnly;
    wstring fnameOnly;
    SplitPath(*fname, &pathOnly, &fnameOnly);

    // is file found in current folder, without the path?
    if (DoesFileExist(fnameOnly)) {
        *fname = fnameOnly;
        return TRUE;
    }

    // not found
    return FALSE;
}

MODEL* LoadModel(const wstring& fname)
{
    MODEL* model = (MODEL*) malloc(sizeof(MODEL));
    LPD3DXBUFFER matbuffer;
    HRESULT result;

    // load mesh from the specified file
    result = D3DXLoadMeshFromX(
                fname.c_str(),          // filename
                D3DXMESH_SYSTEMMEM,     // mesh options
                g_D3DDev,               // Direct3D device
                NULL,                   // adjacency buffer
                &matbuffer,             // material buffer
                NULL,                   // special effects
                &model->material_count, // number of materials
                &model->mesh);          // resulting mesh

    if (result != D3D_OK) {
        MessageBox(NULL, L"Error loading model file", g_GameName.c_str(), MB_ICONERROR | MB_OK);
        return NULL;
    }

    // extract material properties and texture names from material buffer
    LPD3DXMATERIAL mtrls = (LPD3DXMATERIAL) matbuffer->GetBufferPointer();
    model->materials = new D3DMATERIAL9[model->material_count];
    model->textures  = new LPDIRECT3DTEXTURE9[model->material_count];

    // create the materials and textures
    wchar_t fn[MAX_PATH];
    for (DWORD i = 0; i < model->material_count; i++) {
        // grab the material
        model->materials[i] = mtrls[i].MatD3D;

        // set ambient color for material
        model->materials[i].Ambient = model->materials[i].Diffuse;

        model->textures[i] = NULL;
        if (mtrls[i].pTextureFilename != NULL) {
            // D3DXMATERIAL::pTextureFilename 是固定的 LPSTR 字符串
            // 需要转换为 wchar_t 宽字符串
            bzu::MbsToWcs(mtrls[i].pTextureFilename, fn, _countof(fn));
            wstring fpath = fn;
            if (FindFile(&fpath)) {
                result = D3DXCreateTextureFromFile(g_D3DDev, fn, &model->textures[i]);
                if (result != D3D_OK) {
                    MessageBox(NULL, L"Could not find texture file", L"Error", MB_OK | MB_ICONERROR);
                    return FALSE;
                }
            }
        }
    }

    // done using material buffer
    matbuffer->Release();

    return model;
}

void DeleteModel(MODEL* model)
{
    // remove materials from memory
    if (model->materials != NULL)
        delete[] model->materials;

    // remove textures from memory
    if (model->textures != NULL) {
        for (DWORD i = 0; i < model->material_count; i++) {
            if (model->textures[i] != NULL)
                model->textures[i]->Release();
        }
        delete[] model->textures;
    }

    // remove mesh from memory
    if (model->mesh != NULL)
        model->mesh->Release();

    // remove model struct from memory
    if (model != NULL)
        free(model);
}

void DrawModel(MODEL* model)
{
    // any materials in this mesh?
    if (model->material_count == 0) {
        model->mesh->DrawSubset(0);
    }
    else {
        // draw each mesh subset
        for (DWORD i = 0; i < model->material_count; i++) {
            // Set the material and texture for this subset
            g_D3DDev->SetMaterial(&model->materials[i]);

            if (model->textures[i]) {
                if (model->textures[i]->GetType() == D3DRTYPE_TEXTURE) {
                    D3DSURFACE_DESC desc;
                    model->textures[i]->GetLevelDesc(0, &desc);
                    if (desc.Width > 0) {
                        g_D3DDev->SetTexture(0, model->textures[i]);
                    }
                }
            }

            // Draw the mesh subset
            model->mesh->DrawSubset(i);
        }
    }
}

}   // namespace gu

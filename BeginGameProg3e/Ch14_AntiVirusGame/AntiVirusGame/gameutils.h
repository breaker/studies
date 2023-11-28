// gameutils.h
//

#pragma once

#include "dsutils.h"

namespace gu
{

////////////////////////////////////////////////////////////////////////////////
// Globals
////////////////////////////////////////////////////////////////////////////////

// User-set game settings
extern std::wstring g_GameName;
extern int          g_GameWidth;
extern int          g_GameHeight;
extern std::wstring g_DataDir;

// User-defined game functions
extern BOOL Game_Init(HWND hwnd);
extern void Game_Run(HWND hwnd);
extern void Game_End();

extern BOOL g_GameOver;

////////////////////////////////////////////////////////////////////////////////
// Game Utils Functions
////////////////////////////////////////////////////////////////////////////////

inline std::wstring GetResPath(LPCWSTR fname)
{
    return (g_DataDir + L"\\" + fname);
}

// macro to detect key presses
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

// Direct3D objects
extern LPDIRECT3D9 g_D3D;
extern LPDIRECT3DDEVICE9 g_D3DDev;
extern LPDIRECT3DSURFACE9 g_BackBuffer;
extern LPD3DXSPRITE g_SpriteObj;

// sprite structure
struct SPRITE {
    BOOL alive; // added ch14
    float x, y;
    int frame, columns;
    int width, height;
    float scaling, rotation;
    int startframe, endframe;
    int starttime, delay;
    int direction;
    float velx, vely;
    D3DCOLOR color;

    SPRITE()
    {
        frame = 0;
        columns = 1;
        width = height = 0;
        scaling = 1.0f;
        rotation = 0.0f;
        startframe = endframe = 0;
        direction = 1;
        starttime = delay = 0;
        velx = vely = 0.0f;
        color = D3DCOLOR_XRGB(255, 255, 255);
    }
};

template <class T>
inline void Release(_Inout_ T*& p)
{
    if (p != NULL) {
        p->Release();
        p = NULL;
    }
}

// Direct3D functions
BOOL Direct3D_Init(HWND hwnd, int width, int height, BOOL fullscreen);
void Direct3D_Shutdown();
LPDIRECT3DSURFACE9 LoadSurface(const std::wstring& fname);
void DrawSurface(LPDIRECT3DSURFACE9 dest, float x, float y, LPDIRECT3DSURFACE9 source);
LPDIRECT3DTEXTURE9 LoadTexture(const std::wstring& fname, D3DCOLOR transcolor = D3DCOLOR_XRGB(0, 0, 0));
void Sprite_Draw_Frame(LPDIRECT3DTEXTURE9 texture, int destx, int desty, int framenum, int framew, int frameh, int columns);
void Sprite_Animate(int &frame, int startframe, int endframe, int direction, int &starttime, int delay);

// added/modified in ch14
void Sprite_Transform_Draw(LPDIRECT3DTEXTURE9 image, int x, int y, int width, int height, int frame = 0, int columns = 1, float rotation = 0.0f, float scaleW = 1.0f, float scaleH = 1.0f, D3DCOLOR color = D3DCOLOR_XRGB(255, 255, 255));
void Sprite_Transform_Draw(LPDIRECT3DTEXTURE9 image, int x, int y, int width, int height, int frame = 0, int columns = 1, float rotation = 0.0f, float scaling = 1.0f, D3DCOLOR color = D3DCOLOR_XRGB(255, 255, 255));

// bounding box collision detection
int Collision(SPRITE sprite1, SPRITE sprite2);

// distance based collision detection
BOOL CollisionD(SPRITE sprite1, SPRITE sprite2);

// DirectInput objects, devices, and states
extern LPDIRECTINPUT8 g_DInput;
extern LPDIRECTINPUTDEVICE8 g_DIMouse;
extern LPDIRECTINPUTDEVICE8 g_DIKeyboard;
extern DIMOUSESTATE g_MouseState;
extern XINPUT_GAMEPAD g_Controllers[4];

// DirectInput functions
BOOL DirectInput_Init(HWND);
void DirectInput_Update();
void DirectInput_Shutdown();
BOOL Key_Down(int);
int Mouse_Button(int);
int Mouse_X();
int Mouse_Y();
void XInput_Vibrate(int contNum = 0, int amount = 65535);
BOOL XInput_Controller_Found();

// font functions
LPD3DXFONT MakeFont(const std::wstring& name, int size);
void FontPrint(LPD3DXFONT font, int x, int y, const std::wstring& text, D3DCOLOR color = D3DCOLOR_XRGB(255, 255, 255));

// primary DirectSound object
extern CSoundManager* g_DSound;

// function prototypes
BOOL DirectSound_Init(HWND hwnd);
void DirectSound_Shutdown();
CSound* LoadSound(const std::wstring& fname);
void PlaySound(CSound* sound);
void LoopSound(CSound* sound);
void StopSound(CSound* sound);

// 3D Mesh code added in chapter 13

// define the MODEL struct
struct MODEL {
    LPD3DXMESH mesh;
    D3DMATERIAL9* materials;
    LPDIRECT3DTEXTURE9* textures;
    DWORD material_count;
    D3DXVECTOR3 translate;
    D3DXVECTOR3 rotate;
    D3DXVECTOR3 scale;

    MODEL()
    {
        material_count = 0;
        mesh = NULL;
        materials = NULL;
        textures = NULL;
        translate = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        rotate = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
    }
};

// 3D mesh function prototypes
void DrawModel(MODEL* model);
void DeleteModel(MODEL* model);
MODEL* LoadModel(const std::wstring& fname);
BOOL FindFile(_Out_ std::wstring* fname);
BOOL DoesFileExist(const std::wstring& fname);
void SplitPath(const std::wstring& inputPath, _Out_ std::wstring* pathOnly, _Out_ std::wstring* fnameOnly);
void SetCamera(float posx, float posy, float posz, float lookx = 0.0f, float looky = 0.0f, float lookz = 0.0f);

}   // namespace gu

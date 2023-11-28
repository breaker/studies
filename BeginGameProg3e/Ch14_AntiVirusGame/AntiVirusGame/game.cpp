// game.cpp
//

#include "pch.h"
#include "gameutils.h"

using std::wstring;

////////////////////////////////////////////////////////////////////////////////
// Game Globals
////////////////////////////////////////////////////////////////////////////////

// font variables
LPD3DXFONT  g_Font;
LPD3DXFONT  g_DebugFont;

// timing variables
BOOL    g_Paused        = FALSE;
DWORD   g_Refresh       = 0;
DWORD   g_ScreenTime    = 0;
double  g_ScreenFPS     = 0.0;
double  g_ScreenCount   = 0.0;
DWORD   g_CoreTime      = 0;
double  g_CoreFPS       = 0.0;
double  g_CoreCount     = 0.0;
DWORD   g_CurrentTime;

// background scrolling variables
int     g_ScrollBufWidth;
int     g_ScrollBufHeight;
double  g_ScrollX;
double  g_ScrollY;
double  g_VirtualLevelSize;
double  g_VirtualScrollX;

LPDIRECT3DSURFACE9 g_Background;

// player variables
LPDIRECT3DTEXTURE9 g_PlayerShip;
gu::SPRITE g_Player;
enum PLAYER_STATES {
    PS_NORMAL       = 0,
    PS_PHASING      = 1,
    PS_CHARGING     = 2,
    PS_OVERLOADING  = 3
};

PLAYER_STATES   g_PlayerState       = PS_NORMAL;
PLAYER_STATES   g_PlayerStatePrev   = PS_NORMAL;
D3DXVECTOR2     g_PosHistory[8];
int     g_PosHistoryIndex   = 0;
DWORD   g_PosHistoryTimer   = 0;
double  g_ChargeAngle       = 0.0;
double  g_ChargeTweak       = 0.0;
double  g_ChargeTweakDir    = 1.0;
double  g_Energy    = 100.0;
double  g_Health    = 100.0;

// enemy virus objects
const int VIRUSES = 100;
LPDIRECT3DTEXTURE9 g_Virus1Image;
gu::SPRITE g_Viruses[VIRUSES];

const int FRAGMENTS = 300;
LPDIRECT3DTEXTURE9 g_FragmentImage;
gu::SPRITE g_Fragments[FRAGMENTS];

// bullet variables
LPDIRECT3DTEXTURE9 g_PurpleFire;
const int BULLETS = 300;
gu::SPRITE g_Bullets[BULLETS];
int g_PlayerShootTimer  = 0;
int g_FirePower         = 1;
int g_BulletCount       = 0;

// sound effects
CSound* g_Tisk;
CSound* g_Foom;
CSound* g_Charging;

// GUI elements
LPDIRECT3DTEXTURE9  g_EnergySlice;
LPDIRECT3DTEXTURE9  g_HealthSlice;

// controller vibration
int g_Vibrating = 0;
int g_Vibration = 100;

////////////////////////////////////////////////////////////////////////////////
// Game Functions
////////////////////////////////////////////////////////////////////////////////

BOOL gu::Game_Init(HWND hwnd)
{
    Direct3D_Init(hwnd, g_GameWidth, g_GameHeight, FALSE);
    DirectInput_Init(hwnd);
    DirectSound_Init(hwnd);

    g_ScrollBufWidth    = g_GameWidth * 2;
    g_ScrollBufHeight   = g_GameHeight;
    g_VirtualLevelSize  = g_ScrollBufWidth * 5;

    // create a font
    g_Font = MakeFont(L"Arial Bold", 24);
    g_DebugFont = MakeFont(L"Arial", 14);

    // load background
    LPDIRECT3DSURFACE9 image = NULL;
    image = LoadSurface(GetResPath(L"binary.bmp"));
    if (!image) return FALSE;

    // load player sprite
    g_PlayerShip = LoadTexture(GetResPath(L"ufo.png"));
    g_Player.x = 100;
    g_Player.y = 350;
    g_Player.width = g_Player.height = 64;

    for (int n = 0; n < 4; n++)
        g_PosHistory[n] = D3DXVECTOR2(-100, 0);

    // load bullets
    g_PurpleFire = LoadTexture(GetResPath(L"purplefire.tga"));
    for (int n = 0; n < BULLETS; n++) {
        g_Bullets[n].alive = FALSE;
        g_Bullets[n].x = 0;
        g_Bullets[n].y = 0;
        g_Bullets[n].width = 55;
        g_Bullets[n].height = 16;
    }

    // load enemy viruses
    g_Virus1Image = LoadTexture(GetResPath(L"virus1.tga"));
    for (int n = 0; n < VIRUSES; n++) {
        g_Viruses[n].alive = TRUE;
        g_Viruses[n].width = 96;
        g_Viruses[n].height = 96;
        g_Viruses[n].x = (float) (1000 + rand() % g_ScrollBufWidth);
        g_Viruses[n].y = (float) (rand() % g_GameHeight);
        g_Viruses[n].velx = (float) ((rand() % 8) * -1);
        g_Viruses[n].vely = (float) (rand() % 2 - 1);
    }

    // load gui elements
    g_EnergySlice = LoadTexture(GetResPath(L"energyslice.tga"));
    g_HealthSlice = LoadTexture(GetResPath(L"healthslice.tga"));

    // load audio files
    g_Tisk = LoadSound(GetResPath(L"clip.wav"));
    g_Foom = LoadSound(GetResPath(L"longfoom.wav"));
    g_Charging = LoadSound(GetResPath(L"charging.wav"));

    // load memory fragments (energy)
    g_FragmentImage = LoadTexture(GetResPath(L"fragment.tga"));
    for (int n = 0; n < FRAGMENTS; n++) {
        g_Fragments[n].alive = TRUE;
        D3DCOLOR fragmentcolor = D3DCOLOR_ARGB(200 + rand() % 55, 150 + rand() % 100, 150 + rand() % 100, 150 + rand() % 100);
        g_Fragments[n].color = fragmentcolor;
        g_Fragments[n].width = 128;
        g_Fragments[n].height = 128;
        g_Fragments[n].scaling = (float) (rand() % 20 + 10) / 150.0f;
        g_Fragments[n].rotation = (float) (rand() % 360);
        g_Fragments[n].velx = (float) (rand() % 4 + 1) * -1.0f;
        g_Fragments[n].vely = (float) (rand() % 10 - 5) / 10.0f;
        g_Fragments[n].x = (float) (rand() % g_ScrollBufWidth);
        g_Fragments[n].y = (float) (rand() % g_GameHeight);
    }

    // create background
    HRESULT result = g_D3DDev->CreateOffscreenPlainSurface(g_ScrollBufWidth, g_ScrollBufHeight, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &g_Background, NULL);
    if (result != D3D_OK) return FALSE;

    // copy image to upper left corner of background
    RECT source_rect = {0, 0, g_GameWidth, g_GameHeight };
    RECT dest_ul = { 0, 0, g_GameWidth, g_GameHeight };

    g_D3DDev->StretchRect(image, &source_rect, g_Background, &dest_ul, D3DTEXF_NONE);

    // copy image to upper right corner of background
    RECT dest_ur = { g_GameWidth, 0, g_GameWidth*2, g_GameHeight };

    g_D3DDev->StretchRect(image, &source_rect, g_Background, &dest_ur, D3DTEXF_NONE);

    // get pointer to the back buffer
    g_D3DDev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &g_BackBuffer);

    // remove image
    image->Release();

    return TRUE;
}

void gu::Game_End()
{
    g_Background->Release();
    g_Font->Release();
    g_DebugFont->Release();
    g_FragmentImage->Release();
    delete g_Charging;
    delete g_Foom;
    delete g_Tisk;
    g_EnergySlice->Release();
    g_HealthSlice->Release();
    g_Virus1Image->Release();
    g_PurpleFire->Release();
    g_PlayerShip->Release();

    DirectSound_Shutdown();
    DirectInput_Shutdown();
    Direct3D_Shutdown();
}

template <class T>
wstring static ToString(const T & t, int places = 2)
{
    std::wostringstream oss;
    oss.precision(places);
    oss.setf(std::ios_base::fixed);
    oss << t;
    return oss.str();
}

void MovePlayer(float movex, float movey)
{
    // cannot move while overloading!
    if (g_PlayerState == PS_OVERLOADING || g_PlayerStatePrev == PS_OVERLOADING)
        return;

    float multi = 4.0f;
    g_Player.x += movex * multi;
    g_Player.y += movey * multi;
}

const double PI = 3.1415926535;
const double PI_UNDER_180 = 180.0f / PI;
const double PI_OVER_180 = PI / 180.0f;

double ToRadians(double degrees)
{
    return degrees * PI_OVER_180;
}

double ToDegrees(double radians)
{
    return radians * PI_UNDER_180;
}

double Wrap(double value, double bounds)
{
    double result = fmod(value, bounds);
    if (result < 0) result += bounds;
    return result;
}

double WrapAngleDegs(double degs)
{
    return Wrap(degs, 360.0);
}

double LinearVelocityX(double angle)
{
    // angle -= 90;
    if (angle < 0) angle = 360 + angle;
    return cos(angle * PI_OVER_180);
}

double LinearVelocityY(double angle)
{
    // angle -= 90;
    if (angle < 0) angle = 360 + angle;
    return sin(angle * PI_OVER_180);
}

void AddEnergy(double value)
{
    g_Energy += value;
    if (g_Energy < 0.0) g_Energy = 0.0;
    if (g_Energy > 100.0) g_Energy = 100.0;
}

void Vibrate(int contnum, int amount, int length)
{
    g_Vibrating = 1;
    g_Vibration = length;
    gu::XInput_Vibrate(contnum, amount);
}

int FindBullet()
{
    int bullet = -1;
    for (int n = 0; n < BULLETS; n++) {
        if (!g_Bullets[n].alive) {
            bullet = n;
            break;
        }
    }
    return bullet;
}

BOOL PlayerOverload()
{
    // disallow overoad unless energy is at 100%
    if (g_Energy < 50.0) return FALSE;

    // reduce energy for this shot
    AddEnergy(-0.5);
    if (g_Energy < 0.0) {
        g_Energy = 0.0;
        return FALSE;
    }

    // play charging sound
    gu::PlaySound(g_Charging);

    // vibrate controller
    Vibrate(0, 20000, 20);

    int b1 = FindBullet();
    if (b1 == -1) return TRUE;
    g_Bullets[b1].alive = TRUE;
    g_Bullets[b1].velx = 0.0f;
    g_Bullets[b1].vely = 0.0f;
    g_Bullets[b1].rotation = (float) (rand() % 360);
    g_Bullets[b1].x = g_Player.x + g_Player.width;
    g_Bullets[b1].y = g_Player.y + g_Player.height / 2 - g_Bullets[b1].height / 2;
    g_Bullets[b1].y += (float) (rand() % 20 - 10);

    return TRUE;
}

void PlayerShoot()
{
    // limit firing rate
    if ((int) timeGetTime() < g_PlayerShootTimer + 100) return;
    g_PlayerShootTimer = timeGetTime();

    // reduce energy for this shot
    AddEnergy(-1.0);
    if (g_Energy < 1.0) {
        g_Energy = 0.0;
        return;
    }

    // play firing sound
    gu::PlaySound(g_Tisk);

    Vibrate(0, 25000, 10);

    int b1 = 0, b2 = 0, b3 = 0, b4 = 0;
    // launch bullets based on firepower level
    switch(g_FirePower) {
    case 1:
        // create a bullet
        b1 = FindBullet();
        if (b1 == -1) return;
        g_Bullets[b1].alive = TRUE;
        g_Bullets[b1].rotation = 0.0;
        g_Bullets[b1].velx = 12.0f;
        g_Bullets[b1].vely = 0.0f;
        g_Bullets[b1].x = g_Player.x + g_Player.width / 2;
        g_Bullets[b1].y = g_Player.y + g_Player.height / 2 - g_Bullets[b1].height / 2;
        break;

    case 2:
        // create bullet 1
        b1 = FindBullet();
        if (b1 == -1) return;
        g_Bullets[b1].alive = TRUE;
        g_Bullets[b1].rotation = 0.0;
        g_Bullets[b1].velx = 12.0f;
        g_Bullets[b1].vely = 0.0f;
        g_Bullets[b1].x = g_Player.x + g_Player.width / 2;
        g_Bullets[b1].y = g_Player.y + g_Player.height / 2 - g_Bullets[b1].height / 2;
        g_Bullets[b1].y -= 10;

        // create bullet 2
        b2 = FindBullet();
        if (b2 == -1) return;
        g_Bullets[b2].alive = TRUE;
        g_Bullets[b2].rotation = 0.0;
        g_Bullets[b2].velx = 12.0f;
        g_Bullets[b2].vely = 0.0f;
        g_Bullets[b2].x = g_Player.x + g_Player.width / 2;
        g_Bullets[b2].y = g_Player.y + g_Player.height / 2 - g_Bullets[b2].height / 2;
        g_Bullets[b2].y += 10;
        break;

    case 3:
        // create bullet 1
        b1 = FindBullet();
        if (b1 == -1) return;
        g_Bullets[b1].alive = TRUE;
        g_Bullets[b1].rotation = 0.0;
        g_Bullets[b1].velx = 12.0f;
        g_Bullets[b1].vely = 0.0f;
        g_Bullets[b1].x = g_Player.x + g_Player.width / 2;
        g_Bullets[b1].y = g_Player.y + g_Player.height / 2 - g_Bullets[b1].height / 2;

        // create bullet 2
        b2 = FindBullet();
        if (b2 == -1) return;
        g_Bullets[b2].alive = TRUE;
        g_Bullets[b2].rotation = 0.0;
        g_Bullets[b2].velx = 12.0f;
        g_Bullets[b2].vely = 0.0f;
        g_Bullets[b2].x = g_Player.x + g_Player.width / 2;
        g_Bullets[b2].y = g_Player.y + g_Player.height / 2 - g_Bullets[b2].height / 2;
        g_Bullets[b2].y -= 16;

        // create bullet 3
        b3 = FindBullet();
        if (b3 == -1) return;
        g_Bullets[b3].alive = TRUE;
        g_Bullets[b3].rotation = 0.0;
        g_Bullets[b3].velx = 12.0f;
        g_Bullets[b3].vely = 0.0f;
        g_Bullets[b3].x = g_Player.x + g_Player.width / 2;
        g_Bullets[b3].y = g_Player.y + g_Player.height / 2 - g_Bullets[b3].height / 2;
        g_Bullets[b3].y += 16;
        break;

    case 4:
        // create bullet 1
        b1 = FindBullet();
        if (b1 == -1) return;
        g_Bullets[b1].alive = TRUE;
        g_Bullets[b1].rotation = 0.0;
        g_Bullets[b1].velx = 12.0f;
        g_Bullets[b1].vely = 0.0f;
        g_Bullets[b1].x = g_Player.x + g_Player.width / 2;
        g_Bullets[b1].x += 8;
        g_Bullets[b1].y = g_Player.y + g_Player.height / 2 - g_Bullets[b1].height / 2;
        g_Bullets[b1].y -= 12;

        // create bullet 2
        b2 = FindBullet();
        if (b2 == -1) return;
        g_Bullets[b2].alive = TRUE;
        g_Bullets[b2].rotation = 0.0;
        g_Bullets[b2].velx = 12.0f;
        g_Bullets[b2].vely = 0.0f;
        g_Bullets[b2].x = g_Player.x + g_Player.width / 2;
        g_Bullets[b2].x += 8;
        g_Bullets[b2].y = g_Player.y + g_Player.height / 2 - g_Bullets[b2].height / 2;
        g_Bullets[b2].y += 12;

        // create bullet 3
        b3 = FindBullet();
        if (b3 == -1) return;
        g_Bullets[b3].alive = TRUE;
        g_Bullets[b3].rotation = 0.0;
        g_Bullets[b3].velx = 12.0f;
        g_Bullets[b3].vely = 0.0f;
        g_Bullets[b3].x = g_Player.x + g_Player.width / 2;
        g_Bullets[b3].y = g_Player.y + g_Player.height / 2 - g_Bullets[b3].height / 2;
        g_Bullets[b3].y -= 32;

        // create bullet 4
        b4 = FindBullet();
        if (b4 == -1) return;
        g_Bullets[b4].alive = TRUE;
        g_Bullets[b4].rotation = 0.0;
        g_Bullets[b4].velx = 12.0f;
        g_Bullets[b4].vely = 0.0f;
        g_Bullets[b4].x = g_Player.x + g_Player.width / 2;
        g_Bullets[b4].y = g_Player.y + g_Player.height / 2 - g_Bullets[b4].height / 2;
        g_Bullets[b4].y += 32;
        break;

    case 5:
        // create bullet 1
        b1 = FindBullet();
        if (b1 == -1) return;
        g_Bullets[b1].alive = TRUE;
        g_Bullets[b1].rotation = 0.0;
        g_Bullets[b1].velx = 12.0f;
        g_Bullets[b1].vely = 0.0f;
        g_Bullets[b1].x = g_Player.x + g_Player.width / 2;
        g_Bullets[b1].y = g_Player.y + g_Player.height / 2 - g_Bullets[b1].height / 2;
        g_Bullets[b1].y -= 12;

        // create bullet 2
        b2 = FindBullet();
        if (b2 == -1) return;
        g_Bullets[b2].alive = TRUE;
        g_Bullets[b2].rotation = 0.0;
        g_Bullets[b2].velx = 12.0f;
        g_Bullets[b2].vely = 0.0f;
        g_Bullets[b2].x = g_Player.x + g_Player.width / 2;
        g_Bullets[b2].y = g_Player.y + g_Player.height / 2 - g_Bullets[b2].height / 2;
        g_Bullets[b2].y += 12;

        // create bullet 3
        b3 = FindBullet();
        if (b3 == -1) return;
        g_Bullets[b3].alive = TRUE;
        g_Bullets[b3].rotation = -4.0;//  86.0;
        g_Bullets[b3].velx = (float) (12.0 * LinearVelocityX(g_Bullets[b3].rotation));
        g_Bullets[b3].vely = (float) (12.0 * LinearVelocityY(g_Bullets[b3].rotation));
        g_Bullets[b3].x = g_Player.x + g_Player.width / 2;
        g_Bullets[b3].y = g_Player.y + g_Player.height / 2 - g_Bullets[b3].height / 2;
        g_Bullets[b3].y -= 20;

        // create bullet 4
        b4 = FindBullet();
        if (b4 == -1) return;
        g_Bullets[b4].alive = TRUE;
        g_Bullets[b4].rotation = 4.0;   // 94.0;
        g_Bullets[b4].velx = (float) (12.0 * LinearVelocityX(g_Bullets[b4].rotation));
        g_Bullets[b4].vely = (float) (12.0 * LinearVelocityY(g_Bullets[b4].rotation));
        g_Bullets[b4].x = g_Player.x + g_Player.width / 2;
        g_Bullets[b4].y = g_Player.y + g_Player.height / 2 - g_Bullets[b4].height / 2;
        g_Bullets[b4].y += 20;
        break;
    }
}

void gu::Game_Run(HWND hwnd)
{
    if (!g_D3DDev) return;
    g_D3DDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 100), 1.0f, 0);

    // get current ticks
    g_CurrentTime = timeGetTime();

    // calculate core frame rate
    g_CoreCount += 1.0;
    if (g_CurrentTime > g_CoreTime + 1000) {
        g_CoreFPS = g_CoreCount;
        g_CoreCount = 0.0;
        g_CoreTime = g_CurrentTime;
    }

    // run update on normal processes (like input) at 60 hz

    if (g_CurrentTime > g_Refresh + 14) {
        g_Refresh = g_CurrentTime;

        DirectInput_Update();

        if (!g_Paused) {
            g_PlayerState = PS_NORMAL;

            if (Key_Down(DIK_UP) || Key_Down(DIK_W) || g_Controllers[0].sThumbLY > 2000)
                MovePlayer(0, -1);

            if (Key_Down(DIK_DOWN) || Key_Down(DIK_S) || g_Controllers[0].sThumbLY < -2000)
                MovePlayer(0, 1);

            if (Key_Down(DIK_LEFT) || Key_Down(DIK_A) || g_Controllers[0].sThumbLX < -2000)
                MovePlayer(-1, 0);

            if (Key_Down(DIK_RIGHT) || Key_Down(DIK_D) || g_Controllers[0].sThumbLX > 2000)
                MovePlayer(1, 0);

            if (Key_Down(DIK_Z) || g_Controllers[0].wButtons & XINPUT_GAMEPAD_B)
                g_PlayerState = PS_PHASING;

            if (Key_Down(DIK_X) || g_Controllers[0].wButtons & XINPUT_GAMEPAD_X)
                g_PlayerState = PS_CHARGING;

            if (Key_Down(DIK_C) || g_Controllers[0].wButtons & XINPUT_GAMEPAD_Y) {
                if (!PlayerOverload())
                    g_PlayerStatePrev = PS_OVERLOADING;
                else
                    g_PlayerState = PS_OVERLOADING;
            }

            if (Key_Down(DIK_SPACE) || g_Controllers[0].wButtons & XINPUT_GAMEPAD_A)
                PlayerShoot();

            // update background scrolling
            g_ScrollX += 1.0;
            if (g_ScrollY < 0)
                g_ScrollY = g_ScrollBufHeight - g_GameHeight;
            if (g_ScrollY > g_ScrollBufHeight - g_GameHeight)
                g_ScrollY = 0;
            if (g_ScrollX < 0)
                g_ScrollX = g_ScrollBufWidth - g_GameWidth;
            if (g_ScrollX > g_ScrollBufWidth - g_GameWidth)
                g_ScrollX = 0;

            // update virtual scroll position
            g_VirtualScrollX += 1.0;
            if (g_VirtualScrollX > g_VirtualLevelSize)
                g_VirtualScrollX = 0.0;

            // update overloaded bullets
            if (g_PlayerState == PS_NORMAL && g_PlayerStatePrev == PS_OVERLOADING) {
                int bulletcount = 0;

                // launch overloaded bullets
                for (int n = 0; n < BULLETS; n++) {
                    // overloaded bullets start with zero velocity
                    if (g_Bullets[n].alive && g_Bullets[n].velx == 0.0f) {
                        bulletcount++;
                        g_Bullets[n].rotation = (float) (rand() % 90 - 45);
                        g_Bullets[n].velx = (float) (20.0 * LinearVelocityX(g_Bullets[n].rotation));
                        g_Bullets[n].vely = (float) (20.0 * LinearVelocityY(g_Bullets[n].rotation));
                    }
                }
                if (bulletcount > 0) {
                    PlaySound(g_Foom);
                    Vibrate(0, 40000, 30);
                }

                g_PlayerStatePrev = PS_NORMAL;
            }

            // update normal bullets
            g_BulletCount = 0;
            for (int n = 0; n < BULLETS; n++) {
                if (g_Bullets[n].alive) {
                    g_BulletCount++;
                    g_Bullets[n].x += g_Bullets[n].velx;
                    g_Bullets[n].y += g_Bullets[n].vely;

                    if (g_Bullets[n].x < 0 || g_Bullets[n].x > g_GameWidth || g_Bullets[n].y < 0 || g_Bullets[n].y > g_GameHeight)
                        g_Bullets[n].alive = FALSE;
                }
            }

            // update enemy viruses
            for (int n = 0; n < VIRUSES; n++) {
                if (g_Viruses[n].alive) {
                    // move horiz based on x velocity
                    g_Viruses[n].x += g_Viruses[n].velx;
                    if (g_Viruses[n].x < -96.0f)
                        g_Viruses[n].x = (float) g_VirtualLevelSize;
                    if (g_Viruses[n].x > (float) g_VirtualLevelSize)
                        g_Viruses[n].x = -96.0f;

                    // move vert based on y velocity
                    g_Viruses[n].y += g_Viruses[n].vely;
                    if (g_Viruses[n].y < -96.0f)
                        g_Viruses[n].y = (float) g_GameHeight;
                    if (g_Viruses[n].y > g_GameHeight)
                        g_Viruses[n].y = -96.0f;
                }
            }

            // update energy fragments
            for (int n = 0; n < FRAGMENTS; n++) {
                if (g_Fragments[n].alive) {
                    g_Fragments[n].x += g_Fragments[n].velx;
                    if (g_Fragments[n].x < 0.0 - g_Fragments[n].width)
                        g_Fragments[n].x = (float) g_ScrollBufWidth;
                    if (g_Fragments[n].x > g_VirtualLevelSize)
                        g_Fragments[n].x = 0.0;
                    if (g_Fragments[n].y < 0.0 - g_Fragments[n].height)
                        g_Fragments[n].y = (float) g_GameHeight;
                    if (g_Fragments[n].y > g_GameHeight)
                        g_Fragments[n].y = 0.0;

                    if (g_PlayerState == PS_CHARGING) {
                        // temporarily enlarge fragment so it can be drawn in
                        float oldscale = g_Fragments[n].scaling;
                        g_Fragments[n].scaling *= 10.0;

                        // is it touching the player?
                        if (CollisionD(g_Player, g_Fragments[n])) {
                            // get center of player
                            float playerx = g_Player.x + g_Player.width / 2.0f;
                            float playery = g_Player.y + g_Player.height / 2.0f;

                            // get center of fragment
                            float fragmentx = g_Fragments[n].x;
                            float fragmenty = g_Fragments[n].y;

                            // suck fragment toward player
                            if (fragmentx < playerx) g_Fragments[n].x += 6.0f;
                            if (fragmentx > playerx) g_Fragments[n].x -= 6.0f;
                            if (fragmenty < playery) g_Fragments[n].y += 6.0f;
                            if (fragmenty > playery) g_Fragments[n].y -= 6.0f;
                        }

                        // restore fragment scale
                        g_Fragments[n].scaling = oldscale;

                        // after scooping up a fragment, check for collision
                        if (CollisionD(g_Player, g_Fragments[n])) {
                            AddEnergy(2.0);
                            g_Fragments[n].x = (float) (3000 + rand() % 1000);
                            g_Fragments[n].y = (float) (rand() % g_GameHeight);
                        }
                    }
                }
            }

            // update controller vibration
            if (g_Vibrating > 0) {
                g_Vibrating++;
                if (g_Vibrating > g_Vibration) {
                    XInput_Vibrate(0, 0);
                    g_Vibrating = 0;
                }
            }
        } // paused

        // calculate screen frame rate
        g_ScreenCount += 1.0;
        if (g_CurrentTime > g_ScreenTime + 1000) {
            g_ScreenFPS = g_ScreenCount;
            g_ScreenCount = 0.0;
            g_ScreenTime = g_CurrentTime;
        }

        // number keys used for testing
        if (Key_Down(DIK_F1)) g_FirePower = 1;
        if (Key_Down(DIK_F2)) g_FirePower = 2;
        if (Key_Down(DIK_F3)) g_FirePower = 3;
        if (Key_Down(DIK_F4)) g_FirePower = 4;
        if (Key_Down(DIK_F5)) g_FirePower = 5;

        if (Key_Down(DIK_P)) g_Paused = !g_Paused;

        if (Key_Down(DIK_E) || g_Controllers[0].bRightTrigger)
            AddEnergy(1.0);

        if (KEY_DOWN(VK_ESCAPE)) g_GameOver = TRUE;
        if (g_Controllers[0].wButtons & XINPUT_GAMEPAD_BACK)
            g_GameOver = TRUE;
    }

    // examine every live virus for collision
    for (int v = 0; v < VIRUSES; v++) {
        if (g_Viruses[v].alive) {
            // test collision with every live bullet
            for (int b = 0; b < BULLETS; b++) {
                if (g_Bullets[b].alive) {
                    if (Collision(g_Viruses[v], g_Bullets[b])) {
                        g_Bullets[b].alive = FALSE;
                        g_Viruses[v].alive = FALSE;
                    }
                }
            }
        }
    }

    // draw scrolling background
    RECT source_rect = {
        (long) g_ScrollX,
        (long) g_ScrollY,
        (long) g_ScrollX + g_GameWidth,
        (long) g_ScrollY + g_GameHeight
    };
    RECT dest_rect = { 0, 0, g_GameWidth, g_GameHeight};
    g_D3DDev->StretchRect(g_Background, &source_rect, g_BackBuffer, &dest_rect, D3DTEXF_NONE);

    // begin rendering
    if (g_D3DDev->BeginScene()) {
        g_SpriteObj->Begin(D3DXSPRITE_ALPHABLEND);

        int x = 0, y = 0;
        D3DCOLOR overcolor, chargecolor;
        switch(g_PlayerState) {
        case PS_PHASING:
            // draw phased out ship images
            for (int n = 0; n < 4; n++) {
                D3DCOLOR phasecolor = D3DCOLOR_ARGB(rand() % 150, 0, 255, 255);

                x = (int) g_Player.x + rand() % 12 - 6;
                y = (int) g_Player.y + rand() % 12 - 6;

                Sprite_Transform_Draw(g_PlayerShip, x, y, g_Player.width, g_Player.height, 0, 1, 0.0f, 1.0f, phasecolor);
            }
            break;

        case PS_CHARGING:
            // animate ship as a gravity well to suck energy
            chargecolor = D3DCOLOR_ARGB(150 + rand() % 100, 50 + rand() % 150, 50, 50);

            g_ChargeAngle = WrapAngleDegs(g_ChargeAngle - 3.0);
            g_ChargeTweak += 0.2 * g_ChargeTweakDir;
            if (g_ChargeTweak < -4.0 || g_ChargeTweak > 4.0)
                g_ChargeTweakDir *= -1;

            x = (int) (g_Player.x + cos(g_ChargeAngle) + g_ChargeTweak);
            y = (int) (g_Player.y + sin(g_ChargeAngle) + g_ChargeTweak);

            Sprite_Transform_Draw(g_PlayerShip, x, y, g_Player.width, g_Player.height, 0, 1, (float) ToRadians(g_ChargeAngle), 1.0f, chargecolor);
            break;

        case PS_OVERLOADING:
            // super weapon charges up
            for (int n = 0; n < 4; n++) {
                overcolor = D3DCOLOR_ARGB(150 + rand() % 100, 80, 255, 255);

                x = (int) g_Player.x + rand() % 12 - 6;
                y = (int) g_Player.y;

                Sprite_Transform_Draw(g_PlayerShip, x, y, g_Player.width, g_Player.height, 0, 1, 0.0f, 1.0f, overcolor);
            }
            break;

        case PS_NORMAL:
            // draw ship normally
            // reset shadows if state just changed
            if (g_PlayerStatePrev != g_PlayerState) {
                for (int n = 0; n < 8; n++) {
                    g_PosHistory[n].x = g_Player.x;
                    g_PosHistory[n].y = g_Player.y;
                }
            }

            D3DCOLOR shadowcolor = D3DCOLOR_ARGB(60, 0, 240, 240);

            if (g_CurrentTime > g_PosHistoryTimer + 40) {
                g_PosHistoryTimer = g_CurrentTime;
                g_PosHistoryIndex++;
                if (g_PosHistoryIndex > 7) {
                    g_PosHistoryIndex = 7;
                    for (int a = 1; a < 8; a++)
                        g_PosHistory[a - 1] = g_PosHistory[a];
                }

                g_PosHistory[g_PosHistoryIndex].x = g_Player.x;
                g_PosHistory[g_PosHistoryIndex].y = g_Player.y;
            }

            for (int n = 0; n < 8; n++) {
                shadowcolor = D3DCOLOR_ARGB(20 + n*10, 0, 240, 240);

                // draw shadows of previous ship position
                Sprite_Transform_Draw(g_PlayerShip, (int) g_PosHistory[n].x, (int) g_PosHistory[n].y, g_Player.width, g_Player.height, 0, 1, 0.0f, 1.0f, shadowcolor);
            }

            // draw ship normally
            D3DCOLOR shipcolor = D3DCOLOR_ARGB(255, 0, 255, 255);
            Sprite_Transform_Draw(g_PlayerShip, (int) g_Player.x, (int) g_Player.y, g_Player.width, g_Player.height, 0, 1, 0.0f, 1.0f, shipcolor);
            break;
        }

        // save player state
        g_PlayerStatePrev = g_PlayerState;

        // draw enemy viruses
        D3DCOLOR viruscolor = D3DCOLOR_ARGB(200, 255, 255, 255);
        for (int n = 0; n < VIRUSES; n++) {
            // is this virus sprite alive?
            if (g_Viruses[n].alive) {
                // is this virus sprite visible on the screen?
                if (g_Viruses[n].x > -96.0f && g_Viruses[n].x < g_GameWidth)
                    Sprite_Transform_Draw(g_Virus1Image, (int) g_Viruses[n].x, (int) g_Viruses[n].y, g_Viruses[n].width, g_Viruses[n].height, 0, 1, 0.0f, 1.0f, viruscolor);
            }
        }

        // draw bullets
        D3DCOLOR bulletcolor = D3DCOLOR_ARGB(255, 255, 255, 255);
        for (int n = 0; n < BULLETS; n++) {
            if (g_Bullets[n].alive)
                Sprite_Transform_Draw(g_PurpleFire, (int) g_Bullets[n].x, (int) g_Bullets[n].y, g_Bullets[n].width, g_Bullets[n].height, 0, 1, (float) ToRadians(g_Bullets[n].rotation), 1.0f, bulletcolor);
        }

        // draw energy fragments
        for (int n = 0; n < FRAGMENTS; n++) {
            if (g_Fragments[n].alive)
                Sprite_Transform_Draw(g_FragmentImage, (int) g_Fragments[n].x, (int) g_Fragments[n].y, g_Fragments[n].width, g_Fragments[n].height, 0, 1, g_Fragments[n].rotation, g_Fragments[n].scaling, g_Fragments[n].color);
        }

        y = g_GameHeight - 12;
        D3DCOLOR color = D3DCOLOR_ARGB(200, 255, 255, 255);
        D3DCOLOR debugcolor = D3DCOLOR_ARGB(255, 255, 255, 255);

        // draw the gui elements
        FontPrint(g_Font, 900, 0, L"Score", color);

        D3DCOLOR energycolor = D3DCOLOR_ARGB(200, 255, 255, 255);
        for (int n = 0; n < g_Energy * 5; n++)
            Sprite_Transform_Draw(g_EnergySlice, 10 + n * 2, 0, 1, 32, 0, 1, 0.0f, 1.0f, 1.0f, energycolor);

        D3DCOLOR healthcolor = D3DCOLOR_ARGB(200, 255, 255, 255);
        for (int n = 0; n < g_Health * 5; n++)
            Sprite_Transform_Draw(g_HealthSlice, 10 + n * 2, g_GameHeight - 21, 1, 20, 0, 1, 0.0f, 1.0f, 1.0f, healthcolor);

        // draw debug messages
        FontPrint(g_DebugFont, 0, y, L"", debugcolor);
        FontPrint(g_DebugFont, 0, y - 12, L"Core FPS = " + ToString(g_CoreFPS) + L" (" + ToString(1000.0 / g_CoreFPS) + L" ms)", debugcolor);
        FontPrint(g_DebugFont, 0, y - 24, L"Screen FPS = " + ToString(g_ScreenFPS), debugcolor);
        FontPrint(g_DebugFont, 0, y - 36, L"Ship X, Y = " + ToString(g_Player.x) + L", " + ToString(g_Player.y), debugcolor);
        FontPrint(g_DebugFont, 0, y - 48, L"Bullets = " + ToString(g_BulletCount));
        FontPrint(g_DebugFont, 0, y - 60, L"Buffer Scroll = " + ToString(g_ScrollX), debugcolor);
        FontPrint(g_DebugFont, 0, y - 72, L"Virtual Scroll = " + ToString(g_VirtualScrollX) + L" / " + ToString(g_VirtualLevelSize));
        FontPrint(g_DebugFont, 0, y - 84, L"Fragment[0] = " + ToString(g_Fragments[0].x) + L", " + ToString(g_Fragments[0].y));

        g_SpriteObj->End();

        g_D3DDev->EndScene();
        g_D3DDev->Present(NULL, NULL, NULL, NULL);
    }
}

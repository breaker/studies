//////////////////////////////////////////////////////////////
// Our Application Framework                                //
// Written by: C. Granberg, 2007                            //
//////////////////////////////////////////////////////////////

#include <windows.h>
#include <d3dx9.h>
#include <fstream>
#include "skinnedMesh.h"

using namespace std;

//Global variables
IDirect3DDevice9*   g_pDevice = NULL;
ID3DXFont*          g_pFont = NULL;
ID3DXEffect*        g_pEffect = NULL;
ofstream            g_debug("debug.txt");

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

bool KeyDown(int vk_code) {
    return (GetAsyncKeyState(vk_code) & 0x8000) ? true : false;
}
bool KeyUp(int vk_code) {
    return (GetAsyncKeyState(vk_code) & 0x8000) ? false : true;
}

class Application {
public:
    Application();
    ~Application();
    HRESULT Init(HINSTANCE hInstance, bool windowed);
    void Update(float deltaTime);
    void Render();
    void Cleanup();
    void Quit();

    void DeviceLost();
    void DeviceGained();

private:
    HWND m_mainWindow;
    D3DPRESENT_PARAMETERS m_present;
    bool m_deviceLost;

    SkinnedMesh m_drone;
    vector<string> m_animations;
    int m_activeAnimation;
    float m_time;
};

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch( msg ) {
    case WM_CREATE:
        break;

    case WM_DESTROY:
        ::PostQuitMessage(0);
        break;
    }
    return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) {
    Application app;

    if (FAILED(app.Init(hInstance, true)))
        return 0;

    MSG msg;
    memset(&msg, 0, sizeof(MSG));
    DWORD startTime = GetTickCount();

    while (msg.message != WM_QUIT) {
        if (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
        else {
            DWORD t = GetTickCount();
            float deltaTime = (t - startTime) * 0.001f;

            app.Update(deltaTime);
            app.Render();

            startTime = t;
        }
    }

    app.Cleanup();

    return (int)msg.wParam;
}

Application::Application() {
    m_time = 0.0f;
}

Application::~Application() {
    if (g_debug.good()) {
        g_debug.close();
    }
}

HRESULT Application::Init(HINSTANCE hInstance, bool windowed) {
    g_debug << "Application Started \n";

    //Create Window Class
    WNDCLASS wc;
    memset(&wc, 0, sizeof(WNDCLASS));
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = (WNDPROC)WndProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = "D3DWND";

    RECT rc = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);

    //Register Class and Create new Window
    RegisterClass(&wc);
    m_mainWindow = ::CreateWindow("D3DWND", "Character Animation with Direct3D: Example 4.2", WS_OVERLAPPEDWINDOW, 0, 0, rc.right - rc.left, rc.bottom - rc.top, 0, 0, hInstance, 0);
    SetCursor(NULL);
    ::ShowWindow(m_mainWindow, SW_SHOW);
    ::UpdateWindow(m_mainWindow);

    //Create IDirect3D9 Interface
    IDirect3D9* d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

    if (d3d9 == NULL) {
        g_debug << "Direct3DCreate9() - FAILED \n";
        return E_FAIL;
    }

    //Check that the Device supports what we need from it
    D3DCAPS9 caps;
    d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

    //Check vertex & pixelshader versions
    if (caps.VertexShaderVersion < D3DVS_VERSION(2, 0) || caps.PixelShaderVersion < D3DPS_VERSION(2, 0)) {
        g_debug << "Warning - Your graphic card does not support vertex and pixelshaders version 2.0 \n";
    }

    //Set D3DPRESENT_PARAMETERS
    m_present.BackBufferWidth            = WINDOW_WIDTH;
    m_present.BackBufferHeight           = WINDOW_HEIGHT;
    m_present.BackBufferFormat           = D3DFMT_A8R8G8B8;
    m_present.BackBufferCount            = 2;
    m_present.MultiSampleType            = D3DMULTISAMPLE_NONE;
    m_present.MultiSampleQuality         = 0;
    m_present.SwapEffect                 = D3DSWAPEFFECT_DISCARD;
    m_present.hDeviceWindow              = m_mainWindow;
    m_present.Windowed                   = windowed;
    m_present.EnableAutoDepthStencil     = true;
    m_present.AutoDepthStencilFormat     = D3DFMT_D24S8;
    m_present.Flags                      = 0;
    m_present.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
    m_present.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;

    //Hardware Vertex Processing
    int vp = 0;
    if ( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
        vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    else vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

    //Create the IDirect3DDevice9
    if (FAILED(d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_mainWindow, vp, &m_present, &g_pDevice))) {
        g_debug << "Failed to create IDirect3DDevice9 \n";
        return E_FAIL;
    }

    //Release IDirect3D9 interface
    d3d9->Release();

    //Load Application Specific resources here...
    D3DXCreateFont(g_pDevice, 20, 0, FW_BOLD, 1, false,
                   DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY,
                   DEFAULT_PITCH | FF_DONTCARE, "Arial", &g_pFont);

    //Load Effect
    ID3DXBuffer *pErrorMsgs = NULL;
    HRESULT hRes = D3DXCreateEffectFromFile(g_pDevice, "resources/fx/lighting.fx", NULL, NULL, D3DXSHADER_DEBUG, NULL, &g_pEffect, &pErrorMsgs);

    if (FAILED(hRes) && (pErrorMsgs != NULL)) {      //Failed to create Effect
        g_debug << (char*)pErrorMsgs->GetBufferPointer() << "\n";
        return E_FAIL;
    }

    m_deviceLost = false;

    m_drone.Load("resources/meshes/soldier.x");
    m_drone.GetAnimations(m_animations);
    m_activeAnimation = 0;
    m_drone.SetAnimation(m_animations[m_activeAnimation]);

    return S_OK;
}

void Application::DeviceLost() {
    try {
        g_pFont->OnLostDevice();
        g_pEffect->OnLostDevice();
        m_deviceLost = true;
    }
    catch(...) {
        g_debug << "Error occured in Application::DeviceLost() \n";
    }
}

void Application::DeviceGained() {
    try {
        g_pDevice->Reset(&m_present);
        g_pFont->OnResetDevice();
        g_pEffect->OnResetDevice();
        m_deviceLost = false;
    }
    catch(...) {
        g_debug << "Error occured in Application::DeviceGained() \n";
    }
}

void Application::Update(float deltaTime) {
    try {
        //Check for lost device
        HRESULT coop = g_pDevice->TestCooperativeLevel();

        if (coop != D3D_OK) {
            if (coop == D3DERR_DEVICELOST) {
                if (m_deviceLost == false)
                    DeviceLost();
            }
            else if (coop == D3DERR_DEVICENOTRESET) {
                if (m_deviceLost == true)
                    DeviceGained();
            }

            Sleep(100);
            return;
        }

        m_time += deltaTime * 0.4f;
        D3DXMATRIX world;
        D3DXMatrixRotationY(&world, m_time);
        m_drone.SetPose(world, deltaTime * 0.5f);

        //Keyboard input
        if (KeyDown(VK_ESCAPE)) {
            Quit();
        }

        if (KeyDown(VK_RETURN) && KeyDown(18)) {     //ALT + RETURN
            //Switch between windowed mode and fullscreen mode
            m_present.Windowed = !m_present.Windowed;

            DeviceLost();
            DeviceGained();

            if (m_present.Windowed) {
                RECT rc = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
                AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);
                SetWindowPos(m_mainWindow, HWND_NOTOPMOST, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_SHOWWINDOW);
                UpdateWindow(m_mainWindow);
            }
        }

        //Toggle Animation
        if (KeyDown(VK_RETURN)) {
            Sleep(300);
            m_activeAnimation = (m_activeAnimation + 1) % (int)m_animations.size();
            m_drone.SetAnimation(m_animations[m_activeAnimation]);
        }
    }
    catch(...) {
        g_debug << "Error in Application::Update() \n";
    }
}

void Application::Render() {
    if (!m_deviceLost) {
        try {
            //Create Matrices
            D3DXMATRIX identity, world, view, proj, shadow;
            D3DXMatrixIdentity(&identity);
            //D3DXMatrixRotationY(&world, m_time);
            D3DXMatrixLookAtLH(&view, &D3DXVECTOR3(cos(m_time) * 3.0f, 1.5f, sin(m_time) * 3.0f), &D3DXVECTOR3(0.0f, 1.0f, 0.0f), &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
            D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI / 4.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 1.0f, 1000.0f);
            D3DXPLANE ground(0.0f, 1.0f, 0.0f, 0.0f);
            D3DXVECTOR4 lightPos(-50.0f, 75.0f, 50.0f, 0.0f);
            D3DXMatrixShadow(&shadow, &lightPos, &ground);

            g_pDevice->SetTransform(D3DTS_WORLD, &identity);
            g_pDevice->SetTransform(D3DTS_VIEW, &view);
            g_pDevice->SetTransform(D3DTS_PROJECTION, &proj);

            // Clear the viewport
            g_pDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0L);

            // Begin the scene
            if (SUCCEEDED(g_pDevice->BeginScene())) {
                //Render Drone
                {
                    g_pEffect->SetMatrix("matW", &identity);
                    g_pEffect->SetMatrix("matVP", &(view * proj));
                    g_pEffect->SetVector("lightPos", &lightPos);

                    m_drone.Render(NULL);
                }

                if (KeyDown(VK_SPACE)) {
                    g_pDevice->Clear(0L, NULL, D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0L);
                    m_drone.RenderSkeleton(NULL, NULL, identity);
                }

                RECT rc = {10, 10, 0, 0};
                g_pFont->DrawText(NULL, "Press Space to Toggle Skeleton", -1, &rc, DT_LEFT | DT_TOP | DT_NOCLIP, 0x66000000);

                RECT rc2 = {10, 30, 0, 0};
                string s = "Press Return to toggle animation: ";
                s += m_animations[m_activeAnimation];
                g_pFont->DrawText(NULL, s.c_str(), -1, &rc2, DT_LEFT | DT_TOP | DT_NOCLIP, 0x66000000);


                // End the scene.
                g_pDevice->EndScene();
                g_pDevice->Present(0, 0, 0, 0);
            }
        }
        catch(...) {
            g_debug << "Error in Application::Render() \n";
        }
    }
}

void Application::Cleanup() {
    //Release all resources here...
    if (g_pFont != NULL)     g_pFont->Release();
    if (g_pDevice != NULL)   g_pDevice->Release();
    g_debug << "Application Terminated \n";
}

void Application::Quit() {
    ::DestroyWindow(m_mainWindow);
    ::PostQuitMessage(0);
}

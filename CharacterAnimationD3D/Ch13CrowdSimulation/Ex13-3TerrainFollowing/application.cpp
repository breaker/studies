//////////////////////////////////////////////////////////////
// Our Application Framework                                //
// Written by: C. Granberg, 2007                            //
//////////////////////////////////////////////////////////////

#include <windows.h>
#include <d3dx9.h>
#include <fstream>
#include "crowd.h"

using namespace std;

//Global Variables
IDirect3DDevice9*   g_pDevice = NULL;
ID3DXSprite*        g_pSprite = NULL;
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

    Crowd* m_pCrowd;
};

//The Window procedure function
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    //User specified events
    switch( msg ) {
    case WM_CREATE:
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    //Default events
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

//WinMain - The application entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) {
    //Create a new Application object
    Application app;

    //Initialize it
    if (FAILED(app.Init(hInstance, true)))
        return 0;

    //Start the windows message loop
    MSG msg;
    memset(&msg, 0, sizeof(MSG));

    //Keep track of the time
    DWORD startTime = GetTickCount();

    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            //If there's a message, deal with it and send it onwards
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {  //Otherwise update the game
            //Calculate the delta time
            DWORD t  = GetTickCount();
            float deltaTime = (t - startTime)*0.001f;

            //Update the application
            app.Update(deltaTime);

            //Render the application
            app.Render();

            startTime = t;
        }
    }

    //Release all resources
    app.Cleanup();

    //... and Quit!
    return (int)msg.wParam;
}

Application::Application() {
    m_pCrowd = NULL;
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

    m_mainWindow = CreateWindow("D3DWND",   //Window class to use
                                "Character Animation with Direct3D: Example 13.3", //Title
                                WS_OVERLAPPEDWINDOW,    //Style
                                0,                      //X
                                0,                      //Y
                                rc.right - rc.left,     //Width
                                rc.bottom - rc.top,     //Height
                                NULL,                   //Parent Window
                                NULL,                   //Menu
                                hInstance,              //Application Instance
                                0);                     //Param

    SetCursor(NULL);
    ShowWindow(m_mainWindow, SW_SHOW);
    UpdateWindow(m_mainWindow);

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

    //Create Sprite
    D3DXCreateSprite(g_pDevice, &g_pSprite);

    //Load Effect
    ID3DXBuffer *pErrorMsgs = NULL;
    HRESULT hRes = D3DXCreateEffectFromFile(g_pDevice, "resources/fx/lighting.fx", NULL, NULL, D3DXSHADER_DEBUG, NULL, &g_pEffect, &pErrorMsgs);

    if (FAILED(hRes) && (pErrorMsgs != NULL)) {      //Failed to create Effect
        MessageBox(NULL, (char*)pErrorMsgs->GetBufferPointer(), "Effect Error", MB_OK);
        return E_FAIL;
    }

    m_deviceLost = false;

    //Create crowd
    m_pCrowd = new Crowd(40);

    return S_OK;
}

void Application::DeviceLost() {
    //Device lost happens at ALT + TAB and similar events
    try {
        g_pFont->OnLostDevice();
        g_pSprite->OnLostDevice();
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
        g_pSprite->OnResetDevice();
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

        //Update crowd
        m_pCrowd->Update(deltaTime);

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
    }
    catch(...) {
        g_debug << "Error in Application::Update() \n";
    }
}

void Application::Render() {
    if (!m_deviceLost) {
        try {
            D3DXMATRIX identity, shadow;
            D3DXMatrixIdentity(&identity);

            //Set ground plane + light position
            D3DXPLANE ground(0.0f, 1.0f, 0.0f, 0.0f);
            D3DXVECTOR4 lightPos(-20.0f, 75.0f, -120.0f, 0.0f);

            //Create the shadow matrix
            D3DXMatrixShadow(&shadow, &lightPos, &ground);

            //Create Transformation Matrices
            D3DXMATRIX view, proj, world;
            D3DXMatrixIdentity(&world);
            D3DXMatrixLookAtLH(&view, &D3DXVECTOR3(0.0f, 5.0f, -20.0f), &D3DXVECTOR3(0.0f, 0.0f, 0.0f), &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
            D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI / 4.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

            //Set transformation matrices
            g_pDevice->SetTransform(D3DTS_WORLD, &world);
            g_pDevice->SetTransform(D3DTS_VIEW, &view);
            g_pDevice->SetTransform(D3DTS_PROJECTION, &proj);

            g_pEffect->SetMatrix("matVP", &(view * proj));
            g_pEffect->SetVector("lightPos", &lightPos);

            // Clear the viewport
            g_pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);

            // Begin the scene
            if (SUCCEEDED(g_pDevice->BeginScene())) {
                m_pCrowd->Render();

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
    if (g_pSprite != NULL)   g_pSprite->Release();
    if (g_pFont != NULL)     g_pFont->Release();
    if (g_pDevice != NULL)   g_pDevice->Release();
    if (g_pEffect != NULL)   g_pEffect->Release();

    g_debug << "Application Terminated \n";
}

void Application::Quit() {
    DestroyWindow(m_mainWindow);
    PostQuitMessage(0);
}
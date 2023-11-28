// MyEngineFrm.h
//

#pragma once

// Globals

extern HWND gHwnd;
extern HINSTANCE gHInstApp;
extern BOOL gIdleInBackground;
extern BOOL gQuitFlag;

// Functions

void AdjustWindowed(HWND hwnd, DWORD style, DWORD exStyle, int width, int height, HMENU hMenu);
void CreateAppWindow(const char* title, BOOL windowed, int width, int height, HMENU hMenu);
void DestroyAppWindow();

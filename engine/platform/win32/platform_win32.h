#ifndef PLATFORM_WIN32_H
#define PLATFORM_WIN32_H

typedef struct win32_wnd win32_wnd;
struct win32_wnd
{
    HWND handle;
    HDC  device_ctx;
};

typedef struct win32_gfx_state win32_gfx_state;
struct win32_gfx_state
{
    HINSTANCE instance;
};

global win32_gfx_state g_win32_gfx_state;

internal LRESULT CALLBACK win32_wnd_msg_callback(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param);

#endif // PLATFORM_WIN32_H

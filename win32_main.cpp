#include <windows.h>
#include "init_vulkan.h"

bool running;

LRESULT CALLBACK win32_callback
(HWND	window,
UINT	message,
WPARAM	wParam,
LPARAM	lParam)
{
	LRESULT result = 0;
	switch(message)
	{
		case WM_DESTROY:
		{
			running = false;
			OutputDebugStringA("WM_DESTROY\n");
		} break;

		case WM_CLOSE:
		{
			running = false;
			OutputDebugStringA("WM_CLOSE\n");
		} break;

		case WM_ACTIVATEAPP:
		{
			OutputDebugStringA("WM_ACTIVATEAPP\n");
		} break;

		case WM_KEYDOWN:
		{
			uint32_t vkCode = wParam;
			bool wasDown = ((lParam & (1 << 30)) != 0);
			bool isDown = ((lParam & (1 << 31)) == 0);
			if(isDown)
			{
				if(vkCode == 'W')
				{
					OutputDebugStringA("WWWWWWWWWWA\n");
				}
			}
		} break;

		default:
		{
			result = DefWindowProc(window, message, wParam, lParam);
		} break;
	}
	return result;
}

int CALLBACK WinMain
(HINSTANCE	instance,
HINSTANCE	prevInstance,
LPSTR		commandline,
int			showCode)
{
	WNDCLASSA win32_class = {};
	win32_class.lpfnWndProc = win32_callback;
	win32_class.hInstance = instance;
	win32_class.lpszClassName = "WINDOWS";

	if(RegisterClassA(&win32_class))
	{
		HWND window_handle = CreateWindowExA(0, win32_class.lpszClassName, "eeeeeeeeeee",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
		WIDTH, HEIGHT, 0, 0, instance, 0);
		if(window_handle)
		{
			running = true;

			init_vulkan(window_handle);
			while(running)
			{
				MSG message;
				while(PeekMessage(&message, 0, 0, 0, PM_REMOVE))
				{
					if(message.message == WM_QUIT)
					{
						running = false;
					}
					TranslateMessage(&message);
					DispatchMessageA(&message);
				}
				draw_frame();
			}
		}
		else
		{
			OutputDebugStringA("Failed to create window handle.");
		}
	}
	else
	{
		OutputDebugStringA("Failed to register window class.");
	}

	return 0;
}
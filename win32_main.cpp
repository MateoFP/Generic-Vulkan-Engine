#include <windows.h>
#include "game.h"

bool running;

LRESULT CALLBACK Win32Callback
(HWND	window,
 UINT	message,
 WPARAM	wParam,
 LPARAM	lParam)
{
	LRESULT result = 0;
	switch(message)
	{
		case WM_QUIT:
		{
			running = false;
		} break;
		case WM_CLOSE:
		{
			running = false;
		} break;
		case WM_DESTROY:
		{
			running = false;
		} break;
		case WM_ACTIVATEAPP:
		{
			OutputDebugStringA("WM_ACTIVATEAPP\n");
		} break;
		default:
		{
			result = DefWindowProc(window, message, wParam, lParam);
		} break;
	}
	return result;
}

void Win32MessageLoop(GameInput *input, Character *player, HWND window)
{
	MSG message;
	while(PeekMessage(&message, 0, 0, 0, PM_REMOVE))
	{
		switch(message.message)
		{
			case WM_KEYUP:
			{
				uint32_t vkCode = (uint32_t)message.wParam;
				bool wasDown = ((message.lParam & (1 << 30)) != 0);
				bool isDown = ((message.lParam & (1 << 31)) == 0);
				if(wasDown != isDown)
				{
				}

				if(vkCode == ' ')
				{
					input->key_spacebar.ended_down = false;
				}
				bool altKeyWasDown = (message.lParam & (1 << 29));
				if((vkCode == VK_F4) && altKeyWasDown)
				{
					running = false;
				}
			} break;
			case WM_KEYDOWN:
			{
				uint32_t vkCode = (uint32_t)message.wParam;
				if(vkCode == 'S')
				{
					player->moving = false;
				}
				if(vkCode == ' ')
				{
					 input->key_spacebar.ended_down = true;
				}
			} break;
			case WM_MOUSEMOVE:
			{	
				POINT mouse_pos;
				GetCursorPos(&mouse_pos);
				ScreenToClient(window, &mouse_pos);
				input->mouse_x = mouse_pos.x;
				input->mouse_y = mouse_pos.y;

				RECT rect = {};
				GetWindowRect(window, &rect);
				rect.left   = rect.left    +30;
				rect.top    = rect.top     +50;
				rect.right  = rect.right   -30;
				rect.bottom = rect.bottom  -30;
				ClipCursor(&rect);
			} break;	
			case WM_RBUTTONDOWN:
			{
				input->MouseRay.direction = RayCast(input->mouse_x, input->mouse_y, 
											gubo.view, gubo.proj, 
											(float)extent.width, (float)extent.height);

				input->MouseRayCollision = GetRayCollisionBox(input->MouseRay, world_box);

				player->destination		= input->MouseRayCollision.point;
				player->movement		= player->position;
				player->movement.xyz[0]	= player->destination.xyz[0] - player->movement.xyz[0];
				player->movement.xyz[2] = player->destination.xyz[2] - player->movement.xyz[2];
				player->movement		= v3_normalize(player->movement);
				player->movement.xyz[0] *= player->speed;
				player->movement.xyz[2] *= player->speed;
				float x_dist = player->destination.xyz[0] - player->position.xyz[0];
				float z_dist = player->destination.xyz[2] - player->position.xyz[2];
				player->angle = atan2(x_dist, z_dist);
				player->moving = true;
			} break;
			case WM_MOUSEWHEEL:
			{
				int32_t wheel_pos = (int32_t)message.wParam;
				if(wheel_pos < 0)
				{
					cam.position.xyz[1] += 20;
				}
				else
				{
					cam.position.xyz[1] -= 20;
				}
			} break;
			default:
			{
				TranslateMessage(&message);
				DispatchMessageA(&message);
			} break;
		}
	}
}

int CALLBACK WinMain
(HINSTANCE	instance,
HINSTANCE	prevInstance,
LPSTR		commandline,
int			showCode)
{
	LARGE_INTEGER PerfCounterFrequencyResult;
	QueryPerformanceFrequency(&PerfCounterFrequencyResult);
	int64_t PerfCountFrequency = PerfCounterFrequencyResult.QuadPart;
	LARGE_INTEGER LastCounter;
	QueryPerformanceCounter(&LastCounter);
	int64_t LastCycleCount = __rdtsc();

	WNDCLASSEXW win32_class = {};
	win32_class.lpfnWndProc = Win32Callback;
	win32_class.hInstance = instance;
	win32_class.lpszClassName = L"win32_class";
	win32_class.hCursor = LoadCursor(0, IDC_HAND);
	win32_class.cbSize = sizeof(WNDCLASSEXA);

	if(RegisterClassExW(&win32_class))
	{
		HWND window_handle = CreateWindowExW(0,
							win32_class.lpszClassName, L"League Of Legends 2",
							WS_TILED | WS_CAPTION | WS_SYSMENU |
							WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VISIBLE , 
							135, 85, WIDTH, HEIGHT, 0, 0, instance, 0);

		if(window_handle)
		{
			GameInput input = {};
			Character player = {};
			init_vulkan(window_handle);
			init_player(&input, &player);
			running = true;

			while(running)
			{
				Win32MessageLoop(&input, &player, window_handle);
				GameUpdate(&player, &input);

				LARGE_INTEGER EndCounter;
				QueryPerformanceCounter(&EndCounter);
				int64_t EndCycleCount = __rdtsc();
				int64_t CyclesElapsed = EndCycleCount - LastCycleCount;
				int64_t CounterElapsed = EndCounter.QuadPart - LastCounter.QuadPart;	
				float MSPerFrame = ((1000.0f * (float)CounterElapsed) / (float)PerfCountFrequency);
				float FPS = (float)PerfCountFrequency / (float)CounterElapsed;
				float MegaCyclesElapsedPerFrame = (float)(CyclesElapsed / (1000.0f * 1000.0f));
				//char Buffer[256];
				//sprintf(Buffer, "%.02f ms/f, %.02f f/s, %.02f mc/f \n", MSPerFrame, FPS, MegaCyclesElapsedPerFrame);
				//OutputDebugStringA(Buffer);
				LastCounter = EndCounter;
				LastCycleCount = EndCycleCount;
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
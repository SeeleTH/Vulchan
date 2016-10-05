#if defined(_DEBUG)
#include <stdio.h>
#include <stdlib.h>
#endif
#include <Windows.h>
#include "waifu\wfassert.h"
#include "waifu\wfvulkan.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) {
	case WM_CLOSE: {
		PostQuitMessage(0);
		break;
	}
	default: {
		break;
	}
	}

	// a pass-through for now. We will return to this callback
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
#if defined(_DEBUG)
	{
		AllocConsole();
		AttachConsole(GetCurrentProcessId());
		FILE* fileTemp;
		freopen_s(&fileTemp, "CONOUT$", "w", stdout);
	}
#endif

	WNDCLASSEX windowClass = {};
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = hInstance;
	windowClass.lpszClassName = "VulkanWindowClass";
	RegisterClassEx(&windowClass);

	HWND windowHandle = CreateWindowEx(NULL, "VulkanWindowClass", "Vulchan",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		100,
		100,
		800, 
		600,
		NULL,
		NULL,
		hInstance,
		NULL);

	wfVulkan::VulkanBase vulkanBase;
	wfOS::WindowContext windowContext;
	windowContext.Handle = windowHandle;
	windowContext.Instance = hInstance;
	vulkanBase.PrepareVulkan(windowContext);

	MSG msg;
	bool done = false;
	while (!done) {
		PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE);
		if (msg.message == WM_QUIT) {
			done = true;
		}
		else {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		RedrawWindow(windowHandle, NULL, NULL, RDW_INTERNALPAINT);
	}

	return (int)msg.wParam;
}
#include <Windows.h>
#include <d3d11.h>


LRESULT CALLBACK WindowProc(
	HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);

		return 0;
	}
	return DefWindowProc(
		hwnd,
		message,
		wParam,
		lParam
	);
}
int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	const char CLASS_NAME[] = "MyWindow";

	WNDCLASSA wc = {};

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	wc.hbrBackground =
		CreateSolidBrush(RGB(50, 100, 200));

	RegisterClassA(&wc);

	int width = 1200;
	int height = 800;

	HWND hwnd = CreateWindowExA(
		0,
		CLASS_NAME,
		"My Engine",

		WS_OVERLAPPEDWINDOW,

		CW_USEDEFAULT,
		CW_USEDEFAULT,

		width,
		height,

		nullptr,
		nullptr,

		hInstance,
		nullptr
	);

	if (hwnd == nullptr)
	{
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);

	MSG msg = {};

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}

	return 0;
}
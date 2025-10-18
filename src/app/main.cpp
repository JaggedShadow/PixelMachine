#include <gpu/RenderContext.h>
#include <Windows.h>

namespace PXMGPU = PixelMachine::GPU;

HWND CreateSystemWindow(const wchar_t *name , const int windowWidth, const int windowHeight) {

	HINSTANCE hInstance = GetModuleHandle(nullptr);
	WNDCLASSEXW wndClassExW = { 0u };
	const wchar_t windowClassName[] = L"DefaultWindowClass";

	wndClassExW.lpszClassName = windowClassName;
	wndClassExW.lpfnWndProc = DefWindowProcW;
	wndClassExW.hInstance = hInstance;
	wndClassExW.hCursor = NULL;
	wndClassExW.hIcon = NULL;
	wndClassExW.hbrBackground = (HBRUSH)(COLOR_BTNTEXT);
	wndClassExW.lpszMenuName = NULL;
	wndClassExW.hIconSm = NULL;
	wndClassExW.style = NULL;
	wndClassExW.cbClsExtra = NULL;
	wndClassExW.cbWndExtra = NULL;
	wndClassExW.cbSize = sizeof(wndClassExW);

	RegisterClassExW(&wndClassExW);

	RECT clientArea = { 0 };
	clientArea.top = 0u;
	clientArea.left = 0u;
	clientArea.bottom = windowHeight;
	clientArea.right = windowWidth;

	DWORD windowStyle = WS_SYSMENU;

	AdjustWindowRectEx(&clientArea, windowStyle, TRUE, WS_EX_OVERLAPPEDWINDOW);

	HWND systemWindowHandle = CreateWindowExW(
		WS_EX_OVERLAPPEDWINDOW,
		windowClassName,
		name,
		windowStyle,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowWidth,
		windowHeight,
		NULL,
		NULL,
		hInstance,
		NULL);

	return systemWindowHandle;
}

int main() {

	HWND windowHandle = CreateSystemWindow(L"PixelMachine", 1280, 720);
	ShowWindow(windowHandle, SW_SHOW);

	PXMGPU::RenderContext::Initialize(windowHandle);
	PXMGPU::RenderContext *pContext = PXMGPU::RenderContext::Get();

	//pContext->BeginPass("TrianglePass");
	//pContext->BindShader("Vertex Shader")
	//pContext->BindShader("Fragment Shader")
	//pContext->SetPrimitiveType("TRIANGLE_LIST")
	//pContext->SetMultisampling(8)
	//pContext->SetClearColor(0,0,0)
	//pContext->EndPass()


	while (true) {
		MSG msg = { 0 };
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//pContext->Draw()
		//pContext->PresentFrame();
	}

	PXMGPU::RenderContext::Destroy();

	return 0;
}
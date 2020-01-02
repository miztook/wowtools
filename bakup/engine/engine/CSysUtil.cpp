#include "CSysUtil.h"

#include "CInputReader.h"
#include "Engine.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (g_Engine)
	{
		IMessageHandler* handler = g_Engine->getMessageHandler();
		if (handler)
		{
			switch (message)
			{
			case WM_QUIT:
			case WM_CLOSE:
				handler->onExit(hWnd);
				break;
			case WM_SIZE:
				handler->onSize(hWnd, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam));
				break;
			default:
				break;
			}
		}

		CInputReader* inputReader = g_Engine->getInputReader();
		if (inputReader)
			inputReader->filterWindowProc(hWnd, message, wParam, lParam);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

bool CSysUtil::isKeyPressed(uint8_t vk)
{
	return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

bool CSysUtil::getCursorPosition(vector2di & pos)
{
	POINT pt;
	if (::GetCursorPos(&pt))
	{
		pos.x = (int32_t)pt.x;
		pos.y = (int32_t)pt.y;
		return true;
	}
	return false;
}

SWindowInfo CSysUtil::createWindow(const char * caption, uint32_t width, uint32_t height, bool fullscreen, bool hide)
{
	if (fullscreen)
		hide = false;

	HINSTANCE hInstance = ::GetModuleHandle(nullptr);
	HWND hwnd;

	const char* className = "mywowWindow";

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = nullptr;
	wcex.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = className;
	wcex.hIconSm = 0;
	wcex.hIcon = ::LoadIcon(nullptr, IDI_APPLICATION);

	::RegisterClassEx(&wcex);

	RECT clientSize;
	clientSize.top = 0;
	clientSize.left = 0;
	clientSize.right = (LONG)width;
	clientSize.bottom = (LONG)height;

	DWORD style = WS_POPUP;

	if (!fullscreen && !hide)
		style = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

	if (hide)
	{
		style &= (~WS_VISIBLE);
	}

	AdjustWindowRect(&clientSize, style, FALSE);

	int32_t clientWidth = clientSize.right - clientSize.left;
	int32_t clientHeight = clientSize.bottom - clientSize.top;

	int32_t windowLeft = (GetSystemMetrics(SM_CXSCREEN) - clientWidth) / 2;
	int32_t windowTop = (GetSystemMetrics(SM_CYSCREEN) - clientHeight) / 2;

	if (windowLeft < 0)
		windowLeft = 0;
	if (windowTop < 0)
		windowTop = 0;	// make sure window menus are in screen on creation

	hwnd = ::CreateWindow(className, caption, style, windowLeft, windowTop,
		clientWidth, clientHeight, nullptr, nullptr, hInstance, nullptr);

	::ShowWindow(hwnd, hide ? SW_HIDE : SW_SHOW);
	::UpdateWindow(hwnd);

	::MoveWindow(hwnd, windowLeft, windowTop, clientWidth, clientHeight, TRUE);

	if (!hide)
	{
		::SetActiveWindow(hwnd);
		::SetForegroundWindow(hwnd);
	}

	SWindowInfo windowInfo;
	windowInfo.width = (uint32_t)width;
	windowInfo.height = (uint32_t)height;
	windowInfo.hwnd = hwnd;

	return windowInfo;
}

void CSysUtil::outputDebug(const char * format, ...)
{
	char str[2048];

	va_list va;
	va_start(va, format);
	Q_vsprintf(str, 2048, format, va);
	va_end(va);

	Q_strcat(str, 2048, "\n");
	OutputDebugStringA(str);
}

void CSysUtil::exitProcess(int ret)
{
	::ExitProcess(ret);
}

#include "Engine/Renderer/Window.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/EventSystem.hpp"
#define  WIN_LEAN_AND_MEAN
#include <windows.h>
#include <Engine/Core/ErrorWarningAssert.hpp>

//-----------------------------------------------------------------------------------------------
Window* Window::s_mainWindow = nullptr;

//-----------------------------------------------------------------------------------------------
// Handles Windows (Win32) messages/events; i.e. the OS is trying to tell us something happened.
// This function is called back by Windows whenever we tell it to (by calling DispatchMessage).
//
// #SD1ToDo: We will move this function to a more appropriate place (Engine/Renderer/Window.cpp) later on...
//
LRESULT CALLBACK WindowsMessageHandlingProcedure(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam)
{
	//Get "THE" window (assume for now we only have one)
	Window* window = Window::GetMainWindowInstance();
	GUARANTEE_OR_DIE( window != nullptr, "Window was null!");

	//Ask the Window for a pointer to the InputSystem it was created with ( in its InputSystemConfig)
	//InputSystem* input = window->GetConfig().m_inputSystem;
	//GUARANTEE_OR_DIE( input != nullptr, "Window's InputSystem pointer was null!");

	//Respond accordingly based on whatever message Windows has left for us (key pressed/released, close/X clicked, lost focus. etc.)
	switch (wmMessageCode)
	{
		case WM_CHAR:
		{
			EventArgs args;
			args.SetValue("char", Stringf("%d", (unsigned char) wParam));
			FireEvent("CharInput", args);
			return 0;
		}
		// App close requested via "X" button, or right-click "Close Window" on task bar, or "Close" from system menu, or Alt-F4
		case WM_CLOSE:
		{
			EventArgs args;
			FireEvent("quit", args);
			return 0; // "Consumes" this message (tells Windows "okay, we handled it")
		}

		// Raw physical keyboard "key-was-just-depressed" event (case-insensitive, not translated)
		case WM_KEYDOWN:
		{
			EventArgs args;
			args.SetValue("KeyCode", Stringf("%d", (unsigned char) wParam));
			FireEvent("KeyPressed", args);
			return 0;
		}

		// Raw physical keyboard "key-was-just-released" event (case-insensitive, not translated)
		case WM_KEYUP:
		{
			EventArgs args;
			args.SetValue("KeyCode", Stringf("%d", (unsigned char)wParam));
			FireEvent("KeyReleased", args);
			return 0;
		}
		// Treat this special mouse-button windows message as if it were an ordinary key down for us:
		case WM_LBUTTONDOWN:
		{
			EventArgs args;
			args.SetValue("KeyCode", Stringf("%d", (unsigned char)KEYCODE_LEFT_MOUSE));
			FireEvent("KeyPressed", args);
			return 0;
		}
		case WM_LBUTTONUP:
		{
			EventArgs args;
			args.SetValue("KeyCode", Stringf("%d", (unsigned char)KEYCODE_LEFT_MOUSE));
			FireEvent("KeyReleased", args);
			return 0;
		}
		case WM_RBUTTONDOWN:
		{
			EventArgs args;
			args.SetValue("KeyCode", Stringf("%d", (unsigned char)KEYCODE_RIGHT_MOUSE));
			FireEvent("KeyPressed", args);
			return 0;
		}
		case WM_RBUTTONUP:
		{
			EventArgs args;
			args.SetValue("KeyCode", Stringf("%d", (unsigned char)KEYCODE_RIGHT_MOUSE));
			FireEvent("KeyReleased", args);
			return 0;
		}
	}
	// Send back to Windows any unhandled/unconsumed messages we want other apps to see (e.g. play/pause in music apps, etc.)
	return DefWindowProc(windowHandle, wmMessageCode, wParam, lParam);
}

Window::Window(WindowConfig const& config)
	:m_config( config )
{
	s_mainWindow = this;
}

Window::~Window()
{
}

void Window::Startup()
{
	CreatOSWindow();
}

void Window::BeginFrame()
{
	RunMessagePump();
}

void Window::EndFrame()
{
	// "Present" the backbuffer by swapping the front (visible) and back (working) screen buffers
	//SwapBuffers( reinterpret_cast<HDC>(m_dc) );
}

void Window::Shutdown()
{

}

Window* Window::GetMainWindowInstance()
{
	return s_mainWindow;
}

WindowConfig const& Window::GetConfig() const
{
	return m_config;
}

float Window::GetAspect() const
{
	return m_config.m_clientAspect;
}

void* Window::GetDC() const
{
	return m_dc;
}

void* Window::GetHwnd() const
{
	return m_hwnd;
}

void Window::CreatOSWindow()
{
	// Define a window style/class
	WNDCLASSEX windowClassDescription;
	memset(&windowClassDescription, 0, sizeof(windowClassDescription));
	windowClassDescription.cbSize = sizeof(windowClassDescription);
	windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast<WNDPROC>(WindowsMessageHandlingProcedure); // Register our Windows message-handling function
	windowClassDescription.hInstance = GetModuleHandle(NULL);
	windowClassDescription.hIcon = NULL;
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = TEXT("Simple Window Class");
	RegisterClassEx(&windowClassDescription);

	// #SD1ToDo: Add support for fullscreen mode (requires different window style flags than windowed mode)
	const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED;
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	// Get desktop rect, dimensions, aspect
	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect(desktopWindowHandle, &desktopRect);
	float desktopWidth = (float)(desktopRect.right - desktopRect.left);
	float desktopHeight = (float)(desktopRect.bottom - desktopRect.top);
	float desktopAspect = desktopWidth / desktopHeight;

	// Calculate maximum client size (as some % of desktop size)
	constexpr float maxClientFractionOfDesktop = 0.90f;
	float clientWidth = desktopWidth * maxClientFractionOfDesktop;
	float clientHeight = desktopHeight * maxClientFractionOfDesktop;
	if (m_config.m_clientAspect > desktopAspect)
	{
		// Client window has a wider aspect than desktop; shrink client height to match its width
		clientHeight = clientWidth / m_config.m_clientAspect;
	}
	else
	{
		// Client window has a taller aspect than desktop; shrink client width to match its height
		clientWidth = clientHeight * m_config.m_clientAspect;
	}

	// Calculate client rect bounds by centering the client area
	float clientMarginX = 0.5f * (desktopWidth - clientWidth);
	float clientMarginY = 0.5f * (desktopHeight - clientHeight);
	RECT clientRect;
	clientRect.left = (int)clientMarginX;
	clientRect.right = clientRect.left + (int)clientWidth;
	clientRect.top = (int)clientMarginY;
	clientRect.bottom = clientRect.top + (int)clientHeight;

	// Calculate the outer dimensions of the physical window, including frame et. al.
	RECT windowRect = clientRect;
	AdjustWindowRectEx(&windowRect, windowStyleFlags, FALSE, windowStyleExFlags);

	WCHAR windowTitle[1024];
	MultiByteToWideChar(GetACP(), 0, m_config.m_windowTitle.c_str(), -1, windowTitle, sizeof(windowTitle) / sizeof(windowTitle[0]));
	//---------------------------------------------------------------------------------
	HMODULE applicationInstanceHandle = GetModuleHandle(NULL);
	windowClassDescription.hInstance = applicationInstanceHandle;
	//---------------------------------------------------------------------------------
	m_hwnd = CreateWindowEx(
		windowStyleExFlags,
		windowClassDescription.lpszClassName,
		windowTitle,
		windowStyleFlags,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		(HINSTANCE)applicationInstanceHandle,
		NULL);

	ShowWindow((HWND)m_hwnd, SW_SHOW);
	SetForegroundWindow((HWND)m_hwnd);
	SetFocus((HWND)m_hwnd);

	m_dc = ::GetDC((HWND)m_hwnd);

	HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);
	SetCursor(cursor);
}

void Window::RunMessagePump()
{
	MSG queuedMessage;
	for (;; )
	{
		const BOOL wasMessagePresent = PeekMessage(&queuedMessage, NULL, 0, 0, PM_REMOVE);
		if (!wasMessagePresent)
		{
			break;
		}

		TranslateMessage(&queuedMessage);
		DispatchMessage(&queuedMessage); // This tells Windows to call our "WindowsMessageHandlingProcedure" (a.k.a. "WinProc") function
	}
}
//---------------------------------------------------------------------------------------
// Return the mouse cursor position in normalized(U,V) coordinates relative to the interior
// (client area) of our window. Reports numbers outside [0, 1] if the mouse cursor is outside
//---------------------------------------------------------------------------------------
//Vec2 Window::GetNormalizedCursorPos() const
//{
//	HWND windowHandle = HWND(m_hwnd);
//	POINT cursorCoords;
//	RECT clientRect;
//	::GetCursorPos(&cursorCoords);// in screen coordinate, (0,0) top-left
//	::ScreenToClient(windowHandle, &cursorCoords); // relative to the window interior
//	::GetClientRect(windowHandle, &clientRect);// size of window interior (0,0 to width, height)
//	float cursorX = float(cursorCoords.x) / float(clientRect.right);//normalized x position
//	float cursorY = float(cursorCoords.y) / float(clientRect.bottom);//normalized y position
//	return Vec2(cursorX, 1.f - cursorY);// We want (0,0) in the bottom-left
//}

IntVec2 Window::GetClientDimensions() {
	RECT clientRect;
	GetClientRect((HWND)m_hwnd, &clientRect); // Retrieve the dimensions of the client area for the specified window

	int width = clientRect.right - clientRect.left; // Calculate width
	int height = clientRect.bottom - clientRect.top; // Calculate height

	return IntVec2(width, height); // Return the dimensions as an IntVec2
}


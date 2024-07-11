#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/DevConsole.hpp"
#include <Windows.h> 

extern EventSystem* g_theEventSystem;
extern InputSystem* g_theInput;
extern Window* g_theWindow;

const unsigned char KEYCODE_F1            = VK_F1; // 112 == 0x70
const unsigned char KEYCODE_F2            = VK_F2;
const unsigned char KEYCODE_F3            = VK_F3;
const unsigned char KEYCODE_F4            = VK_F4;
const unsigned char KEYCODE_F5            = VK_F5;
const unsigned char KEYCODE_F6            = VK_F6;
const unsigned char KEYCODE_F7            = VK_F7;
const unsigned char KEYCODE_F8            = VK_F8;
const unsigned char KEYCODE_F9            = VK_F9;
const unsigned char KEYCODE_F10           = VK_F10;
const unsigned char KEYCODE_F11           = VK_F11;
const unsigned char KEYCODE_ESC           = VK_ESCAPE;
const unsigned char KEYCODE_SPACE         = VK_SPACE;
const unsigned char KEYCODE_ENTER         = VK_RETURN;
const unsigned char KEYCODE_BACKSPACE	  = VK_BACK;
const unsigned char KEYCODE_SHIFT		  = VK_SHIFT;
const unsigned char KEYCODE_INSERT		  = VK_INSERT;
const unsigned char KEYCODE_DELETE		  = VK_DELETE;
const unsigned char KEYCODE_HOME		  = VK_HOME;
const unsigned char KEYCODE_END			  = VK_END;
const unsigned char KEYCODE_UPARROW       = VK_UP;
const unsigned char KEYCODE_DOWNARROW     = VK_DOWN;
const unsigned char KEYCODE_LEFTARROW     = VK_LEFT;
const unsigned char KEYCODE_RIGHTARROW    = VK_RIGHT;
const unsigned char KEYCODE_LEFT_MOUSE	  = VK_LBUTTON;
const unsigned char KEYCODE_RIGHT_MOUSE	  = VK_RBUTTON;
const unsigned char KEYCODE_TILDE		  = 0xC0;
const unsigned char KEYCODE_LEFTBRACKET	  = 0xDB;
const unsigned char KEYCODE_RIGHTBRACKET  = 0xDD;


InputSystem::InputSystem(InputConfig const& config)
	:m_config( config )
{

}

InputSystem::~InputSystem()
{

}

void InputSystem::Startup()
{
	for (int i = 0; i < NUM_XBOX_CONTROLLERS; ++i)
	{
		m_controllers[i].m_id = i;
	}
	
	g_theEventSystem->SubscribeEventCallbackFunction("KeyPressed", InputSystem::Event_KeyPressed);
	g_theEventSystem->SubscribeEventCallbackFunction("KeyReleased", InputSystem::Event_KeyReleased);
	
}

void InputSystem::Shutdown()
{

}

void InputSystem::BeginFrame()
{
	for (int controllerIndex = 0; controllerIndex < NUM_XBOX_CONTROLLERS; ++controllerIndex)
	{
		m_controllers[controllerIndex].Update();
	}

	// Check if our current hidden mode does not match the state that Windows currently has the cursor in
	while (ShowCursor(FALSE) >= 0 && m_cursorState.m_hiddenMode); // Hide cursor
	while (ShowCursor(TRUE) < 0 && !m_cursorState.m_hiddenMode); // Show cursor

	// Save off the previous cursor client position from last frame.
	m_cursorState.m_cursorClientDelta = m_cursorState.m_cursorClientPosition;

	// Get the current cursor client position from Windows.
	POINT cursorPos;
	GetCursorPos(&cursorPos); // Global cursor position 
	ScreenToClient((HWND)Window::GetMainWindowInstance()->GetHwnd(), &cursorPos); // Convert to client area position

	m_cursorState.m_cursorClientPosition.x = cursorPos.x;
	m_cursorState.m_cursorClientPosition.y = cursorPos.y;

	// If we are in relative mode
	if (m_cursorState.m_relativeMode) {
		// Calculate cursor client delta
		m_cursorState.m_cursorClientDelta = IntVec2(m_cursorState.m_cursorClientPosition.x - m_cursorState.m_cursorClientDelta.x, m_cursorState.m_cursorClientPosition.y - m_cursorState.m_cursorClientDelta.y);

		// Set the Windows cursor position back to the center of our client region
		HWND hwnd = (HWND)Window::GetMainWindowInstance()->GetHwnd();
		RECT clientRect;
		GetClientRect(hwnd, &clientRect);
		POINT centerPoint = {
			(clientRect.right - clientRect.left) / 2,
			(clientRect.bottom - clientRect.top) / 2
		};
		ClientToScreen(hwnd, &centerPoint);
		SetCursorPos(centerPoint.x, centerPoint.y);

		// Immediately get the Windows cursor position again
		GetCursorPos(&cursorPos);
		ScreenToClient(hwnd, &cursorPos);
		m_cursorState.m_cursorClientPosition.x = cursorPos.x;
		m_cursorState.m_cursorClientPosition.y = cursorPos.y;
	}
	else {
		// If we are not in relative mode, just set our cursor client delta to zero.
		m_cursorState.m_cursorClientDelta = IntVec2(0, 0);
	}
}

void InputSystem::EndFrame()
{
	for (int i = 0; i < NUM_KEYCODES; ++i) {
		m_keyStates[i].m_wasPressedLastFrame = m_keyStates[i].m_isPressed;
	}
}

bool InputSystem::WasKeyJustPressed(unsigned char keyCode)
{
	if (m_keyStates[keyCode].m_isPressed && !m_keyStates[keyCode].m_wasPressedLastFrame)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool InputSystem::WasKeyJustReleased(unsigned char keyCode)
{
	if (!m_keyStates[keyCode].m_isPressed && m_keyStates[keyCode].m_wasPressedLastFrame)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool InputSystem::IsKeyDown(unsigned char keyCode)
{
	return m_keyStates[keyCode].m_isPressed;
}

bool InputSystem::HandleKeyPressed(unsigned char keyCode)
{
	m_keyStates[keyCode].m_isPressed = true;
	return true;
}

bool InputSystem::HandleKeyReleased(unsigned char keyCode)
{
	m_keyStates[keyCode].m_isPressed = false;
	return true;
}

XboxController const& InputSystem::GetController(int controllerID)
{
	return m_controllers[controllerID];
}

bool InputSystem::Event_KeyPressed(EventArgs& args)
{
	if (!g_theInput)
	{
		return false;
	}
	unsigned char keyCode = (unsigned char) args.GetValue("KeyCode", -1);
	g_theInput->HandleKeyPressed(keyCode);
	return true;
}

bool InputSystem::Event_KeyReleased(EventArgs& args)
{
	if (!g_theInput)
	{
		return false;
	}
	unsigned char keyCode = (unsigned char)args.GetValue("KeyCode", -1);
	g_theInput->HandleKeyReleased(keyCode);
	return true;
}

void InputSystem::SetCursorMode(bool hiddenMode, bool relativeMode)
{
	m_cursorState.m_hiddenMode = hiddenMode;
	m_cursorState.m_relativeMode = relativeMode;

	if (hiddenMode) {
		ShowCursor(FALSE); // Hide cursor
	}
	else {
		ShowCursor(TRUE); // Show cursor
	}
}

Vec2 InputSystem::GetCursorClientDelta() const
{
	if (!m_cursorState.m_relativeMode) {
		return Vec2(0, 0); // Return zero delta if not in relative mode
	}
	return Vec2(static_cast<float>(m_cursorState.m_cursorClientDelta.x), static_cast<float>(m_cursorState.m_cursorClientDelta.y));
}

Vec2 InputSystem::GetCursorNormalizedPosition() const
{
	IntVec2 clientDimensions = g_theWindow->GetClientDimensions();
	float normalizedX = static_cast<float>(m_cursorState.m_cursorClientPosition.x) / static_cast<float>(clientDimensions.x);
	float normalizedY = static_cast<float>(m_cursorState.m_cursorClientPosition.y) / static_cast<float>(clientDimensions.y);
	return Vec2(normalizedX, 1.0f - normalizedY); // Invert Y to match game screen conventions
}

CursorState InputSystem::GetCursorState()
{
	return m_cursorState;
}

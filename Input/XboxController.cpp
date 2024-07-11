#include <Windows.h> 
#include <Xinput.h>
#include "Engine/Input/XboxController.hpp"
#pragma comment( lib, "xinput9_1_0" ) // Xinput 1_4 doesn't work in older Windows versions; use XInput 9_1_0 explicitly for best compatibility

XboxController::XboxController()
{

}

XboxController::~XboxController()
{

}

bool XboxController::IsConnected() const
{
	return m_isConnected;
}

int XboxController::GetControllerID() const
{
	return m_id;
}

AnalogJoystick const& XboxController::GetLeftStick() const
{
	return m_leftStick;
}

AnalogJoystick const& XboxController::GetRightStick() const
{
	return m_rightStick;
}

float XboxController::GetLeftTrigger() const
{
	return m_leftTrigger;
}

float XboxController::GetRightTrigger() const
{
	return m_rightTrigger;
}

KeyButtonState const& XboxController::GetButton(XboxButtonID buttonID) const
{
	return m_buttons[static_cast<int>(buttonID)];
}

bool XboxController::IsButtonDown(XboxButtonID buttonID) const
{
	return m_buttons[static_cast<int>(buttonID)].m_isPressed;
}

bool XboxController::WasButtonJustPressed(XboxButtonID buttonID) const
{
	return m_buttons[static_cast<int>(buttonID)].m_isPressed &&
		!m_buttons[static_cast<int>(buttonID)].m_wasPressedLastFrame;
}

bool XboxController::WasButtonJustReleased(XboxButtonID buttonID) const
{
	return !m_buttons[static_cast<int>(buttonID)].m_isPressed &&
		m_buttons[static_cast<int>(buttonID)].m_wasPressedLastFrame;
}

void XboxController::Update()
{
	XINPUT_STATE xboxControllerState = {};

	DWORD result = XInputGetState(m_id, &xboxControllerState);

	if (result == ERROR_SUCCESS) {
		m_isConnected = true;

		// Update the joystick states
		UpdateJoyStick(m_leftStick, xboxControllerState.Gamepad.sThumbLX, xboxControllerState.Gamepad.sThumbLY);
		UpdateJoyStick(m_rightStick, xboxControllerState.Gamepad.sThumbRX, xboxControllerState.Gamepad.sThumbRY);

		// Update triggers
		updateTrigger(m_leftTrigger, xboxControllerState.Gamepad.bLeftTrigger);
		updateTrigger(m_rightTrigger, xboxControllerState.Gamepad.bRightTrigger);

		// Update buttons (here's an example for one, others will be similar)
		updateButton(XboxButtonID::XBOX_BUTTON_UP, xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_UP);
		updateButton(XboxButtonID::XBOX_BUTTON_DOWN, xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_DOWN);
		updateButton(XboxButtonID::XBOX_BUTTON_LEFT, xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_LEFT);
		updateButton(XboxButtonID::XBOX_BUTTON_RIGHT, xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_RIGHT); 
		
		updateButton(XboxButtonID::XBOX_BUTTON_LB, xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER);
		updateButton(XboxButtonID::XBOX_BUTTON_RB, xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER);
		updateButton(XboxButtonID::XBOX_BUTTON_LS, xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_THUMB);
		updateButton(XboxButtonID::XBOX_BUTTON_RS, xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_THUMB);

		updateButton(XboxButtonID::XBOX_BUTTON_A, xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_A);
		updateButton(XboxButtonID::XBOX_BUTTON_B, xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_B);
		updateButton(XboxButtonID::XBOX_BUTTON_X, xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_X);
		updateButton(XboxButtonID::XBOX_BUTTON_Y, xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_Y);

		updateButton(XboxButtonID::XBOX_BUTTON_START, xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_START);
		updateButton(XboxButtonID::XBOX_BUTTON_BACK, xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_BACK);

	}
	else {
		m_isConnected = false;
		Reset();
	}
}

void XboxController::Reset()
{
	m_isConnected = false;
	m_leftTrigger = 0.0f;
	m_rightTrigger = 0.0f;
	m_leftStick.Reset();
	m_rightStick.Reset();
	for (int i = 0; i < (int)XboxButtonID::NUM; i++)
	{
		m_buttons[i].m_isPressed = false;
		m_buttons[i].m_wasPressedLastFrame = false;
	}
}

void XboxController::UpdateJoyStick(AnalogJoystick& out_joystick, short rawX, short rawY)
{
	float normalizedX = rawX / 32768.0f;
	float normalizedY = rawY / 32768.0f;
	out_joystick.updatePosition(normalizedX, normalizedY);
}

void XboxController::updateTrigger(float& out_triggerValue, unsigned char rawValue)
{
	out_triggerValue = rawValue / 255.0f;
}

void XboxController::updateButton(XboxButtonID buttonID, unsigned short buttonFlags, unsigned short buttonFlag)
{
	m_buttons[(int)buttonID].m_wasPressedLastFrame = m_buttons[(int)buttonID].m_isPressed;
	m_buttons[(int)buttonID].m_isPressed = (buttonFlags & buttonFlag) != 0;
}

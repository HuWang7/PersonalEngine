#pragma once

class KeyButtonState
{
public:
	KeyButtonState();

public:
	bool m_isPressed;       // Current state of the button.
	bool m_wasPressedLastFrame;  // State of the button in the previous frame.
};

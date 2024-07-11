#pragma once
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/EventSystem.hpp"
 

struct DevConsoleLine {
	Rgba8 color;
	std::string text;
};

struct DevConsoleConfig
{
	Renderer* m_theRenderer = nullptr;
	Camera* m_camera = nullptr;
	std::string m_fontName = "SquirrelFixedFont";
	float m_fontAspect = 0.7f;
	int m_linesOnScreen = 40;
	int m_maxCommandHistory = 128;
	bool m_startOpen = false;
};


//------------------------------------------------------------------
class DevConsole
{
public:
	DevConsole( DevConsoleConfig const& config);
	~DevConsole();

	void Startup();
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	void Execute( std::string const& consoleCommandText );
	void AddLine( Rgba8 const& color, std::string const& text );
	void Render( AABB2 const& bounds, Renderer* rendererOverride=nullptr) const;
	
	void ToggleMode();
	void ToggleInsertionPointVisibility();
	bool IsOpen();

	static const Rgba8 ERROR_COLOR;
	static const Rgba8 WARNING;
	static const Rgba8 INFO_MAJOR;
	static const Rgba8 INFO_MINOR;
	static const Rgba8 COMMAND_ECHO;
	static const Rgba8 COMMAND_REMOTE_ECHO;
	static const Rgba8 INPUT_TEXT;
	static const Rgba8 INPUT_INSERTION_POINT;

	// Handle key input.
	static bool Event_KeyPressed(EventArgs& args);

	// Handle char input by appending valid characters to our current input line.
	static bool Event_CharInput(EventArgs& args);

	// Clear all lines of text.
	static bool Command_Clear(EventArgs& args);

	//Echo the text in the argument to the dev console.
	static bool Command_Echo(EventArgs& args);

	// Display all currently registered commands in the event system.
	static bool Command_Help(EventArgs& args);

public:
	void Render_OpenFull( AABB2 const& bounds, Renderer& renderer, BitmapFont& font, float fontAspect=1.f) const;

protected:
	DevConsoleConfig m_config;

	// True if the dev console is currently visible and accepting input.
	bool m_isOpen = false;

	// All lines added to the dev console since the last time it was cleared.
	std::vector<DevConsoleLine> m_lines;

	// Our current line of input text.
	std::string m_inputText;

	// Index of the insertion point in our current input text.
	int m_insertionPointPosition = 0;

	// True if our insertion point is currently in the visibile phase of blinking.
	bool m_insertionPointVisible = true;

	// Timer for controlling insertion point visibility.
	Timer* m_insertionPointBlinkTimer;

	// History of all commands executed.
	std::vector<std::string> m_commandHistory;

	// Our current index in our history of commands as we are scrolling.
	int m_historyIndex = -1;

};
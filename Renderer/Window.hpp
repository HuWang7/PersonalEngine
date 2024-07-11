#pragma once
#include <string>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"

//--------------------------------------------------------------------------------
class InputSystem;


//--------------------------------------------------------------------------------
struct WindowConfig
{
	std::string  m_windowTitle = "Untitled App";
	float        m_clientAspect = 2.0f;
	bool         m_isFullscreen = false;
};


//--------------------------------------------------------------------------------
class Window
{
public:
	Window( WindowConfig const& config );
	~Window();

	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	
	WindowConfig const& GetConfig() const;
	float               GetAspect() const;
	void*               GetDC() const;
	void*				GetHwnd() const;
	IntVec2				GetClientDimensions();

	static Window* GetMainWindowInstance(); 
	

protected:
	void CreatOSWindow();
	void RunMessagePump();

protected:
	static Window*         s_mainWindow;  //#ToDo later on: refactor to support multiple windows

protected:
	WindowConfig           m_config;
	void*                  m_hwnd = nullptr; //HWND in Windows (handle to the current window)
	void*                  m_dc = nullptr; // HDC in Windows (handle to the disPlay device context)
};


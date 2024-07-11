#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/InputSystem.hpp"

DevConsole* g_theConsole = nullptr;
extern EventSystem* g_theEventSystem;
extern BitmapFont* g_theFont;

const Rgba8 DevConsole::ERROR_COLOR = Rgba8::RED;
const Rgba8 DevConsole::WARNING = Rgba8::YELLOW;
const Rgba8 DevConsole::INFO_MAJOR = Rgba8::GREEN;
const Rgba8 DevConsole::INFO_MINOR = Rgba8::WHITE;

DevConsole::DevConsole(DevConsoleConfig const& config)
	:m_config(config)
{
	m_insertionPointBlinkTimer = new Timer(0.5f, &Clock::GetSystemClock());
	m_insertionPointBlinkTimer->Start();
}

DevConsole::~DevConsole()
{
	delete m_insertionPointBlinkTimer;
	m_insertionPointBlinkTimer = nullptr;
}

void DevConsole::Startup()
{
	g_theEventSystem->SubscribeEventCallbackFunction("KeyPressed", DevConsole::Event_KeyPressed);
	g_theEventSystem->SubscribeEventCallbackFunction("CharInput", DevConsole::Event_CharInput);
	g_theEventSystem->SubscribeEventCallbackFunction("help", Command_Help);
	//g_theEventSystem->SubscribeEventCallbackFunction("clear", Command_Clear);
	g_theConsole->AddLine(DevConsole::INFO_MAJOR, "help - Get help menu");
}

void DevConsole::Shutdown()
{

}

void DevConsole::BeginFrame()
{
	if (m_insertionPointBlinkTimer->DecrementPeriodIfElapsed()) {
		m_insertionPointVisible = !m_insertionPointVisible;
	}
}

void DevConsole::EndFrame()
{

}

void DevConsole::Execute(std::string const& consoleCommandText)
{
	std::vector<std::string> commands = SplitStringOnDelimiter(consoleCommandText, '\n');

	for (const std::string& commandLine : commands) 
	{
		size_t firstSpace = commandLine.find(' ');
		std::string commandName;
		std::string argsString;

		if (firstSpace != std::string::npos) 
		{
			commandName = commandLine.substr(0, firstSpace);
			argsString = commandLine.substr(firstSpace + 1);
		}
		else {
			commandName = commandLine;
		}

		EventArgs args;
		std::vector<std::string> keyValuePairs = SplitStringOnDelimiter(argsString, ' ');
		for (const std::string& pair : keyValuePairs) 
		{
			size_t equalsPos = pair.find('=');
			if (equalsPos != std::string::npos) 
			{
				std::string key = pair.substr(0, equalsPos);
				std::string value = pair.substr(equalsPos + 1);
				args.SetValue(key, value);
			}
		}

		g_theEventSystem->FireEvent(commandName, args);
	}

	if (!consoleCommandText.empty()) 
	{
		m_commandHistory.push_back(consoleCommandText);
		m_historyIndex = -1;
	}
}

void DevConsole::AddLine(Rgba8 const& color, std::string const& text)
{
	DevConsoleLine line;
	line.color = color;
	line.text = text;
	m_lines.push_back(line);
}

void DevConsole::Render(AABB2 const& bounds, Renderer* rendererOverride/*=nullptr*/) const
{
	if (rendererOverride == nullptr)
	{
		rendererOverride = m_config.m_theRenderer;
	}
	rendererOverride->SetBlendMode(BlendMode::ALPHA);

	std::vector<Vertex_PCU> verts;
	AddVertsForAABB2D(verts, bounds, Rgba8(0, 0, 0, 128));
	rendererOverride->DrawVertexArray((int)verts.size(), verts.data());
	rendererOverride->BindTexture(nullptr);

	float lineBoundsMaxY= 0.f;
	float lineHeight = bounds.m_maxs.y*0.05f;
	std::vector<Vertex_PCU> textVerts;
	textVerts.reserve(10000);
	int numOfVector = (int)m_lines.size();
	for (const auto& line : m_lines) 
	{
		if (numOfVector > 0 && line.text.size() != 0)
		{
			lineBoundsMaxY = (numOfVector * lineHeight);
			float lineBoundMinY = lineBoundsMaxY - lineHeight;
			AABB2 LineBounds = AABB2(Vec2(0.f, lineBoundMinY), Vec2(bounds.m_maxs.x, lineBoundsMaxY));
			g_theFont->AddVertsForTextInBox2D(textVerts, LineBounds, lineHeight,line.text, line.color, 0.6f, Vec2(0.f, 0.f));
			numOfVector -= 1;
		}
		else
		{
			continue;
		}
	}
	Texture* fontTexture = &g_theFont->GetTexture();
	rendererOverride->BindTexture(fontTexture);
	rendererOverride->DrawVertexArray((int)textVerts.size(), textVerts.data());
	rendererOverride->BindTexture(nullptr);

}

void DevConsole::ToggleMode()
{
	m_isOpen = !m_isOpen;
}

void DevConsole::ToggleInsertionPointVisibility()
{
	m_insertionPointVisible = !m_insertionPointVisible;
}

bool DevConsole::IsOpen()
{
	return m_isOpen;
}

void DevConsole::Render_OpenFull(AABB2 const& bounds, Renderer& renderer, BitmapFont& font, float fontAspect/*=1.f*/) const {
	UNUSED(fontAspect);
    renderer.SetBlendMode(BlendMode::ALPHA);
    std::vector<Vertex_PCU> backgroundVerts;
    AddVertsForAABB2D(backgroundVerts, bounds, Rgba8(0, 0, 0, 128));
    renderer.DrawVertexArray(static_cast<int>(backgroundVerts.size()), backgroundVerts.data());

    const float lineHeight = bounds.GetDimensions().y / static_cast<float>(m_config.m_linesOnScreen);
    std::vector<Vertex_PCU> textVerts;
	std::vector<Vertex_PCU> shadowVerts;
    textVerts.reserve(10000);
	shadowVerts.reserve(10000);

    float currentY = bounds.m_mins.y + lineHeight;
	Vec2 shadowOffset(2.0f, -2.0f);

    // Render each line of the console output
    int linesToRender = std::min(static_cast<int>(m_lines.size()), m_config.m_linesOnScreen - 1);
    auto lineIt = m_lines.rbegin();
    for (int i = 0; i < linesToRender; ++i, ++lineIt) {
        const DevConsoleLine& line = *lineIt;
        AABB2 lineBounds = AABB2(Vec2(bounds.m_mins.x, currentY), Vec2(bounds.m_maxs.x, currentY + lineHeight));
		AABB2 shadowLineBounds = AABB2(Vec2(bounds.m_mins.x, currentY) + shadowOffset, Vec2(bounds.m_maxs.x, currentY + lineHeight) + shadowOffset);
		font.AddVertsForTextInBox2D(shadowVerts, shadowLineBounds, lineHeight, line.text, Rgba8::BLACK, 0.6f, Vec2(0.f, 0.f));
        font.AddVertsForTextInBox2D(textVerts, lineBounds, lineHeight, line.text, line.color, 0.6f, Vec2(0.f, 0.f));
        currentY += lineHeight;
    }

    // Render the input text line at the bottom
	std::string inputTextBeforeCursor = m_inputText.substr(0, m_insertionPointPosition);
	std::string inputTextAfterCursor = m_inputText.substr(m_insertionPointPosition);
	std::string inputDisplayText;
	if (m_insertionPointVisible) {
		inputDisplayText = "> " + inputTextBeforeCursor + "_" + inputTextAfterCursor;
	}
	else
	{
		inputDisplayText = "> " + inputTextBeforeCursor + inputTextAfterCursor;
	}
    AABB2 inputLineBounds = AABB2(Vec2(bounds.m_mins.x, bounds.m_mins.y), Vec2(bounds.m_maxs.x, bounds.m_mins.y + lineHeight));
	AABB2 shadowInputLineBounds = AABB2(Vec2(bounds.m_mins.x, bounds.m_mins.y) + shadowOffset, Vec2(bounds.m_maxs.x, bounds.m_mins.y + lineHeight) + shadowOffset);
	font.AddVertsForTextInBox2D(shadowVerts, shadowInputLineBounds, lineHeight, inputDisplayText, Rgba8::BLACK, 0.6f, Vec2(0.f, 0.f));
    font.AddVertsForTextInBox2D(textVerts, inputLineBounds, lineHeight, inputDisplayText, Rgba8::WHITE, 0.6f, Vec2(0.f, 0.f));

	Texture* fontTexture = &font.GetTexture();
	if (fontTexture) {
		renderer.BindTexture(fontTexture);
		renderer.DrawVertexArray(static_cast<int>(shadowVerts.size()), shadowVerts.data());
		renderer.BindTexture(nullptr);

		renderer.BindTexture(fontTexture);
		renderer.DrawVertexArray(static_cast<int>(textVerts.size()), textVerts.data());
		renderer.BindTexture(nullptr);
	}
}


bool DevConsole::Event_KeyPressed(EventArgs& args)
{
	if (g_theConsole->IsOpen())
	{
		unsigned char keyCode = (unsigned char)args.GetValue("KeyCode", 0);
		if (keyCode == KEYCODE_ENTER)
		{
			if (g_theConsole->m_inputText.empty())
			{
				g_theConsole->ToggleMode();
			}
			else
			{
				g_theConsole->Execute(g_theConsole->m_inputText);
				g_theConsole->m_inputText.clear();
				g_theConsole->m_insertionPointPosition = 0;
			}
		}
		if (keyCode == KEYCODE_TILDE)
		{
			g_theConsole->ToggleMode();
		}
		if (keyCode == KEYCODE_ESC)
		{
			if (g_theConsole->m_inputText.empty())
			{
				g_theConsole->ToggleMode();
			}
			else
			{
				g_theConsole->m_inputText.clear();
				g_theConsole->m_insertionPointPosition = 0;
			}
		}
		if (keyCode == KEYCODE_LEFTARROW)
		{
			if (g_theConsole->m_insertionPointPosition > 0)
			{
				g_theConsole->m_insertionPointPosition--;
			}
		}
		if (keyCode == KEYCODE_RIGHTARROW)
		{
			if (g_theConsole->m_insertionPointPosition < g_theConsole->m_inputText.length()) 
			{
				g_theConsole->m_insertionPointPosition++;
			}
		}
		if (keyCode == KEYCODE_HOME)
		{
			g_theConsole->m_insertionPointPosition = 0;
		}
		if (keyCode == KEYCODE_END)
		{
			g_theConsole->m_insertionPointPosition = (int)g_theConsole->m_inputText.length();
		}
		if (keyCode == KEYCODE_DELETE)
		{
			if (g_theConsole->m_insertionPointPosition < g_theConsole->m_inputText.length())
			{
				g_theConsole->m_inputText.erase(g_theConsole->m_insertionPointPosition, 1);
			}
		}
		if (keyCode == KEYCODE_BACKSPACE)
		{
			if (g_theConsole->m_insertionPointPosition > 0) {
				g_theConsole->m_inputText.erase(g_theConsole->m_insertionPointPosition - 1, 1);
				g_theConsole->m_insertionPointPosition--;
			}
		}
		if (keyCode == KEYCODE_UPARROW)
		{
			if (!g_theConsole->m_commandHistory.empty() && g_theConsole->m_historyIndex < static_cast<int>(g_theConsole->m_commandHistory.size()) - 1) {
				g_theConsole->m_historyIndex++;
				g_theConsole->m_inputText = g_theConsole->m_commandHistory[g_theConsole->m_commandHistory.size() - 1 - g_theConsole->m_historyIndex];
				g_theConsole->m_insertionPointPosition = (int)g_theConsole->m_inputText.length();
			}
		}
		if (keyCode == KEYCODE_DOWNARROW)
		{
			if (g_theConsole->m_historyIndex > 0) {
				g_theConsole->m_historyIndex--;
				g_theConsole->m_inputText = g_theConsole->m_commandHistory[g_theConsole->m_commandHistory.size() - 1 - g_theConsole->m_historyIndex];
				g_theConsole->m_insertionPointPosition = (int)g_theConsole->m_inputText.length();
			}
			else if (g_theConsole->m_historyIndex == 0) {
				g_theConsole->m_historyIndex = -1;
				g_theConsole->m_inputText.clear();
				g_theConsole->m_insertionPointPosition = 0;
			}
		}
		g_theConsole->m_insertionPointVisible = true;
		g_theConsole->m_insertionPointBlinkTimer->Start();
		return true;
	}
	return false;
}

bool DevConsole::Event_CharInput(EventArgs& args)
{
	if (g_theConsole->IsOpen())
	{
		unsigned char inputChar = (unsigned char)args.GetValue("char", 0);
		if (inputChar >= 32 && inputChar <= 126 && inputChar != '~' && inputChar != '`') {
			g_theConsole->m_inputText.insert(g_theConsole->m_insertionPointPosition, 1, inputChar);
			g_theConsole->m_insertionPointPosition ++;
		}
		g_theConsole->m_insertionPointVisible = true; 
		g_theConsole->m_insertionPointBlinkTimer->Start();
		return true;
	}
	return false; 
}

bool DevConsole::Command_Clear(EventArgs& args)
{
	UNUSED(args);
	g_theConsole->m_lines.clear();
	g_theConsole->AddLine(INFO_MAJOR, "Console cleared.");
	g_theConsole->AddLine(INFO_MAJOR, "DebugRendererObject cleared.");
	return true;
}

bool DevConsole::Command_Echo(EventArgs& args)
{
	std::string message = args.GetValue("message", ""); 
	g_theConsole->AddLine(INFO_MINOR, message); 
	return true; 
}

bool DevConsole::Command_Help(EventArgs& args)
{
	UNUSED(args);
	auto commandNames = g_theEventSystem->GetAllEventNames();
	for (const auto& name : commandNames) {
		g_theConsole->AddLine(INFO_MAJOR, name);
	}
	return true;
}





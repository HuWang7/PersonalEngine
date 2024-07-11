#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/DevConsole.hpp"

EventSystem* g_theEventSystem = nullptr;
extern DevConsole* g_theConsole;


EventSystem::EventSystem(EventSystemConfig const& config)
	:m_config(config)
{

}

EventSystem::~EventSystem()
{

}

void EventSystem::Startup()
{

}

void EventSystem::Shutdown()
{

}

void EventSystem::BeginFrame()
{

}

void EventSystem::EndFrame()
{

}

void EventSystem::SubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr)
{
	EventSubscription newSubscription;
	newSubscription.callback = functionPtr;
	m_subscriptions[eventName].push_back(newSubscription);
}

void EventSystem::UnsubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr)
{
	auto iter = m_subscriptions.find(eventName);
	if (iter != m_subscriptions.end()) {
		auto& subscriptions = iter->second;
		for (auto it = subscriptions.begin(); it != subscriptions.end(); ) {
			if (it->callback == functionPtr) {
				it = subscriptions.erase(it); // Erase and move to the next element
			}
			else {
				++it; // Move to the next element
			}
		}
	}
}

void EventSystem::FireEvent(std::string const& eventName, EventArgs& args)
{
	auto iter = m_subscriptions.find(eventName);
	if (iter != m_subscriptions.end()) {
		for (EventSubscription& subscription : iter->second) {
			bool wasConsumed = subscription.callback(args);
			if (wasConsumed) {
				break;
			}
		}
		if (eventName != "KeyPressed" && eventName != "CharInput" && eventName != "KeyReleased")
		{
			g_theConsole->AddLine(DevConsole::INFO_MINOR, "Fire event: " + eventName);
		}
	}
	else
	{
		if (g_theConsole && g_theConsole->IsOpen())
		{
			g_theConsole->AddLine(DevConsole::ERROR_COLOR, "Unknown command: " + eventName);
		}
	}
}

void EventSystem::FireEvent(std::string const& eventName)
{
	auto iter = m_subscriptions.find(eventName);
	if (iter != m_subscriptions.end()) {
		EventArgs args;
		FireEvent(eventName, args);
		if (eventName != "KeyPressed" && eventName != "CharInput" && eventName != "KeyReleased")
		{
			g_theConsole->AddLine(DevConsole::INFO_MINOR, "Fire event: " + eventName);
		}
	}
	else
	{
		if (g_theConsole && g_theConsole->IsOpen())
		{
			g_theConsole->AddLine(DevConsole::ERROR_COLOR, "Unknown command: " + eventName);
		}
	}
}

std::vector<std::string> EventSystem::GetAllEventNames() const
{
	std::vector<std::string> eventNames;
	for (const auto& subscriptionPair : m_subscriptions) {
		eventNames.push_back(subscriptionPair.first);
	}
	return eventNames;
}

void SubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr)
{
	if (g_theEventSystem) {
		g_theEventSystem->SubscribeEventCallbackFunction(eventName, functionPtr);
	}
}

void UnsubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr)
{
	if (g_theEventSystem) {
		g_theEventSystem->UnsubscribeEventCallbackFunction(eventName, functionPtr);
	}
}

void FireEvent(std::string const& eventName, EventArgs& args)
{
	if (g_theEventSystem) {
		g_theEventSystem->FireEvent(eventName, args);
	}
}

void FireEvent(std::string const& eventName)
{
	if (g_theEventSystem) {
		g_theEventSystem->FireEvent(eventName);
	}
}

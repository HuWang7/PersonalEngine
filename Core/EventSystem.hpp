#pragma once
#include <vector>
#include <string>
#include <map>
#include "Engine/Core/NamedStrings.hpp"
//------------------------------------------------------------------------------
typedef NamedStrings EventArgs;
typedef bool (*EventCallbackFunction)(EventArgs& args);

struct EventSubscription
{
	EventCallbackFunction callback; 
};

//------------------------------------------------------------------------------
typedef std::vector<EventSubscription> SubscriptionList;

struct EventSystemConfig
{

};

//------------------------------------------------------------------------------
class EventSystem
{
public:
	EventSystem( EventSystemConfig const& config);
	~EventSystem();
	void Startup();
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	void SubscribeEventCallbackFunction( std::string const& eventName, EventCallbackFunction functionPtr);
	void UnsubscribeEventCallbackFunction( std::string const& eventName, EventCallbackFunction functionPtr);
	void FireEvent( std::string const& eventName, EventArgs& args );
	void FireEvent( std::string const& eventName);
	std::vector<std::string> GetAllEventNames() const;

protected:
	EventSystemConfig	m_config;
	std::map< std::string, SubscriptionList > m_subscriptions; 
};

//------------------------------------------------------------------------------
// Standalone global-namespace helper functions; these forward to "the" event system, if it exists
//

void SubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr);
void UnsubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr);
void FireEvent(std::string const& eventName, EventArgs& args);
void FireEvent(std::string const& eventName);

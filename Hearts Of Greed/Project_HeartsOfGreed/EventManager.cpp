#include "App.h"
#include "EventManager.h"


Event::Event()
{
	id = EVENT_ENUM::NULL_EVENT;
	idTrigger = EVENT_ENUM::NULL_EVENT;
}

Event::Event(EVENT_ENUM idValue, EVENT_ENUM idTriggerValue)
{
	id = idValue;
	idTrigger = idTriggerValue;
}

Event::~Event()
{
}

ModuleEventManager::ModuleEventManager()
{
	name.create("EventyManager");
}


// Destructor
ModuleEventManager::~ModuleEventManager()
{}


// Called before render is available
bool ModuleEventManager::Awake(pugi::xml_node& config)
{
	bool ret = true;

	return ret;
}


// Called before the first frame
bool ModuleEventManager::Start()
{
	bool ret = true;


	return ret;
}


//// Called before quitting
bool ModuleEventManager::CleanUp()
{
	CleanListenerMap();

	eventVector.clear();


	return true;
}

void ModuleEventManager::GenerateEvent(EVENT_ENUM eventId, EVENT_ENUM eventTriggerId)
{

	if (eventId != EVENT_ENUM::NULL_EVENT)
	{
		Event newEvent(eventId, eventTriggerId);

		EVENT_ENUM eventCheck = CheckEventTrigger(eventId);

		if (eventCheck != EVENT_ENUM::NULL_EVENT)
		{
			FireEvent(eventCheck);
		}

		if (eventTriggerId == EVENT_ENUM::NULL_EVENT)
		{
			FireEvent(eventId);
		}
		else
		{
			eventVector.push_back(newEvent);
		}

	}

}

void ModuleEventManager::FireEvent(EVENT_ENUM eventId) const
{

	if (eventId != EVENT_ENUM::NULL_EVENT)
	{
		if (eventListenersMap.size() == 0)
			return;

		if (eventListenersMap.count(eventId) == 1)
		{
			std::vector<Module*> listeners = eventListenersMap.at(eventId);
			int numElem = listeners.size();
			for (int i = 0; i < numElem; i++)
			{
				listeners[i]->AddEvent(eventId);
			}
			listeners.clear();
		}
	}

}

EVENT_ENUM ModuleEventManager::CheckEventTrigger(EVENT_ENUM eventTrigger)
{
	EVENT_ENUM ret= EVENT_ENUM::NULL_EVENT;

	int numElem = eventVector.size();

	for (int i = 0; i < numElem; i++)
	{
		if (eventVector[i].idTrigger == eventTrigger)
		{
			ret= eventVector[i].id;
			eventVector.erase(eventVector.begin() + i);
			break;
		}
	}
	
	return ret;
}

//returns true if the event has been registered or if the new listener has been added, else returns false
bool ModuleEventManager::EventRegister(EVENT_ENUM event, Module* mod)
{
	bool ret = false;

	//if event isn't on the map create a new entry on the map
	if (!eventListenersMap.count(event))
	{
		CreateEventOnMap(event);
		eventListenersMap[event].push_back(mod);
		ret = true;
	}
	//if listener has already been added don't add
	//else add a new listener
	else if (FindListener(event, mod)==-1)
	{
		eventListenersMap[event].push_back(mod);
		ret = true;
	}


	return ret;
}


//returns true if an element has been erased from the listeners map
bool ModuleEventManager::EventUnRegister(EVENT_ENUM event, Module* mod)
{
	bool ret = false;

	//if event does exist
	if (eventListenersMap.count(event))
	{	
		//then if listener does exist erase the element
		int i = FindListener(event, mod);
		if (i!=-1)
		{
			EraseListener(event, mod, i);
			ret = true;
		}

	}

	
	return ret;
}


void ModuleEventManager::CreateEventOnMap(EVENT_ENUM event)
{
	//further testing in this function is needed TODO
	std::vector<Module*> vec;
	eventListenersMap[event] = vec;

	vec.clear();
}


//returns a positive number if a listener is already in the vector else returns -1
int ModuleEventManager::FindListener(EVENT_ENUM event, Module* mod)
{
	int ret = -1;


	for (int i = 0; i < eventListenersMap[event].size(); i++)
	{


		if (eventListenersMap[event].at(i) == mod)
		{
			ret = i;
			break;
		}


	}


	return ret;
}


std::vector<Module*>::iterator ModuleEventManager::EraseListener(EVENT_ENUM event, Module* mod,int vecId)
{
	std::vector<Module*>::iterator iter=eventListenersMap[event].begin();
	
	//generate the iterator for the given ID
	for (int i = 0; i < vecId; i++)
	{
		iter++;
	}

	//delete the listener
	eventListenersMap[event].erase(iter);
	
	//if there are no more listeners delete the map event entry
	if (eventListenersMap[event].size() <= 0)
	{
		eventListenersMap.erase(event);
	}


	return iter;
}


void  ModuleEventManager::CleanListenerMap()
{
	eventListenersMap.clear();
}


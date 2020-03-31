// ----------------------------------------------------
// Module.h
// Interface for all engine modules
// ----------------------------------------------------

#ifndef __MODULE_H__
#define __MODULE_H__

#define MAX_COLLIDERS_PER_FRAME 50

#include "p2SString.h"
#include <vector>
#include "PugiXml\src\pugixml.hpp"
#include "p2Point.h"

struct Collider;

class App;

enum class EVENT_ENUM
{
	AUDIO_INCOMES,
	KEYBOARD_INPUT,
	MOUSE_INPUT,
	SELECT_UNITS,
	STOP_SELECTING_UNITS,
	ENTITY_COMMAND, //aka right click
	ENTITY_INTERACTION, //aka left click
	
	// Hero creation /revival
	HERO_MELEE_ON_BATTLE,
	HERO_GATHERER_ON_BATTLE,
	HERO_RANGED_ON_BATTLE,

	// Heroe dies
	HERO_MELEE_OUT,
	HERO_GATHERER_OUT,
	HERO_RANGED_OUT,

	PAUSE_GAME,
	NULL_EVENT
};

class Module
{

public:

	Module();

	void Init();

	// Called before render is available
	virtual bool Awake(pugi::xml_node&);

	// Called before the first frame
	virtual bool Start();

	// Called each loop iteration
	virtual bool PreUpdate(float dt);

	// Called each loop iteration
	virtual bool Update(float dt);

	// Called each loop iteration
	virtual bool PostUpdate(float dt);

	// Called before quitting
	virtual bool CleanUp();

	virtual bool Load(pugi::xml_node&);

	virtual bool Save(pugi::xml_node&) const;

	// Module activation 
	bool IsEnabled() const;

	void Enable();

	void Disable();


	virtual void OnCollision(Collider*, Collider*);

	// Event managing: saves an event into the listener vector

	void AddEvent(EVENT_ENUM eventId);

	void RemoveEvent(EVENT_ENUM eventId);

	bool CheckListener(Module*mod);

	virtual void ExecuteEvent(EVENT_ENUM eventId);

public:

	P2SString				name;
	bool					active;
	std::vector<EVENT_ENUM>	listener;

private:

	bool enabled = true;

};

#endif // __MODULE_H__
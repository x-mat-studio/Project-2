#ifndef __APP_H__
#define __APP_H__

#define TIME_CONST 50
#define MAX_DT 3

#include "Module.h"
#include "PerfectTimer.h"
#include "SimpleTimer.h"
#include "PugiXml\src\pugixml.hpp"

typedef unsigned int uint;

// Modules
class ModuleWindow;
class ModuleInput;
class ModuleAudio;
class ModuleRender;
class ModuleTextures;
class ModuleCollision;
class ModuleMap;
class ModuleTestScene;
class ModulePlayer;
class ModuleEntityManager;
class ModuleEventManager;

class App
{
public:

	// Constructor
	App(int argc, char* args[]);

	// Destructor
	virtual ~App();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update();

	// Called before quitting
	bool CleanUp();

	// Add a new module to handle
	void AddModule(Module* module);

	// Exposing some properties for reading
	int GetArgc() const;
	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;

	void LoadGame();
	void SaveGame() const;
	

private:

	// Load config file
	pugi::xml_node LoadConfig(pugi::xml_document&) const;

	// Call modules before each loop iteration
	void PrepareUpdate();

	// Call modules before each loop iteration
	void FinishUpdate();

	// Call modules before each loop iteration
	bool PreUpdate();

	// Call modules on each loop iteration
	bool DoUpdate();

	// Call modules after each loop iteration
	bool PostUpdate();

	// Load / Save
	bool LoadGameNow();
	bool SavegameNow() const;

public:

	// Modules
	ModuleWindow*			win;
	ModuleInput*			input;
	ModuleAudio*			audio;
	ModuleRender*			render;
	ModuleTextures*			tex;
	ModuleMap*				map;
	ModuleTestScene*		testScene;
	ModuleCollision*		coll;
	ModulePlayer*			player;
	ModuleEntityManager*	entityManager;
	ModuleEventManager*		eventManager;

	uint32_t			frames;
	
	uint				frameRate = 0;

	bool				vSyncActivated;
	bool				debugMode = false;

private:
	std::vector<Module*>	modules;

	int					argc;
	char**				args;

	P2SString			title;
	P2SString			organization;
	P2SString			loadGame;
	mutable P2SString	saveGame;

	PerfectTimer		pTimer;
	
	SimpleTimer			startupTime;
	SimpleTimer			frameTime;
	SimpleTimer			lastSecFrameTime;
	
	uint64				frameCount = 0;
	
	uint32				lastSecFrameCount = 0;
	uint32				prevLastSecFrameCount = 0;

	bool				capFrames;
	bool				wantToLoad;
	mutable bool		wantToSave;

	float				dt;
};

extern App* app; 

#endif
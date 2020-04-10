#ifndef __LOSE_SCENE__
#define __LOSE_SCENE__

#include "Module.h"
#include "SDL/include/SDL.h"


class ModuleLoseScene : public Module
{
public:

	ModuleLoseScene();

	// Destructor
	virtual ~ModuleLoseScene();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate(float dt);

	// Called each loop iteration
	bool Update(float dt);

	// Called each loop iteration
	bool PostUpdate(float dt);

	// Called before quitting
	bool CleanUp();


	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

private:

	void ExecuteEvent(EVENT_ENUM eventId) const;

private:
	//images
	SDL_Texture* youLost;

	//sounds
	//int winMusic;

};

#endif // __LOSE_SCENE__
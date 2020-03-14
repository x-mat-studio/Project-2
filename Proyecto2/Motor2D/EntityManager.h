#ifndef __ENTITYMANAGER_H__
#define __ENTITYMANAGER_H__

#include "SDL/include/SDL.h"
#include "Module.h"
#include "vector"

class Hero;

class Entity;

class ModuleEntityManager : public Module
{
public:

	ModuleEntityManager();

	// Destructor
	virtual ~ModuleEntityManager();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate(float dt);

	//// Called before quitting
	//bool CleanUp();

	//// Load / Save
	//bool Load(pugi::xml_node&);
	//bool Save(pugi::xml_node&) const;

private:

	//The list where we will store all the entities
	std::vector <Entity> entityVector;

	Hero* test;

};

#endif //__ENTITYMANAGER_H__
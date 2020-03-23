#pragma once

#ifndef __AI_H__
#define __AI_H__

#include "Module.h"
#include "SDL/include/SDL_rect.h"

class ModuleAI : public Module
{
public:

	ModuleAI();
	~ModuleAI();

	bool Awake(pugi::xml_node&);

	void OnCollision(Collider*, Collider*);

	iMPoint* GetObjective();
	

private:
	void CreateSelectionCollider(Collider*);
	
private:
	iMPoint objectivePos;
};


#endif//__AI_H__
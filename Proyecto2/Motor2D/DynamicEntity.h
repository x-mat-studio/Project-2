#pragma once

#ifndef __DYNAMICENTITY_H__
#define __DYNAMICENTITY_H__

#include "Entity.h"
#include <list>

class DynamicEntity : public Entity
{
public:

	DynamicEntity(fMPoint position, ENTITY_TYPE type, Collider* collider, int moveRange1, int moveRange2);

	~DynamicEntity();

	//Returns true while moving, returns false when stops
	bool Move();

	bool GeneratePath(int x, int y);
	void DebugDraw();

public:
	int pathToFollow;
	bool isMoving;

	//used for moving close entities to have an smooth group movement
	int moveRange1;
	int moveRange2;

	fMPoint speed;

protected:

	iMPoint origin, mouse;
	Animation* current_animation;
	std::vector <iMPoint> path;
	std::vector<DynamicEntity*> closeEntityList;
	std::vector<DynamicEntity*> collidingEntityList;

protected:

	void Draw(float dt);

private:
	fMPoint DynamicEntity::GetDirectionSpeed(std::vector<DynamicEntity*>closeEntityList);
	fMPoint DynamicEntity::GetCohesionSpeed(std::vector<DynamicEntity*>closeEntityList, fMPoint position);
	fMPoint DynamicEntity::GetSeparationSpeed(std::vector<DynamicEntity*>collidingEntityList, fMPoint position);
	SDL_Rect GetAnimationRect(float dt);

};

#endif//__DYNAMICENTITY_H__
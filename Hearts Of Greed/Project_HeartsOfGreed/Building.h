#pragma once

#ifndef __BUILDING_H__
#define __BUILDING_H__

#include "Entity.h"


class Core;
class UpgradeCenter;
class Turret;
class Barricade;
class Base;
class ParticleSystem;

enum class BUILDING_STATE : int
{
	ST_UNKNOWN = -1,

	ST_ALLY,
	ST_NEUTRAL,
	ST_ENEMY,

	ST_ALL,
};

enum class BUILDING_DECOR : int
{
	ST_UNKNOWN = -1,

	ST_01,
	ST_02,
	ST_03,

	STREETLIGHT,

	NONE,

	ST_ALL,
};

class Building : public Entity
{
public:

	Building(fMPoint position, int maxHitPoints, int currenthitPoints, int recoveryHitPointsRate, int xpOnDeath, int buildingCost, int transparency, Collider* collider,
		ENTITY_TYPE type = ENTITY_TYPE::BUILDING, BUILDING_DECOR decor = BUILDING_DECOR::NONE);

	Building(fMPoint position, Building* copy, ENTITY_ALIGNEMENT alignement);

	Building();

	virtual ~Building();

	void Destroy();

	//Destructor
	

	// Called each loop iteration
	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate(float dt);

	void OnCollision(Collider* collider);
	virtual void Draw(float dt);
	virtual void MinimapDraw(float scale, float halfWidth);

	void ActivateTransparency();
	BUILDING_DECOR GetDecor();

	void UnleashParticleSmoke();

	void HandleMyParticleSystem(float dt);

	void TimeMyparticleSystem(float dt);

private:
	void ChangeBuildingState(BUILDING_STATE state);

	void BeingRepaired();

	void Contruct();

	

public:

	BUILDING_STATE	currentState;
	BUILDING_DECOR myDecor;
	Base* myBase;
	SDL_Texture* selectedTexture;

protected:
	int			recoveryHitPointsRate;

	bool		transparent;
	int			transparencyValue;

	float		myParticleTimer;
	ParticleSystem* myParticleSystem;
	
private:
	
	int			xpOnDeath;
	int			buildingCost;
	

	bool		selected;

};


#endif // !

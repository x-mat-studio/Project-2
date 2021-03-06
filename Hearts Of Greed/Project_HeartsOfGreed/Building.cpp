#include "Building.h"
#include "Render.h"
#include "Textures.h"
#include "EntityManager.h"
#include "FoWManager.h"
#include "ParticleSystem.h"
#include "Brofiler/Brofiler/Brofiler.h"

Building::Building() :
	recoveryHitPointsRate(0),
	xpOnDeath(0),
	buildingCost(0),
	transparencyValue(0),
	myParticleTimer(0),

	myBase(nullptr),
	selectedTexture(nullptr),
	transparent(false),
	selected(false),
	myParticleSystem(nullptr),
	currentState(BUILDING_STATE::ST_UNKNOWN),
	myDecor(BUILDING_DECOR::NONE)
{}

Building::Building(fMPoint position, int maxHitPoints, int currentHitPoints, int recoveryHitPointsRate, int xpOnDeath, int buildingCost, int transparency, Collider* collider,
	ENTITY_TYPE type, BUILDING_DECOR decor) :

	Entity(position, type, ENTITY_ALIGNEMENT::NEUTRAL, collider, maxHitPoints, currentHitPoints),

	recoveryHitPointsRate(recoveryHitPointsRate),
	xpOnDeath(xpOnDeath),
	buildingCost(buildingCost),
	transparencyValue(transparency),
	myParticleTimer(0),

	myBase(nullptr),
	selectedTexture(nullptr),
	transparent(false),
	selected(false),
	myParticleSystem(nullptr),
	currentState(BUILDING_STATE::ST_UNKNOWN),
	myDecor(decor)
{
}


Building::Building(fMPoint position, Building* copy, ENTITY_ALIGNEMENT alignement) :

	Entity(position, copy->type, alignement, copy->collider, copy->hitPointsMax, copy->hitPointsCurrent),

	recoveryHitPointsRate(copy->recoveryHitPointsRate),
	xpOnDeath(copy->xpOnDeath),
	buildingCost(copy->buildingCost),
	transparencyValue(copy->transparencyValue),
	myDecor(copy->myDecor),
	selectedTexture(copy->selectedTexture),
	myParticleTimer(0),

	myBase(nullptr),
	transparent(false),
	selected(false),
	myParticleSystem(nullptr),
	currentState(BUILDING_STATE::ST_UNKNOWN)
{
	if (type == ENTITY_TYPE::BUILDING && visionEntity == nullptr)
	{
		visionEntity = app->fowManager->CreateFoWEntity(position, false, 1, 3);
	}
}



void Building::Destroy()
{
}


Building::~Building()
{
	myBase = nullptr;
	selectedTexture = nullptr;

	visionEntity = nullptr;
}


bool Building::PreUpdate(float dt)
{
	transparent = false;

	return true;
}


bool Building::Update(float dt)
{
	HandleMyParticleSystem(dt);
	return true;
}


bool Building::PostUpdate(float dt)
{
	return true;
}

void Building::OnCollision(Collider* collider)
{
	if (collider->type == COLLIDER_HERO || collider->type == COLLIDER_ENEMY)
	{
		transparent = true;
	}
}


void Building::ChangeBuildingState(BUILDING_STATE state)
{
	currentState = state;
}


void Building::BeingRepaired()
{
}



void Building::Contruct()
{
}

void Building::UnleashParticleSmoke()
{
	//building smoke

	if (myParticleSystem != nullptr)
	{
		//this should not be called since buildings dont move, but whatevs
		
		myParticleSystem->Activate();
		myParticleSystem->Move(this->position.x, this->position.y);
	}
	else {
		switch (this->type)
		{

		case ENTITY_TYPE::BLDG_TURRET:
			myParticleSystem = (ParticleSystem*)app->entityManager->AddParticleSystem(TYPE_PARTICLE_SYSTEM::BUILDING_SMOKE, this->position.x - 50, this->position.y - 60);
			break;

		default:
			myParticleSystem = (ParticleSystem*)app->entityManager->AddParticleSystem(TYPE_PARTICLE_SYSTEM::BUILDING_SMOKE, this->position.x, this->position.y);
			break;
		}
	}
}

void Building::HandleMyParticleSystem(float dt)
{
	if (myParticleSystem != nullptr) {

		if (myParticleSystem->IsActive()) {

			TimeMyparticleSystem(dt);
		}
	}
}

void Building::TimeMyparticleSystem(float dt)
{
	//implied that your system is not nullptr
	if (myParticleSystem->IsActive())
	{
		myParticleTimer += dt;

		if (myParticleTimer > 4) {
			myParticleTimer = 0;
			myParticleSystem->Desactivate();
		}
	}
}


void Building::Draw(float dt)
{
	BROFILER_CATEGORY("DRAW Static Buildings", Profiler::Color::DarkGoldenRod);

	fMPoint newPos = position + offset;
	
	if (selectedByPlayer)
	{
		if (transparent)
		{
			app->render->Blit(selectedTexture, newPos.x, newPos.y, nullptr, false, true, transparencyValue);
		}
		else
		{
			app->render->Blit(selectedTexture, newPos.x, newPos.y, nullptr, false, true);
		}
	}
	else
	{
		if (transparent)
		{
			app->render->Blit(texture, newPos.x, newPos.y, nullptr, false, true, transparencyValue);
		}
		else
		{
			app->render->Blit(texture, newPos.x, newPos.y, nullptr, false, true);
		}
	}
	
}

void Building::MinimapDraw(float scale, float halfWidth)
{
	float worldX = position.x + offset.x;
	float worldY = position.y + offset.y;

	worldX += app->render->currentCamX;
	worldY += app->render->currentCamY;

	SDL_Texture* auxTexture = nullptr;


	if (texture != nullptr)
	{
		app->render->MinimapBlit(texture, worldX + halfWidth, worldY, NULL, scale);
	}
	else
	{
		app->minimap->MinimapFoWNeedsUpdate();
	}

}

void Building::ActivateTransparency()
{
	transparent = true;
}

BUILDING_DECOR Building::GetDecor()
{
	return myDecor;
}

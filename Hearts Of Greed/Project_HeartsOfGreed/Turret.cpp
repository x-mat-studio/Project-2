#include "Turret.h"
#include "EntityManager.h"
#include "Render.h"
#include "Map.h"
#include "FoWManager.h"
#include "Base.h"
#include "EventManager.h"
#include "ParticleSystem.h"
#include "Pathfinding.h"


Turret::Turret(int turretLvl, int attackDmg, float attackSpeed, int range, int vision, fMPoint position, Collider* collider, Animation& idleRight, Animation& idleRightUp, Animation& idleRightDown, Animation& idleLeft,
	Animation& idleLeftUp, Animation& idleLeftDown, Animation& shootingRight, Animation& shootingRightUp, Animation& shootingRightDown, Animation& shootingLeft, Animation& shootingLeftUp,
	Animation& shootingLeftDown, int maxHitPoints, int currentHitPoints, int recoveryHitPointsRate, int xpOnDeath, int buildingCost, int transparency, float damageIncrease, int rangeIncrease, float speedIncrease, float hpIncrease) :

	Building(position, maxHitPoints, currentHitPoints, recoveryHitPointsRate, xpOnDeath, buildingCost, transparency, collider, ENTITY_TYPE::BLDG_TURRET),

	shootingLeft(shootingLeft),
	shootingLeftUp(shootingLeftUp),
	shootingLeftDown(shootingLeftDown),
	shootingRightUp(shootingRightUp),
	shootingRightDown(shootingRightDown),
	shootingRight(shootingRight),
	idleRight(idleRight),
	idleRightDown(idleRightDown),
	idleRightUp(idleRightUp),
	idleLeft(idleLeft),
	idleLeftUp(idleLeftUp),
	idleLeftDown(idleLeftDown),

	turretLvl(turretLvl),
	attackDmg(attackDmg),
	attackSpeed(attackSpeed),
	range(range),
	vision(vision),

	damageIncrease(damageIncrease),
	rangeIncrease(rangeIncrease),
	speedIncrease(speedIncrease),
	hpIncrease(hpIncrease),

	attackCD(0),
	bonusAttack(0),
	bonusArmor(0),
	temporalTimer(0),
	timeUntilDeath(0),

	shortTermObjective(nullptr),

	state(TURRET_STATES::IDLE),
	dir(FACE_DIR::SOUTH_EAST)
{
	currentAnimation = &this->idleRightDown;
}


Turret::Turret(fMPoint position, Turret* copy, ENTITY_ALIGNEMENT alignement) :

	Building(position, copy, alignement),


	shootingLeft(copy->shootingLeft),
	shootingLeftUp(copy->shootingLeftUp),
	shootingLeftDown(copy->shootingLeftDown),
	shootingRightUp(copy->shootingRightUp),
	shootingRightDown(copy->shootingRightDown),
	shootingRight(copy->shootingRight),
	idleRight(copy->idleRight),
	idleRightDown(copy->idleRightDown),
	idleRightUp(copy->idleRightUp),
	idleLeft(copy->idleLeft),
	idleLeftUp(copy->idleLeftUp),
	idleLeftDown(copy->idleLeftDown),

	turretLvl(copy->turretLvl),
	attackDmg(copy->attackDmg),
	attackSpeed(copy->attackSpeed),
	range(copy->range),
	vision(copy->vision),

	damageIncrease(copy->damageIncrease),
	rangeIncrease(copy->rangeIncrease),
	speedIncrease(copy->speedIncrease),
	hpIncrease(copy->hpIncrease),

	attackCD(0),
	bonusAttack(0),
	bonusArmor(0),
	temporalTimer(0),
	timeUntilDeath(0),

	shortTermObjective(nullptr),

	state(TURRET_STATES::IDLE),
	dir(copy->dir)
{
	currentAnimation = &idleRightDown;

	this->visionEntity = app->fowManager->CreateFoWEntity(this->position, true, vision);

	//app->pathfinding->SetWalkabilityMap(false, app->map->WorldToMap(position.x - 60, position.y - 10));
}


Turret::~Turret()
{
	shortTermObjective = nullptr;
	currentAnimation = nullptr;

	idleRight = Animation();
	idleRightUp = Animation();
	idleRightDown = Animation();
	idleLeft = Animation();
	idleLeftUp = Animation();
	idleLeftDown = Animation();
	shootingRight = Animation();
	shootingRightUp = Animation();
	shootingRightDown = Animation();
	shootingLeft = Animation();
	shootingLeftUp = Animation();
	shootingLeftDown = Animation();

	inputs.clear();
}


bool Turret::PreUpdate(float dt)
{
	transparent = false;
	return true;
}


bool Turret::Update(float dt)
{
	//check inputs to traverse state matrix
	ExternalInput(inputs, dt);
	InternalInput(inputs, dt);
	state = ProcessFsm(inputs);

	HandleMyParticleSystem(dt);
	StateMachine();
	ResetBonusStats();

	
	return true;
}


bool Turret::PostUpdate(float dt)
{
	if (app->debugMode)
	{
		//Position --------------------------------------
		app->render->DrawQuad({ (int)position.x, (int)position.y, 2,2 }, 255, 0, 0);

		fMPoint nextPoint = { 0,0 };
		iMPoint origin = app->map->WorldToMap(round(position.x), round(position.y));
		origin = app->map->MapToWorld(origin.x, origin.y);

		app->render->DrawQuad({ (int)origin.x, (int)origin.y, 10,10 }, 255, 255, 255, 125);

	}

	
	return true;
}

bool Turret::Start(SDL_Texture* texture)
{
	this->texture = texture;
	if (collider != nullptr)
	{
		collider = new Collider(collider->rect, collider->type, collider->callback, this);
		collider->thisEntity = this;
		app->coll->AddColliderEntity(collider);

		collider->SetPos(position.x, position.y);

		offset.x = -((float)collider->rect.w * 0.5f);

		offset.y = -((float)collider->rect.h * 0.66f);

		center.x = (float)collider->rect.w * 0.5f;
		center.y = (float)collider->rect.h * 0.5f;

		CollisionPosUpdate();
	}
	started = true;

	UnleashParticleSmoke();

	return true;
}


void Turret::CheckObjective(Entity* entity)
{
	if (shortTermObjective == entity)
	{
		shortTermObjective = nullptr;

		SearchObjective();
		inputs.push_back(TURRET_INPUTS::IN_IDLE);
	}
}


bool Turret::SearchObjective()
{
	bool ret = false;
	SDL_Rect rect;

	rect.x = position.x - range;
	rect.y = position.y - center.y - range;
	rect.w = range * 2;
	rect.h = range * 2;

	Entity* objective;
	objective = app->entityManager->SearchEntityRect(&rect, align);

	if (objective != nullptr && shortTermObjective != objective)
	{
		ret = true;
	}

	shortTermObjective = objective;

	return ret;
}

void Turret::Draw(float dt)
{
	if (transparent)
	{
		app->render->Blit(texture, position.x, position.y, &currentAnimation->GetCurrentFrameBox(dt), false, true, transparencyValue, 255, 255, 255, 1.0f, currentAnimation->GetCurrentFrame(dt).pivotPositionX - offset.x, -offset.y);
	}
	else
		app->render->Blit(texture, position.x, position.y, &currentAnimation->GetCurrentFrameBox(dt), false, true, 0, 255, 255, 255, 1.0f, currentAnimation->GetCurrentFrame(dt).pivotPositionX - offset.x, -offset.y);
}

int Turret::GetLvl()
{
	return turretLvl;
}

int Turret::GetAD()
{
	return attackDmg;
}

int Turret::GetAS()
{
	return attackSpeed;
}

int Turret::GetRng()
{
	return range;
}

void Turret::DrawSelected()
{
	if (selectedByPlayer == true)
		app->render->Blit(app->entityManager->selectedTexture, this->position.x - 5, this->position.y - this->collider->rect.h+15);
}

int Turret::RecieveDamage(float damage, bool ignoreArmor)
{
	if (bonusArmor > 0 && !ignoreArmor)
	{
		damage -= damage * bonusArmor * 0.01f;
	}

	if (hitPointsCurrent > 0)
	{
		hitPointsCurrent -= damage;

		int randomCounter = rand() % 10;

		if (randomCounter == 0)
			app->audio->PlayFx(app->entityManager->buildingGetsHit, 0, -1, this->GetMyLoudness(), this->GetMyDirection(), false);
		else if (randomCounter == 9)
			app->audio->PlayFx(app->entityManager->buildingGetsHit2, 0, -1, this->GetMyLoudness(), this->GetMyDirection(), false);

		if (hitPointsCurrent <= 0)
		{
			Die();
		}
	}

	return 0;
}


bool Turret::CheckAttackRange()
{
	if (shortTermObjective == nullptr)
	{
		inputs.push_back(TURRET_INPUTS::IN_IDLE);
		return false;
	}


	if (shortTermObjective->GetAlignment() == align)
	{
		shortTermObjective = nullptr;
		inputs.push_back(TURRET_INPUTS::IN_IDLE);
		return false;
	}

	SDL_Rect rect;
	rect.x = position.x - range;
	rect.y = position.y - center.y - range;
	rect.w = range * 2;
	rect.h = range * 2;


	if (shortTermObjective->GetCollider()->CheckCollision(rect))
	{
		return true;
	}
	else
	{
		inputs.push_back(TURRET_INPUTS::IN_OUT_OF_RANGE);
		return false;
	}
}


void Turret::Attack()
{
	if (shortTermObjective)
		shortTermObjective->RecieveDamage(attackDmg * (bonusAttack * 0.01f + 1));
}


void Turret::Die()
{
	app->eventManager->GenerateEvent(EVENT_ENUM::ENTITY_DEAD, EVENT_ENUM::NULL_EVENT);

	app->audio->PlayFx(app->entityManager->turretDeath, 0, -1, GetMyLoudness(), GetMyDirection());

	toDelete = true;

	if (minimapIcon != nullptr)
	{
		minimapIcon->toDelete = true;
		minimapIcon->minimapPos = nullptr;
	}

	if (visionEntity != nullptr)
	{
		visionEntity->deleteEntity = true;
		visionEntity = nullptr;
	}

	if (myBase != nullptr)
	{
		myBase->RemoveTurret(this);
	}

	if (myParticleSystem != nullptr) {
		
		myParticleSystem->Die();
		myParticleSystem = nullptr;
	}

	//app->pathfinding->SetWalkabilityMap(true, app->map->WorldToMap(position.x - 60, position.y - 10));

}


Entity* Turret::EnemyInRange()
{


	return nullptr;
}


void Turret::InternalInput(std::vector<TURRET_INPUTS>& inputs, float dt)
{
	if (attackCD > 0)
	{
		attackCD += dt;

		if (attackCD >= attackSpeed)
		{
			inputs.push_back(TURRET_INPUTS::IN_ATTACK_CHARGED);
			attackCD = 0;
		}
	}

	if (temporalTimer > 0)
	{
		temporalTimer += dt;

		if (temporalTimer >= timeUntilDeath)
		{
			Die();
		}
	}
}


bool Turret::ExternalInput(std::vector<TURRET_INPUTS>& inputs, float dt)
{
	if (CheckAttackRange())
	{
		inputs.push_back(TURRET_INPUTS::IN_ATTACK);
	}
	else
	{
		SearchObjective();
	}

	return true;
}


TURRET_STATES Turret::ProcessFsm(std::vector<TURRET_INPUTS>& inputs)
{
	TURRET_INPUTS lastInput;

	while (inputs.empty() == false)
	{
		lastInput = inputs.back();
		inputs.pop_back();

		switch (state)
		{

		case TURRET_STATES::IDLE:
		{
			switch (lastInput)
			{
			case TURRET_INPUTS::IN_ATTACK: state = TURRET_STATES::ATTACK;		break;

			case TURRET_INPUTS::IN_DEAD:   state = TURRET_STATES::DEAD;			break;
			}
		}	break;

		case TURRET_STATES::ATTACK:
		{
			switch (lastInput)
			{
			case TURRET_INPUTS::IN_CHARGING_ATTACK:		state = TURRET_STATES::CHARGING_ATTACK;	break;

			case TURRET_INPUTS::IN_OBJECTIVE_DONE:		state = TURRET_STATES::IDLE;			break;

			case TURRET_INPUTS::IN_OUT_OF_RANGE:		state = TURRET_STATES::IDLE;			break;

			case TURRET_INPUTS::IN_IDLE:				state = TURRET_STATES::IDLE;			break;

			case TURRET_INPUTS::IN_DEAD:			    state = TURRET_STATES::DEAD;			break;
			}
		}	break;


		case TURRET_STATES::CHARGING_ATTACK:
		{
			switch (lastInput)
			{
			case TURRET_INPUTS::IN_ATTACK_CHARGED:  state = TURRET_STATES::ATTACK;				break;

			case TURRET_INPUTS::IN_OBJECTIVE_DONE:  state = TURRET_STATES::IDLE;				break;

			case TURRET_INPUTS::IN_OUT_OF_RANGE:	state = TURRET_STATES::IDLE;				break;

			case TURRET_INPUTS::IN_IDLE:			state = TURRET_STATES::IDLE;				break;

			case TURRET_INPUTS::IN_DEAD:			state = TURRET_STATES::DEAD;				break;
			}
		}	break;


		case TURRET_STATES::DEAD:
		{
		}	break;

		}

	}

	return state;
}


void Turret::StateMachine()
{
	switch (state)
	{
	case TURRET_STATES::IDLE:

		break;

	case TURRET_STATES::ATTACK:

		if (shortTermObjective != nullptr)
			dir = DetermineDirection(shortTermObjective->position - position);

		if (attackCD == 0)
		{
			Attack();


			attackCD += 0.01f;
		}
		else
		{
			inputs.push_back(TURRET_INPUTS::IN_CHARGING_ATTACK);
		}

		break;

	case TURRET_STATES::CHARGING_ATTACK:

		app->audio->PlayFx(app->entityManager->turretShooting, 0, 5, this->GetMyLoudness(), this->GetMyDirection(),false);

		break;

	case TURRET_STATES::DEAD:
		Die();
		break;
	}

	SetAnimation(state);
}

FACE_DIR Turret::DetermineDirection(fMPoint faceDir)
{
	FACE_DIR newDir = dir;

	if (faceDir.x > 0)
	{
		if (faceDir.y < -0.1f)
		{
			newDir = FACE_DIR::NORTH_EAST;

		}
		else if (faceDir.y > 0.1f)
		{
			newDir = FACE_DIR::SOUTH_EAST;
		}
		else
		{
			newDir = FACE_DIR::EAST;
		}
	}
	else if (faceDir.x < 0)
	{
		if (faceDir.y < -0.1f)
		{
			newDir = FACE_DIR::NORTH_WEST;
		}
		else if (faceDir.y > 0.1f)
		{
			newDir = FACE_DIR::SOUTH_WEST;

		}
		else
		{
			newDir = FACE_DIR::WEST;
		}
	}


	return newDir;
}

void Turret::SetAnimation(TURRET_STATES state)
{
	switch (state)
	{
	case TURRET_STATES::IDLE:
	{
		switch (dir)
		{
		case FACE_DIR::NORTH_EAST:
			currentAnimation = &idleRightUp;
			break;
		case FACE_DIR::NORTH_WEST:
			currentAnimation = &idleLeftUp;
			break;
		case FACE_DIR::EAST:
			currentAnimation = &idleRight;
			break;
		case FACE_DIR::SOUTH_EAST:
			currentAnimation = &idleRightDown;
			break;
		case FACE_DIR::SOUTH_WEST:
			currentAnimation = &idleLeftDown;
			break;
		case FACE_DIR::WEST:
			currentAnimation = &idleLeft;
			break;
		}
	}
	break;
	case TURRET_STATES::ATTACK:
	{
		switch (dir)
		{
		case FACE_DIR::NORTH_EAST:
			currentAnimation = &shootingRightUp;
			break;
		case FACE_DIR::NORTH_WEST:
			currentAnimation = &shootingLeftUp;
			break;
		case FACE_DIR::EAST:
			currentAnimation = &shootingRight;
			break;
		case FACE_DIR::SOUTH_EAST:
			currentAnimation = &shootingRightDown;
			break;
		case FACE_DIR::SOUTH_WEST:
			currentAnimation = &shootingLeftDown;
			break;
		case FACE_DIR::WEST:
			currentAnimation = &shootingLeft;
			break;
		}
		break;
	}
	}
}


void Turret::ResetBonusStats()
{
	bonusAttack = 0.f;
	bonusArmor = 0.f;
}

void Turret::SetTemporalTimer(float time)
{
	if (time > 0.f)
	{
		temporalTimer += 0.01;
		timeUntilDeath = time;
	}
	else
	{
		temporalTimer = 0.f;
		timeUntilDeath = 0;
	}

}

void Turret::HandleMyParticleSystem(float dt)
{
	if (myParticleSystem != nullptr) {

		if (myParticleSystem->IsActive()) {

			TimeMyparticleSystem(dt);
		}
	}
}

void Turret::TimeMyparticleSystem(float dt)
{
	//implied that your system is not nullptr
	if (myParticleSystem->IsActive())
	{
		myParticleTimer += dt;

		if (myParticleTimer > 3) {
			myParticleTimer = 0;
			myParticleSystem->Desactivate();
		}
	}
}

void Turret::SetLevel(int lvl)
{
	for (int i = 1; i < lvl; i++)
	{
		LevelUp();
	}
}


void Turret::LevelUp()
{
	attackDmg = damageIncrease + (damageIncrease * log(turretLvl + 1) * (turretLvl + 1));
	range += rangeIncrease;

	attackSpeed -= speedIncrease;

	hitPointsMax += hpIncrease;
	hitPointsCurrent = hitPointsMax;

	turretLvl++;
}
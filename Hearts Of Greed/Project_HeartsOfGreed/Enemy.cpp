#include "Enemy.h"
#include "App.h"
#include "AI.h"
#include "EntityManager.h"
#include "EventManager.h"
#include "FoWManager.h"
#include "Audio.h"
#include "Textures.h"
#include "Render.h"
#include "Input.h"
#include "Map.h"
#include "Pathfinding.h"
#include "Brofiler/Brofiler/Brofiler.h"

Enemy::Enemy(fMPoint position, ENTITY_TYPE type, Collider* collider, Animation& walkLeft, Animation& walkLeftUp, Animation& walkLeftDown, Animation& walkRightUp,
	Animation& walkRightDown, Animation& walkRight, Animation& idleRight, Animation& idleRightUp, Animation& idleRightDown, Animation& idleLeft, Animation& idleLeftUp, Animation& idleLeftDown,
	Animation& punchLeft, Animation& punchLeftUp, Animation& punchLeftDown, Animation& punchRightUp, Animation& punchRightDown, Animation& punchRight,
	Animation& deathRight, Animation& deathRightUp, Animation& deathRightDown, Animation& deathLeft, Animation& deathLeftUp, Animation& deathLeftDown, int maxHitPoints, int currentHitPoints,
	int recoveryHitPointsRate, int vision, int attackDamage, float attackSpeed, int attackRange, int movementSpeed, int xpOnDeath, float scale) :

	DynamicEntity(position, movementSpeed, type, ENTITY_ALIGNEMENT::NEUTRAL, collider, maxHitPoints, currentHitPoints, 15, 25),
	walkLeft(walkLeft),
	walkLeftUp(walkLeftUp),
	walkLeftDown(walkLeftDown),
	walkRightUp(walkRightUp),
	walkRightDown(walkRightDown),
	walkRight(walkRight),
	idleRight(idleRight),
	idleRightDown(idleRightDown),
	idleRightUp(idleRightUp),
	idleLeft(idleLeft),
	idleLeftUp(idleLeftUp),
	idleLeftDown(idleLeftDown),
	punchLeft(punchLeft),
	punchLeftUp(punchLeftUp),
	punchLeftDown(punchLeftDown),
	punchRightUp(punchRightUp),
	punchRightDown(punchRightDown),
	punchRight(punchRight),
	deathRight(deathRight),
	deathRightDown(deathRightDown),
	deathRightUp(deathRightUp),
	deathLeft(deathLeft),
	deathLeftUp(deathLeftUp),
	deathLeftDown(deathLeftDown),

	recoveryHitPointsRate(recoveryHitPointsRate),
	vision(vision),
	attackDamage(attackDamage),
	attackSpeed(attackSpeed),
	attackRange(attackRange),
	attackCooldown(0),


	xpOnDeath(xpOnDeath),
	longTermObjective{ NULL, NULL },
	shortTermObjective(nullptr),
	damageTakenTimer(0.f),
	attackRangeInPX(0.f),
	haveOrders(false),
	drawingVFX(false),
	scale(scale),
	currentAnimation(nullptr),

	state(ENEMY_STATES::IDLE)
{}


Enemy::Enemy(fMPoint position, Enemy* copy, ENTITY_ALIGNEMENT align) :

	DynamicEntity(position, copy->unitSpeed, copy->type, align, copy->collider, copy->hitPointsMax, copy->hitPointsCurrent, copy->moveRange1, copy->moveRange2),
	walkLeft(copy->walkLeft),
	walkLeftUp(copy->walkLeftUp),
	walkLeftDown(copy->walkLeftDown),
	walkRightUp(copy->walkRightUp),
	walkRightDown(copy->walkRightDown),
	walkRight(copy->walkRight),
	idleRight(copy->idleRight),
	idleRightDown(copy->idleRightDown),
	idleRightUp(copy->idleRightUp),
	idleLeft(copy->idleLeft),
	idleLeftUp(copy->idleLeftUp),
	idleLeftDown(copy->idleLeftDown),
	punchLeft(copy->punchLeft),
	punchLeftUp(copy->punchLeftUp),
	punchLeftDown(copy->punchLeftDown),
	punchRightUp(copy->punchRightUp),
	punchRightDown(copy->punchRightDown),
	punchRight(copy->punchRight),
	deathRight(copy->deathRight),
	deathRightDown(copy->deathRightDown),
	deathRightUp(copy->deathRightUp),
	deathLeft(copy->deathLeft),
	deathLeftUp(copy->deathLeftUp),
	deathLeftDown(copy->deathLeftDown),

	recoveryHitPointsRate(copy->recoveryHitPointsRate),
	vision(copy->vision),
	attackDamage(copy->attackDamage),
	attackSpeed(copy->attackSpeed),
	attackRange(copy->attackRange),
	attackCooldown(0),

	xpOnDeath(copy->xpOnDeath),
	longTermObjective{ NULL, NULL },
	shortTermObjective(nullptr),
	damageTakenTimer(0.f),
	attackRangeInPX(0.f),

	scale(copy->scale),
	haveOrders(false),
	drawingVFX(false),

	state(ENEMY_STATES::IDLE)
{
	//FoW Related
	visionEntity = app->fowManager->CreateFoWEntity(position, false); //TODO this is going to be the enemy vision distance
	currentAnimation = &idleRightDown;

	float halfH = app->map->data.tileHeight * 0.5;
	float halfW = app->map->data.tileWidth * 0.5;
	attackRangeInPX = (sqrt(halfW * halfW + halfH * halfH) * attackRange + 0.5f * attackRange + halfH * 0.3f);

	debuffs.SetCallBack(this);
}


Enemy::~Enemy()
{
	shortTermObjective = nullptr;
	currentAnimation = nullptr;

	inputs.clear();

	walkLeft = Animation();
	walkLeftUp = Animation();
	walkLeftDown = Animation();
	walkRightUp = Animation();
	walkRightDown = Animation();
	walkRight = Animation();
	idleRight = Animation();
	idleRightDown = Animation();
	idleRightUp = Animation();
	idleLeft = Animation();
	idleLeftUp = Animation();
	idleLeftDown = Animation();
	punchLeft = Animation();
	punchLeftUp = Animation();
	punchLeftDown = Animation();
	punchRightUp = Animation();
	punchRightDown = Animation();
	punchRight = Animation();
}


bool Enemy::Update(float dt)
{
	BROFILER_CATEGORY("Enemy Update", Profiler::Color::Blue);

	//Check inputs to traverse state matrix

	debuffs.UpdateEffects(dt);

	if (debuffs.IsStuned() == true)
		return true;

	ExternalInput(inputs, dt);
	InternalInput(inputs, dt);
	state = ProcessFsm(inputs);

	StateMachine(dt);

	if (IsMoving() == true || state != ENEMY_STATES::IDLE)
		GroupMovement(dt);

	Roar();
	DrawOnSelect();
	CollisionPosUpdate();


	return true;
}


void Enemy::StateMachine(float dt)
{
	SetAnimation(state);

	switch (state)
	{
	case ENEMY_STATES::IDLE:
		break;

	case ENEMY_STATES::MOVE:

		if (Move(dt) == true)
		{
			if (shortTermObjective == nullptr && !haveOrders && this->path.empty())
			{
				inputs.push_back(ENEMY_INPUTS::IN_IDLE);
			}

		}
		else
			inputs.push_back(ENEMY_INPUTS::IN_IDLE);


		if (visionEntity != nullptr)
		{
			visionEntity->SetNewPosition(position);
		}
		break;

	case ENEMY_STATES::ATTACK:

		if (attackCooldown == 0)
		{
			if (CheckAttackRange() == true)
			{
				if (shortTermObjective != nullptr)
					dir = DetermineDirection(shortTermObjective->position - position);

				if (currentAnimation->GetCurrentFrameNum() >= currentAnimation->lastFrame * 0.5f)
				{
					if (Attack() == true)
					{
						if (this->type == ENTITY_TYPE::ENEMY_RANGED)
							drawingVFX = true;

						attackCooldown += 0.01f;
					}
				}

			}
			else
			{
				inputs.push_back(ENEMY_INPUTS::IN_OBJECTIVE_DONE);
			}

		}
		else if (currentAnimation->GetCurrentFrameNum() >= currentAnimation->lastFrame - 1)
		{
			inputs.push_back(ENEMY_INPUTS::IN_CHARGING_ATTACK);
		}
		break;

	case ENEMY_STATES::CHARGING_ATTACK:
		if (shortTermObjective != nullptr)
			dir = DetermineDirection(shortTermObjective->position - position);

		if (attackCooldown == 0)
		{
			inputs.push_back(ENEMY_INPUTS::IN_ATTACK);
		}
		break;

	case ENEMY_STATES::DEAD:
		toDelete = false;

		if (currentAnimation->GetCurrentFrameNum() >= currentAnimation->lastFrame - 1)
		{
			toDelete = true;
			app->eventManager->GenerateEvent(EVENT_ENUM::ENTITY_DEAD, EVENT_ENUM::NULL_EVENT);
		}
		break;
	}


}

void Enemy::Roar()
{
	//DEBUGSOUND
	int randomCounter = rand() % 2000;

	if (randomCounter == 997) {

		app->audio->PlayFx(app->entityManager->wanamingoRoar, 0, -1, this->GetMyLoudness(), this->GetMyDirection());

	}
	if (randomCounter == 998) {

		app->audio->PlayFx(app->entityManager->wanamingoRoar2, 0, -1, this->GetMyLoudness(), this->GetMyDirection());

	}
}

void Enemy::DrawOnSelect()
{
	if (selectedByPlayer || missionEntity)
		app->render->Blit(app->entityManager->targetedTexture, this->collider->rect.x + this->collider->rect.w / 2, this->collider->rect.y);
}

void Enemy::DrawVFX(float dt)
{
	return;
}


bool Enemy::PostUpdate(float dt)
{
	if (debuffs.IsStuned() == true)
		return true;

	GetAnimationCurrentFrame(dt);
	return true;
}


bool Enemy::MoveTo(float x, float y)
{
	if (GeneratePath(x, y, 1))
	{
		inputs.push_back(ENEMY_INPUTS::IN_MOVE);
		return true;
	}

	return false;
}


void Enemy::OnCollision(Collider* collider)
{
	if (collider->type == COLLIDER_RECLUIT_IA)
	{
		fMPoint* point = app->ai->GetObjective(position);
		if (point != nullptr)
		{
			longTermObjective = *point;
			haveOrders = true;
		}
	}
}


void Enemy::Draw(float dt)
{
	Frame currFrame = currentAnimation->GetCurrentFrame();

	if (damageTakenTimer > 0.f)
		app->render->Blit(texture, position.x, position.y, &currFrame.frame, false, true, 0, 255, 0, 0, scale, currFrame.pivotPositionX, currFrame.pivotPositionY/*, -currFrame.pivotPositionX, -currFrame.pivotPositionY*/);

	else
		app->render->Blit(texture, position.x, position.y, &currFrame.frame, false, true, 0, 255, 255, 255, scale, currFrame.pivotPositionX, currFrame.pivotPositionY/*, -currFrame.pivotPositionX, -currFrame.pivotPositionY*/);

	if (drawingVFX)
		DrawVFX(dt);

	DebugDraw();
}

Frame Enemy::GetAnimationCurrentFrame(float dt)
{
	Frame currFrame;

	if (state == ENEMY_STATES::ATTACK)
	{
		currFrame = currentAnimation->GetCurrentFrame(dt * attackSpeed);
	}
	else
		currFrame = currentAnimation->GetCurrentFrame(dt);

	return currFrame;
}

bool Enemy::Attack()
{
	if (shortTermObjective != nullptr)
	{
		if (shortTermObjective->GetAlignment() == align)
		{
			path.clear();
			DestroyPath();
			shortTermObjective = nullptr;
			SearchObjective();

			inputs.push_back(ENEMY_INPUTS::IN_OBJECTIVE_DONE);

			return false;
		}

		else
		{
			shortTermObjective->RecieveDamage(attackDamage);
			return true;
		}
	}

	return false;
}


void Enemy::Die()
{
	collider->thisEntity = nullptr;

	int randomCounter = rand() % 2;

	if (randomCounter == 1)
	{
		app->audio->PlayFx(app->entityManager->wanamingoDies, 0, -1, this->GetMyLoudness(), this->GetMyDirection());
	}
	else
	{
		app->audio->PlayFx(app->entityManager->wanamingoDies2, 0, -1, this->GetMyLoudness(), this->GetMyDirection());
	}

	if (minimapIcon != nullptr)
	{
		minimapIcon->toDelete = true;
		minimapIcon->minimapPos = nullptr;
		minimapIcon->parent = nullptr;
		minimapIcon = nullptr;
	}

	if (visionEntity != nullptr)
	{
		visionEntity->deleteEntity = true;
		visionEntity = nullptr;
	}

	inputs.push_back(ENEMY_INPUTS::IN_DEAD);
}


void Enemy::CheckObjective(Entity* entity)
{
	if (shortTermObjective == entity)
	{
		path.clear();
		DestroyPath();
		shortTermObjective = nullptr;
		SearchObjective();

		inputs.push_back(ENEMY_INPUTS::IN_IDLE);
	}
}

void Enemy::SearchForNewObjective()
{
	shortTermObjective = app->entityManager->SearchUnitsInRange(vision, this);
}

void Enemy::RecoverHealth()
{}


bool Enemy::SearchObjective()
{
	bool ret = false;

	SDL_Rect rect;

	rect.x = position.x - vision;
	rect.y = position.y - center.y - vision;
	rect.w = vision * 2;
	rect.h = vision * 2;

	Entity* objective;
	objective = app->entityManager->SearchEntityRect(&rect, align);


	if (objective != nullptr)
	{
		ret = true;
	}

	shortTermObjective = objective;

	return ret;
}


bool Enemy::CheckAttackRange()
{
	if (shortTermObjective == nullptr)
	{
		return false;
	}


	if (shortTermObjective->GetAlignment() == align)
	{
		shortTermObjective = nullptr;
		return false;
	}


	iMPoint myPos = app->map->WorldToMap(position.x, position.y);

	fMPoint objPosW = shortTermObjective->GetPosition();
	iMPoint objPosM = app->map->WorldToMap(objPosW.x, objPosW.y);

	if (app->pathfinding->CreateLine(myPos, objPosM).size() - 1 < attackRange + shortTermObjective->GetRadiusSize())
	{
		return true;

	}
	else if (app->pathfinding->GetLastLine()->size() - 1 <= attackRange + shortTermObjective->GetRadiusSize()
		&& position.DiagonalDistance(shortTermObjective->GetPosition()) <= attackRangeInPX)
	{
		return true;
	}

	else
	{
		inputs.push_back(ENEMY_INPUTS::IN_OUT_OF_RANGE);
		return false;
	}
}


void Enemy::InternalInput(std::vector<ENEMY_INPUTS>& inputs, float dt)
{
	if (attackCooldown > 0)
	{
		attackCooldown += dt;

		if (attackCooldown > (1 / attackSpeed))
		{
			inputs.push_back(ENEMY_INPUTS::IN_ATTACK_CHARGED);
			attackCooldown = 0;
		}

	}

	if (damageTakenTimer > 0.f)
	{
		damageTakenTimer -= dt;

		if (damageTakenTimer <= 0.f)
			damageTakenTimer = 0.f;
	}
}


bool Enemy::ExternalInput(std::vector<ENEMY_INPUTS>& inputs, float dt)
{
	if (CheckAttackRange())
	{
		inputs.push_back(ENEMY_INPUTS::IN_ATTACK);
	}

	else
	{

		if (SearchObjective() == true)
		{
			MoveTo(shortTermObjective->GetPosition().x, shortTermObjective->GetPosition().y);
		}

		else if (haveOrders)
		{
			MoveTo(longTermObjective.x, longTermObjective.y);
		}

	}

	return true;
}


ENEMY_STATES Enemy::ProcessFsm(std::vector<ENEMY_INPUTS>& inputs)
{
	ENEMY_INPUTS lastInput;

	while (inputs.empty() == false)
	{
		lastInput = inputs.back();
		inputs.pop_back();

		switch (state)
		{

		case ENEMY_STATES::IDLE:
		{
			switch (lastInput)
			{
			case ENEMY_INPUTS::IN_MOVE:   state = ENEMY_STATES::MOVE;		break;

			case ENEMY_INPUTS::IN_ATTACK: state = ENEMY_STATES::ATTACK;		break;

			case ENEMY_INPUTS::IN_DEAD:   state = ENEMY_STATES::DEAD;		break;
			}
		}	break;


		case ENEMY_STATES::MOVE:
		{
			switch (lastInput)
			{
			case ENEMY_INPUTS::IN_IDLE:   state = ENEMY_STATES::IDLE;		break;

			case ENEMY_INPUTS::IN_ATTACK: state = ENEMY_STATES::ATTACK;		break;

			case ENEMY_INPUTS::IN_DEAD:   state = ENEMY_STATES::DEAD;		break;
			}
		}	break;


		case ENEMY_STATES::ATTACK:
		{
			switch (lastInput)
			{
			case ENEMY_INPUTS::IN_CHARGING_ATTACK: state = ENEMY_STATES::CHARGING_ATTACK;	break;

			case ENEMY_INPUTS::IN_OBJECTIVE_DONE:  state = ENEMY_STATES::IDLE;				break;

			case ENEMY_INPUTS::IN_OUT_OF_RANGE:    state = ENEMY_STATES::MOVE;				break;

			case ENEMY_INPUTS::IN_DEAD:			   state = ENEMY_STATES::DEAD;				break;
			}
		}	break;


		case ENEMY_STATES::CHARGING_ATTACK:
		{
			switch (lastInput)
			{
			case ENEMY_INPUTS::IN_ATTACK_CHARGED: state = ENEMY_STATES::ATTACK;				break;

			case ENEMY_INPUTS::IN_OBJECTIVE_DONE:  state = ENEMY_STATES::IDLE;				break;

			case ENEMY_INPUTS::IN_OUT_OF_RANGE:    state = ENEMY_STATES::MOVE;				break;

			case ENEMY_INPUTS::IN_MOVE:			   state = ENEMY_STATES::MOVE;				break;

			case ENEMY_INPUTS::IN_DEAD:			   state = ENEMY_STATES::DEAD;				break;
			}
		}	break;


		case ENEMY_STATES::DEAD:
		{
		}	break;

		}

	}

	return state;

}

int Enemy::RecieveDamage(float damage, bool ignoreArmor)
{
	int ret = -1;

	if (hitPointsCurrent > 0)
	{
		hitPointsCurrent -= damage;
		damageTakenTimer = 0.3f;

		if (hitPointsCurrent <= 0)
		{
			Die();
			ret = xpOnDeath;
		}
		else
			app->audio->PlayFx(app->entityManager->wanamingoGetsHit, 0, -1, this->GetMyLoudness(), this->GetMyDirection(), true);
	}

	return ret;
}


void Enemy::SetAnimation(ENEMY_STATES state)
{
	switch (state)
	{
	case ENEMY_STATES::MOVE:
	{
		switch (dir)
		{
		case FACE_DIR::NORTH_EAST:
			currentAnimation = &walkRightUp;
			break;
		case FACE_DIR::NORTH_WEST:
			currentAnimation = &walkLeftUp;
			break;
		case FACE_DIR::EAST:
			currentAnimation = &walkRight;
			break;
		case FACE_DIR::SOUTH_EAST:
			currentAnimation = &walkRightDown;
			break;
		case FACE_DIR::SOUTH_WEST:
			currentAnimation = &walkLeftDown;
			break;
		case FACE_DIR::WEST:
			currentAnimation = &walkLeft;
			break;
		}
	}
	break;
	case ENEMY_STATES::IDLE:
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
		break;
	}

	case ENEMY_STATES::CHARGING_ATTACK:
	case ENEMY_STATES::ATTACK:
	{
		switch (dir)
		{
		case FACE_DIR::NORTH_EAST:
			currentAnimation = &punchRightUp;
			break;
		case FACE_DIR::NORTH_WEST:
			currentAnimation = &punchLeftUp;
			break;
		case FACE_DIR::EAST:
			currentAnimation = &punchRight;
			break;
		case FACE_DIR::SOUTH_EAST:
			currentAnimation = &punchRightDown;
			break;
		case FACE_DIR::SOUTH_WEST:
			currentAnimation = &punchLeftDown;
			break;
		case FACE_DIR::WEST:
			currentAnimation = &punchLeft;
			break;
		}
		break;
	}

	case ENEMY_STATES::DEAD:
	{
		switch (dir)
		{
		case FACE_DIR::NORTH_EAST:
			currentAnimation = &deathRight;
			break;
		case FACE_DIR::NORTH_WEST:
			currentAnimation = &deathLeftUp;
			break;
		case FACE_DIR::EAST:
			currentAnimation = &deathRight;
			break;
		case FACE_DIR::SOUTH_EAST:
			currentAnimation = &deathRightDown;
			break;
		case FACE_DIR::SOUTH_WEST:
			currentAnimation = &deathLeftDown;
			break;
		case FACE_DIR::WEST:
			currentAnimation = &deathLeft;
			break;

		}

		currentAnimation->loop = false;

		break;
	}

	}
}


int Enemy::GetHP()
{
	return hitPointsCurrent;
}

int Enemy::GetAD()
{
	return attackDamage;
}

float Enemy::GetAS()
{
	return attackSpeed;
}

int Enemy::GetVision()
{
	return vision;
}

int Enemy::GetRecov()
{
	return recoveryHitPointsRate;
}


int Enemy::GetLongTermObjectiveX()
{
	return longTermObjective.x;
}


int Enemy::GetLongTermObjectiveY()
{
	return longTermObjective.y;
}


//This is only used when we load a game, do not use it anywhere else
void Enemy::SetLongTermObjective(fMPoint point)
{
	if (point.x != 0 && point.y != 0)
	{
		haveOrders = true;
		longTermObjective = point;
	}
}
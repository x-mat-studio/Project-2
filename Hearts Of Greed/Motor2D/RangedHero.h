#ifndef __RANGED_HERO_H__
#define __RANGED_HERO_H__

#include "Hero.h"

class RangedHero : public Hero
{
public:
	RangedHero(fMPoint position, Collider* col, Animation& walkLeft, Animation& walkLeftUp, Animation& walkLeftDown, Animation& walkRightUp,
		Animation& walkRightDown, Animation& walkRight, Animation& idleRight, Animation& idleRightDown, Animation& idleRightUp, Animation& idleLeft,
		Animation& idleLeftUp, Animation& idleLeftDown, int level, int hitPoints, int recoveryHitPointsRate, int energyPoints, int recoveryEnergyRate,
		int attackDamage, int attackSpeed, int attackRange, int movementSpeed, int vision, float attackCooldown, float skill1ExecutionTime,
		float skill2ExecutionTime, float skill3ExecutionTime, float skill1RecoverTime, float skill2RecoverTime, float skill3RecoverTime);

	RangedHero(fMPoint position, RangedHero* copy, ENTITY_ALIGNEMENT alignement);

	bool UseHability1();
	bool UseHability2();
	bool UseHability3();
};


#endif //__RANGED_HERO_H__
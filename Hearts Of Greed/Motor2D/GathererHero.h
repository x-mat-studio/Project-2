#ifndef __GATHERER_HERO_H__
#define __GATHERER_HERO_H__

#include "Hero.h"

class GathererHero : public Hero
{
public:
	GathererHero(fMPoint position, Collider* col, Animation& walkLeft, Animation& walkLeftUp, Animation& walkLeftDown, Animation& walkRightUp,
		Animation& walkRightDown, Animation& walkRight, Animation& idleRight, Animation& idleRightDown, Animation& idleRightUp, Animation& idleLeft,
		Animation& idleLeftUp, Animation& idleLeftDown, Animation& punchLeft, Animation& punchLeftUp, Animation& punchLeftDown, Animation& punchRightUp,
		Animation& punchRightDown, Animation& punchRight, Animation& skill1Right, Animation& skill1RightUp, Animation& skill1RightDown, Animation& skill1Left,
		Animation& skill1LeftUp, Animation& skill1LeftDown, Animation& tileOnWalk, 
		Animation& deathRight, Animation& deathRightUp, Animation& deathRightDown, Animation& deathLeft, Animation& deathLeftUp, Animation& deathLeftDown, 
		int level, int maxHitPoints, int currentHitPoints, int recoveryHitPointsRate, int maxEnergyPoints, int energyPoints, int recoveryEnergyRate,
		int attackDamage, float attackSpeed, int attackRange, int movementSpeed, int vision, Skill& skill1, Animation& vfxExplosion,
		int hpLevelUp, int damageLevelUp, int energyLevelUp, int atkSpeedLevelUp);

	GathererHero(fMPoint position, GathererHero* copy, ENTITY_ALIGNEMENT alignement);
	~GathererHero();

	bool ActivateSkill1(fMPoint mouseClick);
	bool ActivateSkill2();
	bool ActivateSkill3();

	bool PreProcessSkill1();
	bool PreProcessSkill2();
	bool PreProcessSkill3();

	bool ExecuteSkill1();
	bool ExecuteSkill2();
	bool ExecuteSkill3();

	void LevelUp();

	void PlayGenericNoise(int random);

private:
	void BlitCommandVfx(Frame& currframe, int alphaValue);

private:

	fMPoint granadePosLaunch;
	skillArea* granadeArea;

	Animation vfxExplosion;

	//This is a placeholder for particles
	SDL_Rect explosionRect;
	Animation* currentVfx;

	bool DrawVfx(float dt);
};


#endif //__GATHERER_HERO_H__

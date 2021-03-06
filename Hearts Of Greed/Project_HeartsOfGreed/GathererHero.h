#ifndef __GATHERER_HERO_H__
#define __GATHERER_HERO_H__

#include "Hero.h"

class Turret;

class GathererHero : public Hero
{
public:
	GathererHero(fMPoint position, Collider* col, Animation& walkLeft, Animation& walkLeftUp, Animation& walkLeftDown, Animation& walkRightUp,
		Animation& walkRightDown, Animation& walkRight, Animation& idleRight, Animation& idleRightDown, Animation& idleRightUp, Animation& idleLeft,
		Animation& idleLeftUp, Animation& idleLeftDown, Animation& punchLeft, Animation& punchLeftUp, Animation& punchLeftDown, Animation& punchRightUp,
		Animation& punchRightDown, Animation& punchRight, Animation& skill1Right, Animation& skill1RightUp, Animation& skill1RightDown, Animation& skill1Left,
		Animation& skill1LeftUp, Animation& skill1LeftDown, Animation& tileOnWalk, 
		Animation& deathRight, Animation& deathRightUp, Animation& deathRightDown, Animation& deathLeft, Animation& deathLeftUp, Animation& deathLeftDown, 
		HeroStats& stats, Skill& skill1, Skill& passiveSkill);

	GathererHero(fMPoint position, GathererHero* copy, ENTITY_ALIGNEMENT alignement);
	~GathererHero();

	bool Start(SDL_Texture* texture);

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
	void OnCollision(Collider* collider);

	void PlayGenericNoise(int random);

	Skill GetPassiveSkill() const;
	void ReplacePassiveSkill(Skill& skill);

	void CheckObjective(Entity* deleted);


private:
	void BlitCommandVfx(Frame& currframe, int alphaValue);
	void UnleashParticlesSkill1(float posx, float posy);
	void UpdatePasiveSkill(float dt);
	void Attack();

private:

	fMPoint skillPosLaunch;
	skillArea* granadeArea;


	Skill passiveSkill;
	Collider* passiveSkillCollider;

	Turret* myTurret;

	
};


#endif //__GATHERER_HERO_H__

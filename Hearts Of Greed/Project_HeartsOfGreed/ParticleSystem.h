#ifndef __PARTICLE_SYSTEM_H__
#define __PARTICLE_SYSTEM_H__

#include "Entity.h"
#include "vector"
#include "p2Point.h"

class Emitter;

enum class TYPE_PARTICLE_SYSTEM : int
{
	NONE = -1,

	SKILL_GATHERER,
	SKILL_RANGED,
	SKILL_MELEE,
	SKILL_ROBOT,

	BUILDING_SMOKE,

	MAX
};

class ParticleSystem : public Entity
{
public:
	ParticleSystem();
	ParticleSystem(float x, float y, ParticleSystem* copy, bool active);

	~ParticleSystem();

	void PushEmiter(Emitter& emitter);

	bool Update(float dt);
	bool PostUpdate(float dt);

	void Desactivate();
	void Activate();

	bool IsActive();

	void Move(int x, int y);

	void Die();

private:
	std::vector<Emitter> emitterVector;

	bool active;
};



#endif // __PARTICLE_SYSTEM_H__
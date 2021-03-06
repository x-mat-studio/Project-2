#ifndef __ENTITYMANAGER_H__
#define __ENTITYMANAGER_H__

#include "SDL/include/SDL.h"
#include "Module.h"
#include "Animation.h"
#include "Entity.h"
#include <list>
#include <map>

class DeadHero;
class Hero;
class GathererHero;
class MeleeHero;
class RangedHero;
class RoboHero;

class Building;
class DynamicEntity;
class Enemy;
class RangedEnemy;
class GigaEnemy;
class NightEnemy;
class Spawner;

class Base;
class Turret;
class Barricade;
class UpgradeCenter;

class ParticleSystem;
class Emitter;

class Quest;

enum class BUILDING_DECOR;
enum class TYPE_PARTICLE_SYSTEM;

struct Skill;

struct skillArea
{
	unsigned short* area = nullptr;
	int users = 0;
};

enum class SPRITE_POSITION : int
{
	INVALID = -1,

	BEHIND_BUILDING,
	HIGHER_THAN_BUILDING,
	LOWER_THAN_BUILDING,
	NULL_BUILDING,
	NULL_MOVABLE_ENTITY,
	BOTH_NULL
};

struct HeroStats;

class ModuleEntityManager : public Module
{
public:

	ModuleEntityManager();
	~ModuleEntityManager();


	bool Awake(pugi::xml_node&);
	bool Start();


	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate(float dt);


	bool CleanUp();


	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;


	void OnCollision(Collider*, Collider*);


	Entity* AddEntity(ENTITY_TYPE type, int x, int y, ENTITY_ALIGNEMENT alignement = ENTITY_ALIGNEMENT::NEUTRAL);
	Entity* AddDecorativeBuilding(BUILDING_DECOR decor, int x, int y);
	Entity* AddParticleSystem(TYPE_PARTICLE_SYSTEM type, int x, int y);
	Quest* SearchQuestByID(int id);

	Entity* GetSample(ENTITY_TYPE);


	void RemoveDeletedEntities();
	void DeleteAllEntities();


	Entity* CheckEntityOnClick(iMPoint mousePos, bool focus = true, ENTITY_ALIGNEMENT alignement = ENTITY_ALIGNEMENT::PLAYER);
	Entity* CheckEntityOnClickbyPriority(iMPoint mousePos);
	Entity* CheckEntityOnClickbyPriorityandAlignment(iMPoint mousePos, bool focus = true, ENTITY_ALIGNEMENT alignement = ENTITY_ALIGNEMENT::PLAYER);

	void CheckHeroOnSelection(SDL_Rect& selection, std::vector<Hero*>* heroVector);
	void CheckDynamicEntitysObjectives(Entity* entity);


	bool CheckIfHeroIsDead(ENTITY_TYPE heroType)const;
	DeadHero* AssignNewDeadHero(Hero& dyingHero);
	DeadHero* AssignNewDeadHero(int level, ENTITY_TYPE type, Skill& skill, Skill& passiveSkill);
	void DeleteDeadHero(ENTITY_TYPE heroType);
	void DeleteAllDeadHeroes();

	void SaveDeadHero(pugi::xml_node& deadHeroesNode, ENTITY_TYPE heroType)const;
	void LoadDeadHero(pugi::xml_node& deadHeroesNode, ENTITY_TYPE heroType);


	void SearchHeroesAlive();
	void SearchEnemiesAlive();


	Entity* SearchEntityRect(SDL_Rect* rect, ENTITY_ALIGNEMENT alig);


	void GetEntityNeighbours(std::vector<DynamicEntity*>* close_entity_list, std::vector<DynamicEntity*>* colliding_entity_list, DynamicEntity* thisUnit);


	void PlayerBuildPreview(int x, int y, ENTITY_TYPE type);

	Hero* CheckUIAssigned(int& anotherHeroWithoutUI);

	Entity* SearchUnitsInRange(float distance, Entity* from);


	void ActivateGodModeHeroes();
	void DesactivateGodModeHeroes();

	void KillAllEnemies();

	skillArea* GenerateNewArea(int radius);

	//This & skill Struct need re-work to accept single target
	int ExecuteSkill(Skill& skillExecution, iMPoint pivot, Entity* objective = nullptr);

	//function used for minimap
	void DrawOnlyStaticBuildings();

	void ResetEntityManager();

	Entity* SearchEntity(ENTITY_TYPE type);

	void ResetUpgradeValues();

	ENTITY_TYPE GetFirstHeroType();

	//Area----
	skillArea* RequestAreaInfo(int radius);
	void CreateDynamicArea(std::vector <iMPoint>* toFill, int area, iMPoint center, skillArea* skillArea = nullptr);

	//Retuns false if it fails to load the skill
	bool RequestSkill(Skill& skillToFill, SKILL_ID id, int lvl = 1);
	bool RequestHeroStats(HeroStats& heroStats, ENTITY_TYPE id, int lvl = 1);

	//Revive
	bool ReviveHero(DeadHero heroToRevive);

	bool SaveQuest(pugi::xml_node& node, int id);

private:

	void CheckIfStarted();

	void ExecuteEvent(EVENT_ENUM eventId);

	//Sprite ordering
	void SpriteOrdering(float dt);
	void EntityQuickSort(std::vector<Entity*>& vector, int low, int high);
	int EntityPartition(std::vector<Entity*>& vector, int low, int high);
	SPRITE_POSITION CheckSpriteHeight(Entity* movEntity, Entity* building) const;

	//Area----
	unsigned short* BuildCircleArea(int radius);


	bool LoadSampleHero(ENTITY_TYPE heroType, pugi::xml_node& heroNode, pugi::xml_node& config);
	bool LoadSampleEnemy(pugi::xml_node& enemyNode, ENTITY_TYPE enemyType);

	bool LoadSampleTurret(pugi::xml_node& turretNode);
	bool LoadSampleBarricade(pugi::xml_node& barricadeNode);
	bool LoadSampleUpgradeCenter(pugi::xml_node& upgradeCenterNode);

	bool LoadSampleSpawner(pugi::xml_node& spawnerNode);
	bool LoadSampleBuilding(pugi::xml_node& buildingNode);
	bool LoadSampleStreetLight(pugi::xml_node& streetLightNode);
	bool LoadSampleBase(pugi::xml_node& baseNode);

	bool LoadSampleParticleSystemsAndEmitters(pugi::xml_node& particleSystemsNode);
	bool LoadSkillAreas(pugi::xml_node& areasNode);

	int CheckPlayerBases();


public:

	SDL_Texture* selectedTexture;
	SDL_Texture* targetedTexture;
	SDL_Texture* debugPathTexture;

	SDL_Texture* moveCommandTileRng;
	SDL_Texture* moveCommandTileGath;
	SDL_Texture* moveCommandTileMelee;
	SDL_Texture* moveCommandTileRobot;

	


	//Sounds
	int wanamingoRoar;
	int wanamingoRoar2;
	int wanamingoGetsHit;
	int wanamingoDies;
	int wanamingoDies2;

	int suitmanGetsHit;
	int suitmanGetsHit2;
	int suitmanGetsDeath;
	int suitmanGetsDeath2;

	int rangedGetsHit;
	int rangedDies;

	int buildingGetsHit;
	int buildingGetsHit2;

	int turretShooting;

	int suitman1Skill;

	int ranged1Skill;
	int ranged1Skil2;

	int suitman1Skill2;
	int armored1Skill2;

	int noise1Suitman;
	int noise2Suitman;
	int noise3Suitman;
	int noise4Suitman;

	int noise1Armored;
	int noise2Armored;
	int noise3Armored;
	int noise4Armored;

	int noise1Ranged;
	int noise2Ranged;
	int noise3Ranged;
	int noise4Ranged;

	int roboDying;

	int lvlup;
	int selectHero;
	int moveHero;
	int placingTurret;
	int placingCenter;
	int reviveHero;

	int turretDeath;


	//public textures
	SDL_Texture* rangedW_VFX;

	SDL_Texture* base2Texture;
	SDL_Texture* base2TextureEnemy;
	SDL_Texture* base2TextureSelected;
	SDL_Texture* base2TextureSelectedEnemy;

	SDL_Texture* upgradeCenterPlayerTexture;
	SDL_Texture* upgradeCenterPlayerSelectedTexture;
	SDL_Texture* upgradeCenterEnemyTexture;
	SDL_Texture* upgradeCenterEnemySelectedTexture;

	// Upgrades multipliers
	float gathererLifeUpgradeValue;
	float gathererDamageUpgradeValue;
	float gathererEnergyUpgradeValue;
	float gathererAtkSpeedUpgradeValue;
	float meleeLifeUpgradeValue;
	float meleeDamageUpgradeValue;
	float meleeEnergyUpgradeValue;
	float meleeAtkSpeedUpgradeValue;
	float rangedLifeUpgradeValue;
	float rangedDamageUpgradeValue;
	float rangedEnergyUpgradeValue;
	float rangedAtkSpeedUpgradeValue;
	float robottoLifeUpgradeValue;
	float robottoDamageUpgradeValue;
	float robottoEnergyUpgradeValue;
	float robottoAtkSpeedUpgradeValue;
	const float upgradeValue;

private:

	//The list where we will store all the entities
	std::vector <Entity*> entityVector;

	// Sprite sorting vectors
	std::vector <Entity*> renderVector;
	std::vector <Entity*> movableEntityVector;
	std::vector <Entity*> buildingVector;
	std::vector <Entity*> selectedVector;

	SDL_Texture* suitManTexture;
	SDL_Texture* armorMaleTexture;
	SDL_Texture* combatFemaleTexture;
	SDL_Texture* roboTexture;

	SDL_Texture* buildingTexture;

	SDL_Texture* base1Texture;

	SDL_Texture* deco1Selected;
	SDL_Texture* deco3Selected;
	SDL_Texture* streetLightTexture;

	SDL_Texture* turretTexture;
	SDL_Texture* barricadeTexture;

	SDL_Texture* enemyTexture;
	SDL_Texture* enemyNightTexture;
	SDL_Texture* enemyGigaTexture;
	SDL_Texture* enemyRangedTexture;

	//Samples

	GathererHero* sampleGatherer;
	MeleeHero* sampleMelee;
	RangedHero* sampleRanged;
	RoboHero* sampleRobo;

	Enemy* sampleEnemy;
	NightEnemy* sampleEnemyNight;
	GigaEnemy* sampleEnemyGiga;
	RangedEnemy* sampleEnemyRanged;

	Spawner* sampleSpawner;

	Building* sampleBuilding;
	SDL_Rect colliderBuilding1;
	Building* sampleStreetLight;
	Base* sampleBase;

	Turret* sampleTurret;
	Barricade* sampleBarricade;
	UpgradeCenter* sampleUpgradeCenter;


	//Particle System Samples

	ParticleSystem* sampleParticleSystem;
	ParticleSystem* sampleGathererSkill;
	ParticleSystem* sampleRangedSkill;
	ParticleSystem* sampleMeleeSkill;
	ParticleSystem* sampleRobotSkill;
	ParticleSystem* sampleBuildingSmoke;

	Emitter* sampleEmitter; Emitter* sampleEmitter2; 	Emitter* sampleEmitter3; 	Emitter* sampleEmitter4;

	Emitter* sampleEmitterSkillGatherer1; Emitter* sampleEmitterSkillGatherer2; Emitter* sampleEmitterSkillGatherer3; 
	Emitter* sampleEmitterSkillGatherer4; Emitter* sampleEmitterSkillGatherer5; Emitter* sampleEmitterSkillGatherer6;

	Emitter* sampleEmitterSkillRanged1; Emitter* sampleEmitterSkillRanged2; Emitter* sampleEmitterSkillRanged3;
	Emitter* sampleEmitterSkillRanged4; Emitter* sampleEmitterSkillRanged5; Emitter* sampleEmitterSkillRanged6;

	Emitter* sampleSmoke1; Emitter* sampleSmoke2; Emitter* sampleSmoke3;
	Emitter* sampleSmoke4; Emitter* sampleSmoke5; Emitter* sampleSmoke6;

	Emitter* sampleMeleeSkill1; Emitter* sampleMeleeSkill2; Emitter* sampleMeleeSkill3;
	Emitter* sampleMeleeSkill4; Emitter* sampleMeleeSkill5; Emitter* sampleMeleeSkill6;

	SDL_Texture* snowball;

	SDL_Texture* explosionTexture2;
	SDL_Texture* spiderweb;
	SDL_Texture* smoke1;
	SDL_Texture* smoke2;
	SDL_Texture* smoke3;
	SDL_Texture* hit1;
	SDL_Texture* hit2;
	SDL_Texture* hit3;

	std::map <int, skillArea> skillAreas;

	DeadHero* deadMelee;
	DeadHero* deadGatherer;
	DeadHero* deadRanged;
	DeadHero* deadRobo;

	P2SString skillFileName;
	P2SString suitmanFileName;
	P2SString armoredFileName;
	P2SString rangedFileName;
	P2SString roboFileName;
};

#endif //__ENTITYMANAGER_H__
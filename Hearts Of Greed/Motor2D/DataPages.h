#ifndef __DATA_PAGES_H__
#define __DATA_PAGES_H__

#include "UI.h"

class Entity;
class UIFactory;

enum class DATA_PAGE_ENUM
{
	FOCUSED_NONE,

	FOCUSED_BARRICADE,
	FOCUSED_BASE,
	FOCUSED_TURRET,
	FOCUSED_UPGRADE_CENTER,

	FOCUSED_GATHERER,
	FOCUSED_MELEE,
	FOCUSED_RANGED,

	FOCUSED_WANAMINGO,

	FOCUSED_UNKNOWN,
};

class DataPages : public UI
{
public:

	DataPages(float x, float y, UI* parent, Entity* entity);
	~DataPages();


private:

	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate(float dt);

	bool CheckData(int previous, int current);

	// Check Data Page Values
	void CheckHeroesValues();
	void CheckWanamingoValues();
	void CheckBaseValues();
	void CheckTurretValues();
	void CheckUpgradeCenterValues();
	void CheckBarricadeValues();

	// Get Data Page Values

	void GetHeroValue();
	void GetWanamingoValue();
	void GetBaseValue();
	void GetTurretValue();
	void GetUpgradeCenterValue();
	void GetBarricadeValue();


	void DeleteCurrentData();


private:

	std::vector<UI*> dataPageVector;
	DATA_PAGE_ENUM state;
	Entity* focusEntity;
	UIFactory* factory;

	int resources;
	int level;
	int attackDamage;
	int attackSpeed;
	int range;
	int vision;
	int hpRecovery;
	int xpToNextLevel;

};



#endif //__DATA_PAGES_H__

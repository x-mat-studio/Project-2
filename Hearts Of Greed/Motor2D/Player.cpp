#include "App.h"

#include "Player.h"

#include "Hero.h"
#include "Input.h"
#include "Render.h"
#include "Window.h"
#include "EntityManager.h"
#include "Brofiler/Brofiler/Brofiler.h"
#include "Map.h"
#include "EventManager.h"

ModulePlayer::ModulePlayer() :

	Module(),

	focusedEntity(nullptr),

	resources(0),
	selectRect{ 0,0,0,0 },

	selectUnits(false),
	entityComand(false),
	entityInteraction(false),
	buildMode(false),
	skill1(false),
	skill2(false),
	skill3(false),
	prepareSkill(false),

	hasClicked(false),

	doSkill(false),

	buildingToBuild(ENTITY_TYPE::UNKNOWN)

{
	name.create("player");
}


ModulePlayer::~ModulePlayer()
{}


// Called before render is available
bool ModulePlayer::Awake(pugi::xml_node& config)
{
	BROFILER_CATEGORY("Player Awake", Profiler::Color::DarkCyan);


	return true;
}


// Called before the first frame
bool ModulePlayer::Start()
{
	//register the module as a listener to the following events
	app->eventManager->EventRegister(EVENT_ENUM::ENTITY_COMMAND, this);
	app->eventManager->EventRegister(EVENT_ENUM::ENTITY_INTERACTION, this);
	app->eventManager->EventRegister(EVENT_ENUM::SELECT_UNITS, this);
	app->eventManager->EventRegister(EVENT_ENUM::STOP_SELECTING_UNITS, this);

	app->eventManager->EventRegister(EVENT_ENUM::SKILL1, this);
	app->eventManager->EventRegister(EVENT_ENUM::SKILL2, this);
	app->eventManager->EventRegister(EVENT_ENUM::SKILL3, this);


	return true;
}


// Called each loop iteration
bool ModulePlayer::PreUpdate(float dt)
{
	BROFILER_CATEGORY("Player Pre-Update", Profiler::Color::Blue);

	if (app->input->GetKey(SDL_SCANCODE_4) == KEY_STATE::KEY_DOWN && buildMode == false) // For debug purposes
	{
		ActivateBuildMode(ENTITY_TYPE::BLDG_BASE);
	}

	else if (app->input->GetKey(SDL_SCANCODE_4) == KEY_STATE::KEY_DOWN && buildMode == true) // For debug purposes
	{
		DesactivateBuildMode();
	}


	CheckListener(this);

	HandleInput();


	return true;
}


// Called each loop iteration
bool ModulePlayer::Update(float dt)
{
	BROFILER_CATEGORY("Player Update", Profiler::Color::Blue);

	CheckListener(this);

	return true;
}

// Called each loop iteration
bool ModulePlayer::PostUpdate(float dt)
{
	BROFILER_CATEGORY("Player Post-Update", Profiler::Color::Blue);

	CheckListener(this);

	if (buildMode == true)
	{
		int x = (-app->render->currentCamX + clickPosition.x) / app->win->GetScale();
		int y = (-app->render->currentCamY + clickPosition.y) / app->win->GetScale();

		app->entityManager->PlayerBuildPreview(x, y, buildingToBuild);
	}

	DrawSelectQuad();
	return true;
}


//Handles Player Input
bool ModulePlayer::HandleInput()
{
	if (buildMode == false)
	{
		if (prepareSkill == true || doSkill == true)
		{
			CommandSkill();
		}
		
		else if (entityComand)
		{
			entityComand = false;
			RightClick();
		}

		else if (entityInteraction)
		{
			entityInteraction = false;
			Click();
		}
		else if (selectUnits && hasClicked)
		{
			Select();
		}
	}

	else
	{
		app->input->GetMousePositionRaw(clickPosition.x, clickPosition.y);


		if (entityInteraction)
		{
			entityInteraction = false;
			BuildClick();
		}
	}

	return true;
}


bool ModulePlayer::Click()
{
	focusedEntity = nullptr;
	hasClicked = true;

	app->input->GetMousePositionRaw(clickPosition.x, clickPosition.y);

	clickPosition.x = (-app->render->currentCamX + clickPosition.x) / app->win->GetScale();
	clickPosition.y = (-app->render->currentCamY + clickPosition.y) / app->win->GetScale();

	focusedEntity = app->entityManager->CheckEntityOnClick(clickPosition);

	if (focusedEntity != nullptr)
	{
		app->eventManager->GenerateEvent(EVENT_ENUM::ENTITY_ON_CLICK, EVENT_ENUM::NULL_EVENT);
		return true;
	}

	return false;
}


void ModulePlayer::Select()
{
	iMPoint mousePosition;

	app->input->GetMousePositionRaw(mousePosition.x, mousePosition.y);

	int rectX;
	int rectY;
	int rectW;
	int rectH;

	mousePosition.x = (-app->render->currentCamX + mousePosition.x) / app->win->GetScale();
	mousePosition.y = (-app->render->currentCamY + mousePosition.y) / app->win->GetScale();

	if (mousePosition.x > clickPosition.x)
	{
		rectX = clickPosition.x;
	}
	else
	{
		rectX = mousePosition.x;
	}

	rectW = abs(mousePosition.x - clickPosition.x);

	if (mousePosition.y > clickPosition.y)
	{
		rectY = clickPosition.y;
	}
	else
	{
		rectY = mousePosition.y;
	}

	rectH = abs(mousePosition.y - clickPosition.y);

	selectRect = { rectX,rectY, rectW,rectH };

	app->entityManager->CheckHeroOnSelection(selectRect, &heroesVector);

}


void ModulePlayer::RightClick()
{
	if (heroesVector.empty())
		return;

	Click();
	bool enemyFound;

	int numHeroes = heroesVector.size();

	for (int i = 0; i < numHeroes; i++)
	{
		enemyFound = heroesVector[i]->LockOn(focusedEntity);

		heroesVector[i]->MoveTo(clickPosition.x, clickPosition.y, enemyFound);

	}

}


void ModulePlayer::CommandSkill()
{
	hasClicked = false;

	if (heroesVector.empty() == true)
	{
		prepareSkill = false;
		skill1 = false;
		skill2 = false;
		skill3 = false;
		doSkill = false;

		return;
	}

	else
		if (heroesVector[0] == nullptr)
		{
			prepareSkill = false;
			skill1 = false;
			skill2 = false;
			skill3 = false;
			doSkill = false;

			return;
		}


	if (prepareSkill == true)
	{
		PrepareHeroSkills();
	}


	else if (doSkill == true)
	{
		DoHeroSkills();
	}


}


void ModulePlayer::PrepareHeroSkills()
{

	if (skill1 == true)
	{
		if (heroesVector[0]->skill1Charged == true)
		{
			doSkill = heroesVector[0]->PrepareSkill1();
			prepareSkill = !doSkill;
		}

		else
		{
			prepareSkill = false;
			skill1 = false;
		}

	}

	else if (skill2 == true)
	{
		if (heroesVector[0]->skill2Charged == true)
		{
			doSkill = heroesVector[0]->PrepareSkill2();
			prepareSkill = !doSkill;
		}

		else
		{
			prepareSkill = false;
			skill2 = false;
		}
	}


	else if (skill3 == true)
	{
		if (heroesVector[0]->skill3Charged == true)
		{
			doSkill = heroesVector[0]->PrepareSkill3();
			prepareSkill = !doSkill;

		}

		else
		{
			prepareSkill = false;
			skill3 = false;
		}
	}

}


void ModulePlayer::DoHeroSkills()
{
	if (entityComand == true)
	{
		entityComand = false;
		heroesVector[0]->SkillCanceled();


		prepareSkill = false;
		skill1 = false;
		skill2 = false;
		skill3 = false;
		doSkill = false;
	}

	if (entityInteraction == true)
	{
		entityInteraction = false;

		if (skill1 == true)
		{
			if (heroesVector[0]->ActivateSkill1(app->input->GetMouseWorld()) == true)
			{
				skill1 = false;
				doSkill = false;
				selectUnits = false;
			}
		}

		else if (skill2 == true)
		{
			if (heroesVector[0]->ActivateSkill2() == true)
			{
				skill2 = false;
				doSkill = false;
				selectUnits = false;
			}
		}

		else if (skill3 == true)
		{
			if (heroesVector[0]->ActivateSkill3() == true)
			{
				skill3 = false;
				doSkill = false;
				selectUnits = false;
			}
		}
	}

}


bool ModulePlayer::BuildClick()
{
	//Needs more work
	int x = (-app->render->currentCamX + clickPosition.x) / app->win->GetScale();
	int y = (-app->render->currentCamY + clickPosition.y) / app->win->GetScale();


	app->entityManager->AddEntity(buildingToBuild, x, y, ENTITY_ALIGNEMENT::PLAYER);

	return true;
}


void ModulePlayer::ExecuteEvent(EVENT_ENUM eventId)
{
	switch (eventId)
	{
	case EVENT_ENUM::SELECT_UNITS:
		selectUnits = true;
		break;

	case EVENT_ENUM::STOP_SELECTING_UNITS:
		selectUnits = false;
		break;

	case EVENT_ENUM::ENTITY_COMMAND:
		entityComand = true;
		break;

	case EVENT_ENUM::ENTITY_INTERACTION:
		entityInteraction = true;
		break;

	case EVENT_ENUM::SKILL1:
	
		if (doSkill == false) 
		{
			skill1 = true;
			prepareSkill = true;
		}
		
		break;

	case EVENT_ENUM::SKILL2:

		if (doSkill == false)
		{
			skill2 = true;
			prepareSkill = true;
		}
		
		break;

	case EVENT_ENUM::SKILL3:
		if (doSkill == false)
		{
			skill3 = true;
			prepareSkill = true;
		}
		
		break;
	}


}


void ModulePlayer::DrawSelectQuad()
{
	if (selectUnits)
	{
		app->render->DrawQuad(selectRect, 0, 200, 0, 50, false);
		app->render->DrawQuad(selectRect, 0, 200, 0, 100);
	}
	else
	{
		selectRect = { 0,0,0,0 };
	}
}


void ModulePlayer::AddResources(int gain)
{
	resources += gain;
}


bool ModulePlayer::UseResources(int cost)
{
	if (cost > resources)
		return false;


	else
	{
		resources -= cost;
		return true;
	}
}


bool ModulePlayer::ActivateBuildMode(ENTITY_TYPE building)
{
	if (buildMode == false || (building != ENTITY_TYPE::BLDG_TURRET && building != ENTITY_TYPE::BLDG_UPGRADE_CENTER && building != ENTITY_TYPE::BLDG_BARRICADE && building != ENTITY_TYPE::BUILDING))
	{
		buildMode = true;
		buildingToBuild = building;

		return true;
	}

	else
		return false;

}


void ModulePlayer::DesactivateBuildMode()
{
	buildMode = false;
	buildingToBuild = ENTITY_TYPE::UNKNOWN;
}


void ModulePlayer::RemoveHeroFromVector(Hero* hero)
{
	int numHeroes = heroesVector.size();

	for (int i = 0; i < numHeroes; i++)
	{
		if (heroesVector[i] == hero)
		{
			heroesVector.erase(heroesVector.begin() + i);
			return;
		}
	}
}


iMPoint ModulePlayer::GetClickPosition()
{
	return clickPosition;
}
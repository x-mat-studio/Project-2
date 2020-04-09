#include "App.h"
#include "Textures.h"
#include "UIManager.h"
#include "UI_Image.h"
#include "UI_Button.h"
#include "UI_Text.h"
#include "UI_Healthbar.h"
#include "UI_Portrait.h"
#include "UI_Scrollbar.h"
#include "EventManager.h"
#include "EntityManager.h"
#include "Window.h"
#include "Brofiler/Brofiler/Brofiler.h"


ModuleUIManager::ModuleUIManager()
{
	name.create("UIManager");
}


// Destructor
ModuleUIManager::~ModuleUIManager()
{
	
	app->tex->UnLoad(atlas);
	atlas = nullptr;

}


// Called before render is available
bool ModuleUIManager::Awake(pugi::xml_node& config)
{
	BROFILER_CATEGORY("UI Manager Awake", Profiler::Color::MediumPurple);

	bool ret = true;

	app->eventManager->EventRegister(EVENT_ENUM::HERO_MELEE_CREATED, this);
	app->eventManager->EventRegister(EVENT_ENUM::HERO_GATHERER_CREATED, this);
	app->eventManager->EventRegister(EVENT_ENUM::HERO_RANGED_CREATED, this);
	app->eventManager->EventRegister(EVENT_ENUM::HERO_MELEE_OUT, this);
	app->eventManager->EventRegister(EVENT_ENUM::HERO_GATHERER_OUT, this);
	app->eventManager->EventRegister(EVENT_ENUM::HERO_RANGED_OUT, this);


	return ret;
}


// Called before the first frame
bool ModuleUIManager::Start()
{
	bool ret = true;

	LoadAtlas();

	return ret;
}

// Called each loop iteration
bool ModuleUIManager::PreUpdate(float dt)
{
	BROFILER_CATEGORY("UI Manager Pre-Update", Profiler::Color::Purple)

		bool ret = true;
	
	CheckListener(this);

	int numEntities = uiVector.size();

	for (int i = 0; i < numEntities; i++)
	{
		uiVector[i]->PreUpdate(dt);
	}

	return true;
}

// Called each loop iteration
bool ModuleUIManager::Update(float dt)
{
	BROFILER_CATEGORY("UI Manager Update", Profiler::Color::Purple)

		bool ret = true;

	int numEntities = uiVector.size();

	for (int i = 0; i < numEntities; i++)
	{
		uiVector[i]->Update(dt);
	}

	return ret;
}

// Called each loop iteration
bool ModuleUIManager::PostUpdate(float dt)
{
	BROFILER_CATEGORY("UI Manager Post Update", Profiler::Color::Purple)

		bool ret = true;

	int numEntities = uiVector.size();

	for (int i = 0; i < numEntities; i++)
	{
		uiVector[i]->PostUpdate(dt);
	}

	return ret;
}

//// Called before quitting
bool ModuleUIManager::CleanUp()
{
	int numElements = uiVector.size();

	for (int i = 0; i < numElements; i++)
	{
		RELEASE(uiVector[i]);
		uiVector[i] = nullptr;
	}

	uiVector.clear();

	portraitPointer = nullptr;

	return true;
}

UI* ModuleUIManager::AddUIElement(fMPoint positionValue, UI* father, UI_TYPE uiType, SDL_Rect rect, P2SString uiName, DRAGGABLE dragable, char* text, SDL_Color color)
{
	UI* newUI = nullptr;

	switch (uiType)
	{
	case UI_TYPE::UI_TEXT:
		newUI = new UI_Text(positionValue, father, uiType, rect, uiName, dragable, text, color);
		break;
	case UI_TYPE::UI_IMG:
		newUI = new UI_Image(positionValue, father, uiType, rect, uiName, dragable);
		break;
	case UI_TYPE::UI_SCROLLBAR:
		newUI = new UI_Scrollbar(positionValue, father, uiType, rect, uiName, dragable);
		break;
	case UI_TYPE::UI_HEALTHBAR:
		newUI = new UI_Healthbar(positionValue, father, uiType, rect, uiName, dragable);
		break;
	case UI_TYPE::UI_PORTRAIT:
		portraitPointer = new UI_Portrait(positionValue, father, uiType, rect, uiName, dragable);
		newUI = portraitPointer;
		break;
	}

	if (newUI != nullptr)
	{
		uiVector.push_back(newUI);
	}

	return newUI;
}

UI* ModuleUIManager::AddButton(fMPoint positionValue, UI* father, UI_TYPE uiType, SDL_Rect rect, P2SString uiName, bool menuClosure, EVENT_ENUM eventTrigger, DRAGGABLE draggable)
{
	UI* newUI = new UI_Button(positionValue, father, uiType, rect, uiName, menuClosure, draggable, eventTrigger);
	uiVector.push_back(newUI);
	return newUI;
}


void ModuleUIManager::RemoveDeletedUI()
{
	int numEntitys = uiVector.size();

	for (int i = 0; i < numEntitys; i++)
	{
		if (uiVector[i]->toDelete == true)
		{
			uiVector.erase(uiVector.begin() + i);
		}
	}

}

void ModuleUIManager::ExecuteEvent(EVENT_ENUM eventId)
{

	int eventCheck = 0;

	switch (eventId)
	{

	// When adding a Hero to these enum, add it to the checking function below
	case EVENT_ENUM::HERO_MELEE_CREATED:
	case EVENT_ENUM::HERO_GATHERER_CREATED:
	case EVENT_ENUM::HERO_RANGED_CREATED:

		if (portraitPointer != nullptr)
		{
			while (eventCheck == 0)
			{
				Hero* hero = app->entityManager->CheckUIAssigned(eventCheck);
				if (hero != nullptr)
				{
					portraitPointer->CreatePortrait(hero);
					portraitPointer->nextVectorPosition += 60;
				}
			}
		}

		break;
	case EVENT_ENUM::HERO_MELEE_OUT:
	case EVENT_ENUM::HERO_GATHERER_OUT:
	case EVENT_ENUM::HERO_RANGED_OUT:
		// DELETE PORTRAIT			TODO
		break;

	case EVENT_ENUM::OPTION_MENU:

		break;

	case EVENT_ENUM::CREDIT_MENU:

		break;
	}
}

void ModuleUIManager::CreateBasicUI()
{
	SDL_Rect rect;
	uint w(app->win->width), h(app->win->height);

	rect = RectConstructor(0, 0, 0, 0);
	AddUIElement(fMPoint(w / app->win->GetScale() - 72, 35), nullptr, UI_TYPE::UI_PORTRAIT, rect, (P2SString)"portraitVector", DRAGGABLE::DRAG_OFF);

	rect = RectConstructor(221, 317, 162, 174);
	AddUIElement(fMPoint(0, h / app->win->GetScale() - rect.h), nullptr, UI_TYPE::UI_IMG, rect, (P2SString)"minimapBackground");

	rect = RectConstructor(449, 24, 24, 24);
	AddUIElement(fMPoint(w / app->win->GetScale() - (1.25f) * rect.w, (1.25f) * rect.w - rect.w), nullptr, UI_TYPE::UI_BUTTON, rect, (P2SString)"pauseButton", DRAGGABLE::DRAG_OFF);

	rect = RectConstructor(0, 0, 0, 0); // Text will ignore Rect.
	AddUIElement(fMPoint(20, 0), nullptr, UI_TYPE::UI_TEXT, rect, (P2SString)"textDemo", DRAGGABLE::DRAG_OFF, "DEMO OF TEXT");

	rect = RectConstructor(17, 12, 195, 37);
	AddUIElement(fMPoint(20, 40), nullptr, UI_TYPE::UI_SCROLLBAR, rect, (P2SString)"scrollBar", DRAGGABLE::DRAG_XY);
}

void ModuleUIManager::CreateMainMenu()
{
	SDL_Rect rect = RectConstructor(17, 12, 195, 36);
	uint w(app->win->width), h(app->win->height);	// TODO Do the right events, their listener, and the pertinent code. Does load just happen, or do I have to close modules and then do it?
	// Load i continue son events diferents (mirar DEV) = cambiam a test scene --> carreguem dades
	
	AddButton(fMPoint(w / app->win->GetScale() - rect.w - 20, (h / (app->win->GetScale() * 4))), nullptr, UI_TYPE::UI_BUTTON, rect, (P2SString)"continueButton", true, EVENT_ENUM::START_GAME_FROM_CONTINUE, DRAGGABLE::DRAG_OFF);

	AddButton(fMPoint(w / app->win->GetScale() - rect.w - 20, (h / (app->win->GetScale() * 4)) + 40), nullptr, UI_TYPE::UI_BUTTON, rect, (P2SString)"newGameButton", true, EVENT_ENUM::START_GAME, DRAGGABLE::DRAG_OFF);

	AddButton(fMPoint(w / app->win->GetScale() - rect.w - 20, (h / (app->win->GetScale() * 4)) + 80), nullptr, UI_TYPE::UI_BUTTON, rect, (P2SString)"optionsButton", false, EVENT_ENUM::OPTION_MENU, DRAGGABLE::DRAG_OFF);

	AddButton(fMPoint(w / app->win->GetScale() - rect.w - 20, (h / (app->win->GetScale() * 4)) + 120), nullptr, UI_TYPE::UI_BUTTON, rect, (P2SString)"creditsButton", false, EVENT_ENUM::CREDIT_MENU, DRAGGABLE::DRAG_OFF);

	AddButton(fMPoint(w / app->win->GetScale() - rect.w - 20, (h / (app->win->GetScale() * 4)) + 160), nullptr, UI_TYPE::UI_BUTTON, rect, (P2SString)"exitGameButton", true, EVENT_ENUM::EXIT_GAME, DRAGGABLE::DRAG_OFF);

	AddUIElement(fMPoint(w / app->win->GetScale() - rect.w + 5, (h / (app->win->GetScale() * 4)) + 5), nullptr, UI_TYPE::UI_TEXT, rect, (P2SString)"continueButton", DRAGGABLE::DRAG_OFF, "C O N T I N U E    G A M E");
	
	AddUIElement(fMPoint(w / app->win->GetScale() - rect.w + 35, (h / (app->win->GetScale() * 4)) + 45), nullptr, UI_TYPE::UI_TEXT, rect, (P2SString)"newGameButton", DRAGGABLE::DRAG_OFF, "N E W    G A M E");
	
	AddUIElement(fMPoint(w / app->win->GetScale() - rect.w + 40, (h / (app->win->GetScale() * 4)) + 85), nullptr, UI_TYPE::UI_TEXT, rect, (P2SString)"optionsButton", DRAGGABLE::DRAG_OFF, "O P T I O N S");
	
	AddUIElement(fMPoint(w / app->win->GetScale() - rect.w + 42, (h / (app->win->GetScale() * 4)) + 125), nullptr, UI_TYPE::UI_TEXT, rect, (P2SString)"creditsButton", DRAGGABLE::DRAG_OFF, "C R E D I T S");
	
	AddUIElement(fMPoint(w / app->win->GetScale() - rect.w + 30, (h / (app->win->GetScale() * 4)) + 165), nullptr, UI_TYPE::UI_TEXT, rect, (P2SString)"exitGameButton", DRAGGABLE::DRAG_OFF, "E X I T    G A M E");

}

SDL_Texture* ModuleUIManager::GetAtlasTexture() const
{ return atlas; }

SDL_Rect ModuleUIManager::RectConstructor(int x, int y, int w, int h)
{
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
	return rect;
}

void ModuleUIManager::LoadAtlas()
{
	if(!atlas)
	atlas = app->tex->Load("spritesheets/atlas.png");
}
#include "App.h"
#include "Input.h"
#include "SDL/include/SDL_scancode.h"
#include "MainMenuScene.h"
#include "FadeToBlack.h"
#include "TestScene.h"
#include "UIManager.h"
#include "UI_Text.h"

ModuleMainMenuScene::ModuleMainMenuScene()
{

}


ModuleMainMenuScene::~ModuleMainMenuScene()
{

}


bool  ModuleMainMenuScene::Awake(pugi::xml_node&)
{
	return true;
}


// Called before the first frame
bool ModuleMainMenuScene::Start()
{

	SDL_Rect rect = { 0, 0, 0, 0 };
	app->uiManager->AddUIElement(fMPoint(20, 0), nullptr, UI_TYPE::UI_TEXT, rect, (P2SString)"MenuScene", DRAGGABLE::DRAG_OFF, "DEMO OF TEXT / Menu Scene /  Press N to go to the Game");

	return true;
}


// Called each loop iteration
bool  ModuleMainMenuScene::PreUpdate(float dt)
{
	CheckListener(this);

	return true;
}


// Called each loop iteration
bool  ModuleMainMenuScene::Update(float dt)
{
	CheckListener(this);

	return true;
}


// Called each loop iteration
bool  ModuleMainMenuScene::PostUpdate(float dt)
{
	bool ret = true;

	//TODO CHANGE THIS FOR THE ACTION THAT CHANGES TO THE MAIN SCENE
	if (app->input->GetKey(SDL_SCANCODE_N) == KEY_STATE::KEY_DOWN) {

		app->fadeToBlack->FadeToBlack(this, app->testScene, 2.0f);
	}

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_STATE::KEY_DOWN) {

		ret = false;
	}

	return ret;
}


// Called before quitting
bool  ModuleMainMenuScene::CleanUp()
{
	app->uiManager->CleanUp();

	return true;
}


bool  ModuleMainMenuScene::Load(pugi::xml_node&)
{
	return true;
}


bool  ModuleMainMenuScene::Save(pugi::xml_node&) const
{
	return true;
}


void ModuleMainMenuScene::ExecuteEvent(EVENT_ENUM eventId) const
{}
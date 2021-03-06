#include "TestScene.h"

#include "App.h"

#include "Input.h"
#include "Render.h"
#include "Window.h"
#include "Audio.h"
#include "Map.h"
#include "Fonts.h"
#include "Pathfinding.h"
#include "FoWManager.h"
#include "EntityManager.h"
#include "FadeToBlack.h"
#include "WinScene.h"
#include "LoseScene.h"
#include "UIManager.h"
#include "MainMenuScene.h"
#include "EventManager.h"
#include "QuestManager.h"
#include "Minimap.h"
#include "Render.h"
#include "Player.h"
#include "AI.h"
#include "Brofiler/Brofiler/Brofiler.h"

ModuleTestScene::ModuleTestScene() :
	prevMousePosX(0),
	prevmousePosY(0),
	timer(0),
	dayNumber(0),

	dayTimer(INT_MAX),
	nightTimer(INT_MAX),
	camVel(0.f),
	fadeTime(0),
	startingScale(1.0f),
	nightApproachesSfx(-1),

	camToReset(false),
	camUp(false),
	camDown(false),
	camRight(false),
	camLeft(false),
	camSprint(false),
	allowCamMovement(true),
	menuScene(false),
	isNightTime(false),
	mapLoaded(false),
	startFromLoad(false),
	haveJustBeenLoaded(false)
{
	name.create("testScene");
}


ModuleTestScene::~ModuleTestScene()
{}


bool  ModuleTestScene::Awake(pugi::xml_node& config)
{

	camVel = config.attribute("camVel").as_float(1);
	initialCamPos.x = -config.attribute("initialCamPosX").as_float(0);
	initialCamPos.y = -config.attribute("initialCamPosY").as_float(0);


	dayTimer = config.attribute("dayTimerSec").as_int(1);
	nightTimer = config.attribute("nightTimerSec").as_int(1);

	camMarginMovements.x = config.attribute("freeCamMarginDetectionPixelsX").as_int(1);
	camMarginMovements.y = config.attribute("freeCamMarginDetectionPixelsY").as_int(1);


	mapBordersUpperLeftCorner.x = config.attribute("mapBordersUpperLeftCornerX").as_int(0);
	mapBordersUpperLeftCorner.y = config.attribute("mapBordersUpperLeftCornerY").as_int(0);

	mapBordersBottomRightCorner.x = config.attribute("mapBordersBottomRightCornerX").as_int(0);
	mapBordersBottomRightCorner.y = config.attribute("mapBordersBottomRightCornerY").as_int(0);

	fadeTime = config.attribute("fadeTime").as_float(0);
	startingScale = config.attribute("startingScale").as_float(.0f);

	nightApproachesSfx = app->audio->LoadFx("Assets/audio/sfx/Interface/NightApproaches.wav");

	return true;
}


// Called before the first frame
bool ModuleTestScene::Start()
{
	mapLoaded = false;
	app->uiManager->SetPopUpClosingBool(false);
	app->player->Enable();
	app->minimap->Enable();

	camToReset = true;
	//Play Music
	app->audio->PlayMusic("Assets/audio/music/Map.ogg", fadeTime, app->audio->musicVolume);

	//Load sfx used in this scene
	if (app->map->LoadNew("finalMap.tmx") == true)
	{
		int w, h;
		uchar* data = nullptr;
		if (app->map->CreateWalkabilityMap(w, h, &data))
		{
			app->pathfinding->SetMap(w, h, data);
			RELEASE_ARRAY(data);
		}
		app->fowManager->CreateFoWMap(app->map->data.width, app->map->data.height);


		isNightTime = false;
		nightRectAlpha.NewEasing(EASING_TYPE::EASE, 0, 0, 0);
		//Test Hero
		if (startFromLoad == false)
		{
			app->entityManager->AddEntity(ENTITY_TYPE::HERO_GATHERER, 400, 4150);
			//app->entityManager->AddEntity(ENTITY_TYPE::HERO_MELEE, pos.x - 680, pos.y);
		}

		app->eventManager->GenerateEvent(EVENT_ENUM::GAME_STARTED, EVENT_ENUM::NULL_EVENT);
	}

	if (startFromLoad == true)
	{
		app->entityManager->DeleteAllEntities();
		app->LoadGame();
		startFromLoad = false;
	}
	else
	{
		dayNumber = 0;
	}



	//Events register
	app->eventManager->EventRegister(EVENT_ENUM::CAMERA_UP, this);
	app->eventManager->EventRegister(EVENT_ENUM::CAMERA_DOWN, this);
	app->eventManager->EventRegister(EVENT_ENUM::CAMERA_LEFT, this);
	app->eventManager->EventRegister(EVENT_ENUM::CAMERA_RIGHT, this);
	app->eventManager->EventRegister(EVENT_ENUM::STOP_CAMERA_UP, this);
	app->eventManager->EventRegister(EVENT_ENUM::STOP_CAMERA_DOWN, this);
	app->eventManager->EventRegister(EVENT_ENUM::STOP_CAMERA_LEFT, this);
	app->eventManager->EventRegister(EVENT_ENUM::STOP_CAMERA_RIGHT, this);
	app->eventManager->EventRegister(EVENT_ENUM::CAMERA_SPRINT, this);
	app->eventManager->EventRegister(EVENT_ENUM::STOP_CAMERA_SPRINT, this);
	app->eventManager->EventRegister(EVENT_ENUM::SAVE_GAME, this);
	app->eventManager->EventRegister(EVENT_ENUM::LOAD_GAME, this);
	app->eventManager->EventRegister(EVENT_ENUM::GAME_SCENE_ENTERED, this);
	app->eventManager->EventRegister(EVENT_ENUM::RETURN_TO_MAIN_MENU, this);

	app->eventManager->EventRegister(EVENT_ENUM::DEBUG_DAY, this);
	app->eventManager->EventRegister(EVENT_ENUM::DEBUG_NIGHT, this);
	app->eventManager->EventRegister(EVENT_ENUM::CAMERA_FOCUS_HERO, this);

	app->eventManager->EventRegister(EVENT_ENUM::START_DAY_NIGHT_TRANSITION, this);

	app->eventManager->GenerateEvent(EVENT_ENUM::GAME_SCENE_ENTERED, EVENT_ENUM::NULL_EVENT);

	app->gamePause = false;

	return true;
}


// Called each loop iteration
bool  ModuleTestScene::PreUpdate(float dt)
{
	if (mapLoaded == false)
	{
		app->minimap->LoadMinimap();
		mapLoaded = true;
	}

	if (camToReset == true)
	{
		if (haveJustBeenLoaded == false)
		{
			app->win->SetScale(startingScale);
			app->render->currentCamX = initialCamPos.x;
			app->render->currentCamY = initialCamPos.y;
		}
		else
		{
			haveJustBeenLoaded = false;
		}
		camToReset = false;
	}

	CheckListener(this);


	//VERTICAL SLICE
	CalculateTimers(dt);


	return true;
}


// Called each loop iteration
bool  ModuleTestScene::Update(float dt)
{
	CheckListener(this);

	float camVelAux = camVel;
	float scale = app->win->GetScale();
	iMPoint mousePos;
	iMPoint mouseRaw;
	SDL_GetMouseState(&mouseRaw.x, &mouseRaw.y);

	mousePos.x = round((float)mouseRaw.x / scale);
	mousePos.y = round((float)mouseRaw.y / scale);


	if (app->input->GetKey(SDL_SCANCODE_9) == KEY_STATE::KEY_DOWN) //Debug key to lock camera movement
	{
		ToggleCamMovement();
	}

	if (nightRectAlpha.IsActive() == true)
	{
		nightRectAlpha.UpdateEasingAddingTime(dt);
	}

	////Cam Easing Testing code
	//if (app->input->GetKey(SDL_SCANCODE_Y) == KEY_STATE::KEY_DOWN)
	//{

	//	MoveCamTo(fMPoint{ 525,3119 }, 2.0, EASING_TYPE::EASE_IN_OUT_SINE);
	//}

	if (allowCamMovement)
	{
		bool wasdMove = false;

		if (camSprint)
		{
			camVelAux *= 2;
			wasdMove = true;
		}
		if (camUp)
		{
			app->render->currentCamY += camVelAux * dt;
			wasdMove = true;
		}
		if (camDown)
		{
			app->render->currentCamY -= camVelAux * dt;
			wasdMove = true;
		}
		if (camLeft)
		{
			app->render->currentCamX += camVelAux * dt;
			wasdMove = true;
		}
		if (camRight)
		{
			app->render->currentCamX -= camVelAux * dt;
			wasdMove = true;
		}


		if (!wasdMove)
		{
			//mouse drag / mouse zoom
			iMPoint scrollWheel;
			app->input->GetScrollWheelMotion(scrollWheel.x, scrollWheel.y);

			if (app->input->GetMouseButtonDown(2) == KEY_STATE::KEY_DOWN) //TODO THIS WILL BE A START DRAGGING EVENT
			{
				StartDragging(mousePos);
				SetCamEasingState(false);
			}
			else if (app->input->GetMouseButtonDown(2) == KEY_STATE::KEY_REPEAT) //TODO THIS WILL BE ACTIVE WHILE STOP DRAGGING EVENT ISN'T SENT
			{
				Drag(mousePos, scale);
				SetCamEasingState(false);
			}
			else if (scrollWheel.y != 0)
			{
				//that 0.25 is an arbitrary number and will be changed to be read from the config file. TODO
				if (app->minimap->ClickingOnMinimap(mouseRaw.x, mouseRaw.y) == false && app->uiManager->mouseOverUI == false)
				{
					Zoom(app->win->GetScaleFactor() * scrollWheel.y, mouseRaw.x, mouseRaw.y, scale);
				}

			}
			else
			{
				if (MouseCameraDisplacement(camVel, dt) == false)
				{
					if (IsCamDoingEasing() == true)
					{
						UpdateCamEasing(dt);
					}
				}
				else
				{
					SetCamEasingState(false);
				}
			}

		}
		else
		{
			SetCamEasingState(false);
		}

		ConstrainCameraToBorders();
	}

	//TODO CHANGE THIS FOR THE ACTION THAT CHANGES TO THE WIN SCENE
	if (app->input->GetKey(SDL_SCANCODE_N) == KEY_STATE::KEY_DOWN)
	{
		app->eventManager->GenerateEvent(EVENT_ENUM::GAME_WIN, EVENT_ENUM::NULL_EVENT);
	}
	//TODO CHANGE THIS FOR THE ACTION THAT CHANGES TO THE LOSE SCENE
	if (app->input->GetKey(SDL_SCANCODE_M) == KEY_STATE::KEY_DOWN)
	{
		app->eventManager->GenerateEvent(EVENT_ENUM::GAME_LOSE, EVENT_ENUM::NULL_EVENT);
	}
	//TODO CHANGE THIS FOR THE ACTION THAT CHANGES TO THE MENU SCENE
	if (menuScene == true)
	{
		if (app->fadeToBlack->FadeToBlack(this, app->mainMenu, fadeTime * 2, FADE_ANIMATION::CURTAIN))
		{
			menuScene = false;
		}
	}
	return true;
}


// Called each loop iteration
bool  ModuleTestScene::PostUpdate(float dt)
{
	bool ret = true;

	//Debug timer key to make the timer be 35 sec
	if (app->input->GetKey(SDL_SCANCODE_B) == KEY_STATE::KEY_DOWN)
		timer = 264;

	BROFILER_CATEGORY("Game Scene PostUpdate", Profiler::Color::LightYellow);

	app->map->Draw();

	int alpha = nightRectAlpha.GetLastRequestedPos();
	DrawNightRect(alpha);


	//if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_STATE::KEY_DOWN) {

	//	ret = false;
	//}

	//DEBUG WALKABILITY

	if (app->debugMode)
	{
		SDL_SetTextureAlphaMod(app->entityManager->debugPathTexture, 255 * 0.5f);
		for (int i = 0; i < app->map->data.width; i++)
		{
			for (int j = 0; j < app->map->data.height; j++)
			{
				if (app->pathfinding->IsWalkable({ i + 1,j }))
				{
					iMPoint p = app->map->MapToWorld(i, j);

					app->render->Blit(app->entityManager->debugPathTexture, p.x, p.y);
				}

			}
		}
		SDL_SetTextureAlphaMod(app->entityManager->debugPathTexture, 255);

	}


	return ret;
}


// Called before quitting
bool  ModuleTestScene::CleanUp()
{

	app->pathfinding->CleanUp();
	app->uiManager->CleanUp();
	app->entityManager->ResetEntityManager();
	app->coll->CleanUp();
	app->map->CleanUp();
	app->fowManager->DeleteFoWMap();
	app->audio->SilenceAll();
	app->minimap->Disable();


	app->player->Disable();
	app->ai->ResetAI();


	return true;
}


bool  ModuleTestScene::Load(pugi::xml_node& data)
{
	pugi::xml_node iterator = data.first_child();

	dayNumber = iterator.attribute("days_passed").as_int();
	isNightTime = iterator.attribute("is_night").as_bool();

	timer = iterator.attribute("timer").as_float();


	haveJustBeenLoaded = true;
	return true;
}


bool  ModuleTestScene::Save(pugi::xml_node& data) const
{
	pugi::xml_node iterator = data.append_child("time_info");

	iterator.append_attribute("days_passed") = dayNumber;
	iterator.append_attribute("is_night") = isNightTime;
	iterator.append_attribute("timer") = timer;
	return true;
}


void ModuleTestScene::Zoom(float addZoomAmount, int windowTargetCenterX, int windowTargetCenterY, float currentScale)
{
	float newScale = app->win->AddScale(addZoomAmount);
	float increment = newScale - currentScale;
	float offsetX = windowTargetCenterX;
	float offsetY = windowTargetCenterY;

	//does not change the camera if there is no scaling
	if (increment != 0.0f)
	{
		app->render->currentCamX = (((app->render->currentCamX - offsetX) * newScale) / currentScale) + offsetX;
		app->render->currentCamY = (((app->render->currentCamY - offsetY) * newScale) / currentScale) + offsetY;
	}
}


void ModuleTestScene::ExecuteEvent(EVENT_ENUM eventId)
{
	switch (eventId)
	{
	case EVENT_ENUM::CAMERA_UP:
		camUp = true;
		break;
	case EVENT_ENUM::STOP_CAMERA_UP:
		camUp = false;
		break;
	case EVENT_ENUM::CAMERA_DOWN:
		camDown = true;
		break;
	case EVENT_ENUM::STOP_CAMERA_DOWN:
		camDown = false;
		break;
	case EVENT_ENUM::CAMERA_RIGHT:
		camRight = true;
		break;
	case EVENT_ENUM::STOP_CAMERA_RIGHT:
		camRight = false;
		break;
	case EVENT_ENUM::CAMERA_LEFT:
		camLeft = true;
		break;
	case EVENT_ENUM::STOP_CAMERA_LEFT:
		camLeft = false;
		break;
	case EVENT_ENUM::CAMERA_SPRINT:
		camSprint = true;
		break;
	case EVENT_ENUM::STOP_CAMERA_SPRINT:
		camSprint = false;
		break;
	case EVENT_ENUM::SAVE_GAME:
		// TODO Save game from here
		break;
	case EVENT_ENUM::LOAD_GAME:
		// TODO Load game from here
		break;

	case EVENT_ENUM::RETURN_TO_MAIN_MENU:
		menuScene = true;
		break;

	case EVENT_ENUM::DEBUG_DAY:
		app->eventManager->GenerateEvent(EVENT_ENUM::DAY_START, EVENT_ENUM::NULL_EVENT);
		isNightTime = false;
		nightRectAlpha.NewEasing(EASING_TYPE::EASE, nightRectAlpha.GetLastRequestedPos(), 0, 1);
		timer = 0;
		dayNumber++;
		break;

	case EVENT_ENUM::DEBUG_NIGHT:
		app->eventManager->GenerateEvent(EVENT_ENUM::NIGHT_START, EVENT_ENUM::NULL_EVENT);
		//app->audio->PlayFx(nightApproachesSfx, 0, -1);
		isNightTime = true;
		nightRectAlpha.NewEasing(EASING_TYPE::EASE, nightRectAlpha.GetLastRequestedPos(), 100, 1);
		//app->uiManager->AddUIElement(fMPoint(20, 0), nullptr, UI_TYPE::UI_TEXT, { 0,0,0,0 }, (P2SString)"TestScene", nullptr, DRAGGABLE::DRAG_OFF, "The night is closing on you... Go back to your previous base before it's too late...");
		timer = 0;
		break;

	case EVENT_ENUM::START_DAY_NIGHT_TRANSITION:
		if (isNightTime == true)
			nightRectAlpha.NewEasing(EASING_TYPE::EASE, 100, 0, 30);
		else
			nightRectAlpha.NewEasing(EASING_TYPE::EASE, 0, 100, 30);

		break;

	case EVENT_ENUM::CAMERA_FOCUS_HERO:
		Entity* hero = app->player->GetFocusedEntity();
		if (hero != nullptr)
		{
			switch (hero->GetType())
			{
			case ENTITY_TYPE::HERO_GATHERER:
			case ENTITY_TYPE::HERO_MELEE:
			case ENTITY_TYPE::HERO_RANGED:
			case ENTITY_TYPE::HERO_ROBO:
				MoveCamTo(hero->position, 1, EASING_TYPE::EASE_IN_OUT_SINE);
			default:
				break;
			}
			break;
		}

	}

}


void ModuleTestScene::StartDragging(iMPoint mousePos)
{
	prevMousePosX = mousePos.x;
	prevmousePosY = mousePos.y;
}


void ModuleTestScene::Drag(iMPoint mousePos, float scale)
{
	iMPoint aux = { 0,0 };
	iMPoint xy = mousePos;
	aux = xy;
	xy.x -= prevMousePosX;
	xy.y -= prevmousePosY;
	app->render->currentCamX += xy.x * scale;
	app->render->currentCamY += xy.y * scale;

	prevMousePosX = aux.x;
	prevmousePosY = aux.y;
}


bool ModuleTestScene::MouseCameraDisplacement(float camVel, float dt)
{
	bool ret = false;
	iMPoint mouseRaw = app->input->GetMousePosScreen();
	uint width;
	uint height;
	app->win->GetWindowSize(width, height);

	if (mouseRaw.x <= camMarginMovements.x)
	{
		app->render->currentCamX += camVel * dt;
		ret = true;
	}
	else if (mouseRaw.x >= width - 1 - camMarginMovements.x)
	{
		app->render->currentCamX -= camVel * dt;
		ret = true;
	}

	if (mouseRaw.y <= camMarginMovements.y)
	{
		app->render->currentCamY += camVel * dt;
		ret = true;

	}
	else if (mouseRaw.y >= height - 1 - camMarginMovements.y)
	{
		app->render->currentCamY -= camVel * dt;
		ret = true;
	}
	return ret;
}


void ModuleTestScene::ToggleCamMovement()
{
	allowCamMovement = !allowCamMovement;
}


void ModuleTestScene::ActivateCamMovement()
{
	allowCamMovement = true;
}


void ModuleTestScene::DeactivateCamMovement()
{
	allowCamMovement = false;
}


bool ModuleTestScene::GetCamMovementActivated() const
{
	return allowCamMovement;
}


void ModuleTestScene::CalculateTimers(float dt)
{
	if (isNightTime == false)
	{
		timer += dt;

		if (timer >= dayTimer)
		{
			app->eventManager->GenerateEvent(EVENT_ENUM::NIGHT_START, EVENT_ENUM::NULL_EVENT);
			isNightTime = true;
			timer = 0;
		}
	}

	else
	{
		timer += dt;

		if (timer >= nightTimer)
		{
			app->eventManager->GenerateEvent(EVENT_ENUM::DAY_START, EVENT_ENUM::NULL_EVENT);
			isNightTime = false;
			timer = 0;
			dayNumber++;
		}
	}
}


void ModuleTestScene::DrawNightRect(int alpha)
{
	int newAlpha = MAX(alpha, 0);
	newAlpha = MIN(alpha, 255);

	//only draw the rect when its not invisible
	if (newAlpha != 0)
	{

		float scale = app->win->GetScale();

		SDL_Rect rect;

		rect.x = 0;
		rect.y = 0;

		if (scale > 1)
		{
			rect.w = app->win->width * scale;
			rect.h = app->win->height * scale;
		}
		else
		{
			rect.w = app->win->width / scale;
			rect.h = app->win->height / scale;
		}

		app->render->DrawQuad(rect, 60, 26, 81, newAlpha, true, false);
	}
}


int ModuleTestScene::GetDayNumber() const
{
	return dayNumber;
}

void ModuleTestScene::ConstrainCameraToBorders()
{
	float scale = app->win->GetScale();
	fMPoint cam;
	int halfCamW;
	int halfCamH;
	uint auxW;
	uint auxH;
	app->win->GetWindowSize(auxW, auxH);

	halfCamW = auxW * 0.5f;
	halfCamH = auxH * 0.5f;
	cam.x = -app->render->currentCamX;
	cam.y = -app->render->currentCamY;

	fMPoint camCenterPoint;
	camCenterPoint.x = (cam.x + halfCamW) / scale;
	camCenterPoint.y = (cam.y + halfCamH) / scale;

	//camera limits

	if (camCenterPoint.x < mapBordersUpperLeftCorner.x)
	{
		app->render->currentCamX = -(mapBordersUpperLeftCorner.x * scale) + halfCamW;
	}
	else if (camCenterPoint.x > mapBordersBottomRightCorner.x)
	{
		app->render->currentCamX = -(mapBordersBottomRightCorner.x * scale) + halfCamW;
	}

	if (camCenterPoint.y < mapBordersUpperLeftCorner.y)
	{
		app->render->currentCamY = -(mapBordersUpperLeftCorner.y * scale) + halfCamH;
	}
	else if (camCenterPoint.y > mapBordersBottomRightCorner.y)
	{
		app->render->currentCamY = -(mapBordersBottomRightCorner.y * scale) + halfCamH;
	}

}


bool ModuleTestScene::IsNight() const
{
	return isNightTime;
}

void ModuleTestScene::NewCamEasing(fMPoint initialPos, fMPoint finalPos, float duration, EASING_TYPE easingType)
{
	camEasingX.NewEasing(easingType, initialPos.x, finalPos.x, duration);
	camEasingY.NewEasing(easingType, initialPos.y, finalPos.y, duration);

}

bool ModuleTestScene::IsCamDoingEasing() const
{
	return camEasingX.IsActive();
}

void ModuleTestScene::UpdateCamEasing(float dt)
{


	if (IsCamDoingEasing() == true)
	{
		fMPoint ret;
		fMPoint cam;

		ret.x = camEasingX.UpdateEasingAddingTime(dt);
		ret.y = camEasingY.UpdateEasingAddingTime(dt);

		cam = WorldToCam(ret);
		app->render->currentCamX = cam.x;
		app->render->currentCamY = cam.y;

	}



}

void ModuleTestScene::SetCamEasingState(bool active)
{
	camEasingX.ChangeActiveState(active);
	camEasingY.ChangeActiveState(active);
}

void ModuleTestScene::MoveCamTo(fMPoint worldPos, float duration, EASING_TYPE easing)
{
	fMPoint initialPos = CamToWorld();

	NewCamEasing(initialPos, worldPos, duration, easing);
}


fMPoint ModuleTestScene::CamToWorld()
{
	fMPoint ret;
	float invScale = (1 / app->win->GetScale());
	int w;
	int h;

	app->render->GetCameraMeasures(w, h);

	ret.x = (-app->render->currentCamX + w * 0.5) * invScale;
	ret.y = (-app->render->currentCamY + h * 0.5) * invScale;

	return ret;
}


fMPoint ModuleTestScene::WorldToCam(fMPoint worldPos)
{
	fMPoint ret;
	float scale = app->win->GetScale();
	int w;
	int h;

	app->render->GetCameraMeasures(w, h);

	ret.x = -(worldPos.x * scale) + (w * 0.5);
	ret.y = -(worldPos.y * scale) + (h * 0.5);

	return ret;
}


void ModuleTestScene::GetTimer(int& min, int& sec)
{
	int totalTime = 0;

	if (isNightTime == true)
	{
		totalTime = nightTimer;
	}

	else
	{
		totalTime = dayTimer;
	}

	int aux = totalTime - timer;

	//TODO change this division for a multiplication
	min = aux / 60;
	sec = aux - min * 60;
}

int ModuleTestScene::GetNightRectAlpha() const
{
	int alpha = nightRectAlpha.GetLastRequestedPos();
	alpha = MIN(alpha, 255);
	alpha = MAX(alpha, 0);

	return alpha;
}

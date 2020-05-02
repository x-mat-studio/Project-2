#include "App.h"
#include "UIFactory.h"
#include "UI_Group.h"

#include "UI_Image.h"
#include "UI_Text.h"
#include "UI_Scrollbar.h"
#include "Button.h"
#include "ResourcesPortrait.h"

#include "Window.h"
#include "Minimap.h"
#include "Player.h"

UIFactory::UIFactory() :
	portraitBackground{ 643, 145, 72, 56 },
	portraitLevelBackground{ 55, 210, 18, 18 },
	portraitHealthbarBackground{ 34, 22, 52, 10 },
	portraitHealthbarGreenImage{ 29, 79, 50, 8 },
	dataPageBackground{ 400, 435, 139, 67 },
	dataPageImageBackground{ 727, 203, 65, 51 },
	gathererPicture{ 351, 149, 68, 52 },
	gathererShopPicture{ 581, 24, 36, 27 },
	meleePicture{ 561, 149, 68, 52 },
	meleeShopPicture{ 658, 24, 36, 27 },
	rangedPicture{ 147, 149, 68, 52 },
	rangedShopPicture{ 619, 24, 36, 27 },
	baseDataPagePicture{ 634, 78, 68, 62 },
	turretDataPagePicture{ 561, 77, 68, 62 },
	wanamingoDataPagePicture{ 894, 116, 70, 79 },
	turretShopPicture{ 696, 12, 34, 40 },
	resourceIcon{ 18, 209, 11, 19 },
	creditsBackgroundImage{ 563, 237, 117, 122 },
	pauseMenuBackground{ 15, 271, 194, 231 },
	optionsMenuBackground{ 677, 369, 278, 153 },
	creditsMenuBackground{ 15, 271, 194, 231 },
	minimapBackground{ 221, 317, 162, 150 },
	shopBackground{ 15, 271, 194, 231 },
	resourcesBackground{ 415, 435, 65, 30 },

	menuButton{ 17, 12, 195, 36 },
	pauseButton{ 449, 24, 24, 24 },
	closeButton{ 424, 25, 23, 23 },
	fullscreenOnButton{ 739, 27, 23, 23 },
	fullscreenOffButton{ 763, 27, 23, 23 },
	shopButton{ 480, 62, 33, 33 },
	reviveButton{ 653, 54, 46, 14 },

	scrollbarBar{ 272, 45, 90, 4 },
	scrollbarButton{ 257, 15, 13, 34 }
{}


UIFactory::~UIFactory()
{
}

// Menu specific functions

UI_Group* UIFactory::CreateMainMenu()
{
	float x (app->win->width / app->win->GetUIScale() - menuButton.w - 20);
	float y (app->win->height / (app->win->GetUIScale() * 4));

	UI_Group* group = new UI_Group(GROUP_TAG::MAIN_MENU);

	//CreateContinueGameButton(x, y, nullptr, group);

	CreateNewGameButton(x, y + 40, nullptr, group);

	CreateOptionsButton(x, y + 80, nullptr, group);

	CreateCreditsButton(x, y + 120, nullptr, group);

	CreateExitGameButton(x, y + 160, nullptr, group);

	//CreateText(x + 5, y + 5, nullptr, "C O N T I N U E    G A M E", group);
	
	return group;
}


UI_Group* UIFactory::CreateOptionsMenu()
{
	float x((app->win->width / app->win->GetUIScale() / 2) - (optionsMenuBackground.w / 2));
	float y((app->win->height / app->win->GetUIScale() / 2) - (optionsMenuBackground.h / 2));

	UI_Group* group = new UI_Group(GROUP_TAG::OPTIONS_MENU);

	UI* background;
	UI* scrollbar;
	
	background = CreateImage(x, y, nullptr, optionsMenuBackground, group, true);

	scrollbar = CreateImage(20, 60, background, scrollbarBar, group, false);
	CreateMusicScrollbar(0, 0, scrollbar, group);

	scrollbar = CreateImage(140, 60, background, scrollbarBar, group, false);
	CreateSFXScrollbar(0, 0, scrollbar, group);

	CreateFullscreenButton(20, 100, background, group);

	CreateCloseOptionMenuButton(optionsMenuBackground.w - (0.75f * closeButton.w), (0.25f * closeButton.h), background, group);

	CreateText(30, 0, background, "Options", group);

	CreateText(20, 25, background, "Music", group);

	CreateText(140, 25, background, "SFX", group);

	CreateText(20, 75, background, "Fullscreen mode", group);

	return group;
}


UI_Group* UIFactory::CreateCreditsMenu()
{
	float x((app->win->width / app->win->GetUIScale() / 2) - (pauseMenuBackground.w / 2));
	float y((app->win->height / app->win->GetUIScale() / 2) - (pauseMenuBackground.h / 2));
	
	UI_Group* group = new UI_Group(GROUP_TAG::CREDITS_MENU);

	UI* background;

	background = CreateImage(x, y, nullptr, pauseMenuBackground, group, true);

	CreateCloseCreditsButton(pauseMenuBackground.w - (3 * closeButton.w / 4), (1 * closeButton.h / 4), background, group);

	CreateImage(40, 50, background, creditsBackgroundImage, group);

	CreateText(5, 0, background, "Aaron Guerrero Cruz", group);

	CreateText(15, 10, background, "Lead", group);

	CreateText(5, 35, background, "Jose Luis Redondo Tello", group);

	CreateText(15, 45, background, "Code", group);

	CreateText(5, 70, background, "Ferran-Roger Basart i Bosch", group);

	CreateText(15, 80, background, "Management + UI", group);
	
	CreateText(5, 105, background, "Alex Melenchon Maza", group);

	CreateText(15, 115, background, "Design", group);

	CreateText(5, 140, background, "Adria Serrano Lopez", group);

	CreateText(15, 150, background, "Audio + Art", group);

	CreateText(5, 175, background, "Oscar Perez Martin", group);

	CreateText(15, 185, background, "QA", group);

	return group;
}


UI_Group* UIFactory::CreateBasicInGameUI()
{
	float x(app->win->width / app->win->GetUIScale());
	float y(app->win->height / app->win->GetUIScale());

	UI_Group* group = new UI_Group(GROUP_TAG::IN_GAME);

	UI* background;

	//rect = RectConstructor(556, 35, 15, 14);
	//father = AddButton(fMPoint(w / app->win->GetUIScale() - 87, 35), nullptr, UI_TYPE::UI_BUTTON, rect, (P2SString)"PortraitHideButton", EVENT_ENUM::NULL_EVENT, false, false, true, false);
	//AddUIElement(fMPoint(w / app->win->GetUIScale() - 72, 35), nullptr, UI_TYPE::UI_PORTRAIT, rect, P2SString("portraitVector"), nullptr, DRAGGABLE::DRAG_OFF);
	//AddButton(fMPoint(162, h / app->win->GetUIScale() - 85), nullptr, UI_TYPE::UI_BUTTON, rect, P2SString("minimapHideButton"), EVENT_ENUM::NULL_EVENT, false, false, true, false);

	//CreateImage(0, app->minimap->position.y, nullptr, minimapBackground, group);

	CreatePauseGameButton(x - (1.25f) * pauseButton.w, ((1.25f) * pauseButton.w) - pauseButton.w, nullptr, group);

	background = CreateImage(x - 65, y - 97, nullptr, resourcesBackground, group);

	CreateImage(6, 7, background, resourceIcon, group);

	CreateResourcesPortrait(24, 3, background, group);

	CreateImage(x - dataPageBackground.w, y - dataPageBackground.h, nullptr, dataPageBackground, group);

	CreateImage(x - 2 * dataPageImageBackground.w + 12, y - dataPageImageBackground.h - 5, nullptr, dataPageImageBackground, group);

	return group;
}


UI_Group* UIFactory::CreateDataPageComponents()
{
	UI_Group* group = new UI_Group(GROUP_TAG::DATA_PAGE);

	UI* background;

	return group;
}


UI_Group* UIFactory::CreatePauseMenu()
{
	float x((app->win->width / app->win->GetUIScale() / 2) - (pauseMenuBackground.w / 2));
	float y((app->win->height / app->win->GetUIScale() / 2) - (pauseMenuBackground.h / 2));
	
	UI_Group* group = new UI_Group(GROUP_TAG::PAUSE_MENU);
	
	UI* background;

	background = CreateImage(x, y, nullptr, pauseMenuBackground, group, true);

	CreateResumeGameButton(0, 8, background, group);

	CreateSaveGameButton(0, 52, background, group);

	CreateLoadGameButton(0, 97, background, group);

	CreateOptionsButton(0, 142, background, group);

	CreateReturnToMainMenuButton(0, 187, background, group);

	return group;
}


UI_Group* UIFactory::CreateShopMenu()
{
	float x((app->win->width / app->win->GetUIScale() / 2) - (shopBackground.w / 2));
	float y((app->win->height / app->win->GetUIScale() / 2) - (shopBackground.h / 2));

	UI_Group* group = new UI_Group(GROUP_TAG::SHOP_MENU);

	
	UI* background;

	char cost[40];

	background = CreateImage(x, y, nullptr, shopBackground, group, true);

	// Heroes
	CreateText(x + 3, y + 5, background, "H E R O   R E S U R R E C T I O N", group);

	CreateImage(x + 30, y + 35, background, gathererShopPicture, group);

	CreateImage(x + 80, y + 35, background, rangedShopPicture, group);

	CreateImage(x + 130, y + 35, background, meleeShopPicture, group);

	CreateGathererReviveButton(x + 25, y + 65, background, group);

	CreateRangedReviveButton(x + 75, y + 65, background, group);

	CreateMeleeReviveButton(x + 125, y + 65, background, group);

	// Turrets
	CreateText(x + 10, y + 85, background, "T U R R E T   B U I L D I N G", group);

	CreateImage(x + 120, y + 135, background, turretShopPicture, group);

	CreateBuyTurretButton(x + 40, y + 120, background, group);

//	if (lastShop != nullptr)
//	{
//		sprintf_s(cost, 40, "Max. %i", lastShop->GetmaxTurrets());
//		CreateText(x + 95, y + 112, background, cost, group);
//	}
	
//	sprintf_s(cost, 40, "- %i", app->player->GetTurretCost());
//	CreateText(x + 45, y + 130, background, cost, group);

	CreateUpgradeTurretButton(x + 40, y + 170, background, group);

	CreateCloseShopMenuButton(closeButton.w * 0.5f, closeButton.h * 0.5f, background, group);

	return group;
}


// Element specific functions

UI* UIFactory::CreateImage(float x, float y, UI* parent, SDL_Rect rect, UI_Group* group, bool dragable)
{
	UI_Image* uiImage = new UI_Image(x, y, parent, rect, app->uiManager->GetAtlasTexture(), dragable);

	group->AddUiElement(uiImage);

	return uiImage;
}


UI* UIFactory::CreateText(float x, float y, UI* parent, char* text, UI_Group* group, bool interactable)
{
	UI_Text* uiText = new UI_Text(x, y, parent, text, interactable);

	group->AddUiElement(uiText);

	return uiText;
}


UI* UIFactory::CreateNewGameButton(float x, float y, UI* parent, UI_Group* group)
{
	Button* button = new Button(fMPoint{ x, y }, parent, menuButton, false, app->uiManager->GetAtlasTexture(), BUTTON_TAG::NEW_GAME);

	group->AddUiElement(button);

	CreateText(55, 5, button, "N E W  G A M E", group);

	return button;
}


UI* UIFactory::CreateOptionsButton(float x, float y, UI* parent, UI_Group* group)
{
	Button* button = new Button(fMPoint{ x, y }, parent, menuButton, false, app->uiManager->GetAtlasTexture(), BUTTON_TAG::OPTIONS);

	group->AddUiElement(button);

	CreateText(58, 5, button, "O P T I O N S", group);

	return button;
}


UI* UIFactory::CreateFullscreenButton(float x, float y, UI* parent, UI_Group* group)
{
	Button* button;
		
	if (app->win->stateResolution == RESOLUTION_MODE::FULLSCREEN)
	{
		button = new Button(fMPoint{ x, y }, parent, fullscreenOffButton, false, app->uiManager->GetAtlasTexture(), BUTTON_TAG::FULLSCREEN_OFF);
	}
	else
	{
		button = new Button(fMPoint{ x, y }, parent, fullscreenOnButton, false, app->uiManager->GetAtlasTexture(), BUTTON_TAG::FULLSCREEN_ON);
	}

	group->AddUiElement(button);

	return nullptr;
}


UI* UIFactory::CreateCreditsButton(float x, float y, UI* parent, UI_Group* group)
{
	Button* button = new Button(fMPoint{ x, y }, parent, menuButton, false, app->uiManager->GetAtlasTexture(), BUTTON_TAG::CREDITS);

	group->AddUiElement(button);

	CreateText(60, 5, button, "C R E D I T S", group);

	return button;
}


UI* UIFactory::CreateExitGameButton(float x, float y, UI* parent, UI_Group* group)
{
	Button* button = new Button(fMPoint{ x, y }, parent, menuButton, false, app->uiManager->GetAtlasTexture(), BUTTON_TAG::EXIT_GAME);

	group->AddUiElement(button);

	CreateText(50, 5, button, "E X I T    G A M E", group);

	return button;
}


UI* UIFactory::CreatePauseGameButton(float x, float y, UI* parent, UI_Group* group)
{
	Button* button = new Button(fMPoint{ x, y }, parent, pauseButton, false, app->uiManager->GetAtlasTexture(), BUTTON_TAG::PAUSE);

	group->AddUiElement(button);

	return nullptr;
}


UI* UIFactory::CreateResumeGameButton(float x, float y, UI* parent, UI_Group* group)
{
	Button* button = new Button(fMPoint{ x, y }, parent, menuButton, false, app->uiManager->GetAtlasTexture(), BUTTON_TAG::RESUME);

	group->AddUiElement(button);

	CreateText(37, 5, button, "R E S U M E  G A M E", group);

	return button;
}


UI* UIFactory::CreateSaveGameButton(float x, float y, UI* parent, UI_Group* group)
{
	Button* button = new Button(fMPoint{ x, y }, parent, menuButton, false, app->uiManager->GetAtlasTexture(), BUTTON_TAG::SAVE);

	group->AddUiElement(button);

	CreateText(47, 5, button, "S A V E  G A M E", group);
	
	return button;
}


UI* UIFactory::CreateLoadGameButton(float x, float y, UI* parent, UI_Group* group)
{
	Button* button = new Button(fMPoint{ x, y }, parent, menuButton, false, app->uiManager->GetAtlasTexture(), BUTTON_TAG::LOAD);

	group->AddUiElement(button);

	CreateText(48, 5, button, "L O A D  G A M E", group);

	return button;
}


UI* UIFactory::CreateReturnToMainMenuButton(float x, float y, UI* parent, UI_Group* group)
{
	// The event triggered is UNPAUSE_GAME_AND_RETURN_TO_MAIN_MENU
	Button* button = new Button(fMPoint{ x, y }, parent, menuButton, false, app->uiManager->GetAtlasTexture(), BUTTON_TAG::MAIN_MENU);

	group->AddUiElement(button);

	CreateText(48, 5, button, "M A I N  M E N U", group);

	return button;
}



UI* UIFactory::CreateCloseOptionMenuButton(float x, float y, UI* parent, UI_Group* group)
{
	Button* button = new Button(fMPoint{ x, y }, parent, closeButton, false, app->uiManager->GetAtlasTexture(), BUTTON_TAG::CLOSE_OPTIONS_MENU);

	group->AddUiElement(button);

	return button;
}


UI* UIFactory::CreateCloseCreditsButton(float x, float y, UI* parent, UI_Group* group)
{
	Button* button = new Button(fMPoint{ x, y }, parent, closeButton, false, app->uiManager->GetAtlasTexture(), BUTTON_TAG::CLOSE_CREDITS_MENU);

	group->AddUiElement(button);

	return button;
}


UI* UIFactory::CreateClosePauseMenuButton(float x, float y, UI* parent, UI_Group* group)
{
	Button* button = new Button(fMPoint{ x, y }, parent, closeButton, false, app->uiManager->GetAtlasTexture(), BUTTON_TAG::RESUME);

	group->AddUiElement(button);

	return button;
}


UI* UIFactory::CreateCloseShopMenuButton(float x, float y, UI* parent, UI_Group* group)
{
	Button* button = new Button(fMPoint{ x, y }, parent, closeButton, false, app->uiManager->GetAtlasTexture(), BUTTON_TAG::CLOSE_SHOP_MENU);

	group->AddUiElement(button);

	return button;
}


UI* UIFactory::CreateShopButton(float x, float y, UI* parent, UI_Group* group)
{

	group->AddUiElement(nullptr);

	return nullptr;
}


UI* UIFactory::CreateGathererReviveButton(float x, float y, UI* parent, UI_Group* group)
{

	CreateText(x + 3, y - 8, nullptr, "Revive", group);

	group->AddUiElement(nullptr);

	return nullptr;
}



UI* UIFactory::CreateMeleeReviveButton(float x, float y, UI* parent, UI_Group* group)
{

	CreateText(x + 3, y - 8, nullptr, "Revive", group);

	group->AddUiElement(nullptr);

	return nullptr;
}

UI* UIFactory::CreateRangedReviveButton(float x, float y, UI* parent, UI_Group* group)
{

	CreateText(x + 3, y - 8, nullptr, "Revive", group);

	group->AddUiElement(nullptr);

	return nullptr;
}

UI* UIFactory::CreateBuyTurretButton(float x, float y, UI* parent, UI_Group* group)
{

	CreateText(x + 10, y - 8, nullptr, "Buy", group);

	group->AddUiElement(nullptr);

	return nullptr;
}

UI* UIFactory::CreateUpgradeTurretButton(float x, float y, UI* parent, UI_Group* group)
{

	CreateText(x + 5, y - 8, nullptr, "Lvl up", group);

	group->AddUiElement(nullptr);

	return nullptr;
}

UI* UIFactory::CreateMusicScrollbar(float x, float y, UI* parent, UI_Group* group)
{
	UI_Scrollbar* scrollbar = new UI_Scrollbar( x, y, parent, scrollbarButton, app->uiManager->GetAtlasTexture(), 128.0f);

	group->AddUiElement(scrollbar);

	return scrollbar;
}

UI* UIFactory::CreateSFXScrollbar(float x, float y, UI* parent, UI_Group* group)
{
	UI_Scrollbar* scrollbar = new UI_Scrollbar(x, y, parent, scrollbarButton, app->uiManager->GetAtlasTexture(), 455.0f);

	group->AddUiElement(scrollbar);

	return scrollbar;
}


UI* UIFactory::CreateResourcesPortrait(float x, float y, UI* parent, UI_Group* group)
{
	ResourcesPortrait* resourcesPortrait = new ResourcesPortrait(x, y, parent, false);

	group->AddUiElement(resourcesPortrait);

	return resourcesPortrait;
}
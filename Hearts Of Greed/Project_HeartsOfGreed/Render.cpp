#include "p2Defs.h"
#include "p2Log.h"
#include "App.h"
#include "Window.h"
#include "Render.h"
#include "EventManager.h"
#include "Collision.h"
#include "Brofiler/Brofiler/Brofiler.h"
#include "UIManager.h"
#include "Input.h"
#include "TestScene.h"
#include "CameraShake.h"


ModuleRender::ModuleRender() :

	Module(),
	background({ 0,0,0,0 }),
	gameExit(false),
	renderer(nullptr),
	viewport{ 0,0,0,0 },
	camera{ 0,0,0,0 },

	currentCamX(0),
	currentCamY(0),
	cameraOffset(0, 0)
{
	name.create("renderer");
}


// Destructor
ModuleRender::~ModuleRender()
{}


// Called before render is available
bool ModuleRender::Awake(pugi::xml_node& config)
{
	bool ret = true;
	// load flags
	Uint32 flags = SDL_RENDERER_ACCELERATED;
	app->vSyncActivated = false;


	if (config.child("vsync").attribute("value").as_bool(true) == true)
	{
		flags |= SDL_RENDERER_PRESENTVSYNC;
		app->vSyncActivated = true;
	}


	renderer = SDL_CreateRenderer(app->win->window, -1, flags);


	if (renderer == NULL)
	{
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		camera.w = app->win->screenSurface->w;
		camera.h = app->win->screenSurface->h;
		camera.x = 0;
		camera.y = 0;
	}

	app->eventManager->EventRegister(EVENT_ENUM::EXIT_GAME, this);

	return ret;
}


// Called before the first frame
bool ModuleRender::Start()
{
	// back background
	SDL_RenderGetViewport(renderer, &viewport);
	currentCamX = camera.x;
	currentCamY = camera.y;
	SDL_SetRelativeMouseMode(SDL_TRUE);
	//these 2 lines are here to test the 1st map TODO delete
	/*app->render->currentCamX = 1027;
	app->render->currentCamY = -2500;*/
	return true;
}


// Called each loop iteration
bool ModuleRender::PreUpdate(float dt)
{
	SDL_RenderClear(renderer);
	return true;
}


bool ModuleRender::Update(float dt)
{
	return true;
}


bool ModuleRender::PostUpdate(float dt)
{
	//BROFILER_CATEGORY("Render PostUpdate", Profiler::Color::LightYellow);
	cameraOffset = app->cameraShake->GetCameraOffset();

	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.g, background.a);

	SDL_Rect inputRect = { 801,27,18,25 };//default mouse

	if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_STATE::KEY_DOWN || app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_STATE::KEY_REPEAT)
		inputRect = { 849,27,15,21 };

	else if (app->testScene->IsEnabled() == true)
	{
		if (app->input->GetMouseButtonDown(SDL_BUTTON_MIDDLE) == KEY_STATE::KEY_DOWN || app->input->GetMouseButtonDown(SDL_BUTTON_MIDDLE) == KEY_STATE::KEY_REPEAT)
			inputRect = { 870,24,30,30 };
		else if (app->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_STATE::KEY_DOWN || app->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_STATE::KEY_REPEAT)
			inputRect = { 849,53,15,21 };
	}

	int x, y;
	SDL_GetMouseState(&x, &y);
	SDL_Rect outputRect = { x,y,inputRect.w,inputRect.h };
	SDL_RenderCopy(renderer, app->uiManager->GetAtlasTexture(), &inputRect, &outputRect);

	SDL_RenderPresent(renderer);

	CheckListener(this);

	if (gameExit)
	{
		return false;
	}

	return true;
}


// Called before quitting
bool ModuleRender::CleanUp()
{
	SDL_DestroyRenderer(renderer);
	SDL_SetRelativeMouseMode(SDL_FALSE);

	return true;
}


// Load Game State
bool ModuleRender::Load(pugi::xml_node& data)
{
	camera.x = currentCamX = data.child("camera").attribute("x").as_int(0);
	camera.y = currentCamY = data.child("camera").attribute("y").as_int(0);

	return true;
}


// Save Game State
bool ModuleRender::Save(pugi::xml_node& data) const
{
	pugi::xml_node cam = data.append_child("camera");

	cam.append_attribute("x") = camera.x;
	cam.append_attribute("y") = camera.y;

	return true;
}


void ModuleRender::SetBackgroundColor(SDL_Color color)
{
	background = color;
}


void ModuleRender::SetViewPort(const SDL_Rect& rect)
{
	SDL_RenderSetViewport(renderer, &rect);
}


void ModuleRender::ResetViewPort()
{
	SDL_RenderSetViewport(renderer, &viewport);
}

void ModuleRender::ExecuteEvent(EVENT_ENUM eventId)
{
	switch (eventId)
	{
	case EVENT_ENUM::EXIT_GAME:
		gameExit = true;
		break;
	}
}

// Blit to screen
bool ModuleRender::Blit(SDL_Texture* texture, int x, int y, const SDL_Rect* section, bool fliped, bool cameraUse, Uint8 alpha, Uint8 r, Uint8 g, Uint8 b, float additionalScale,
	float pivotX, float pivotY, float speedX, float speedY, double angle, int rotpivot_x, int rotpivot_y)
{
	camera.x = currentCamX + cameraOffset.x;
	camera.y = currentCamY + cameraOffset.y;
	bool ret = true;
	float scale = 2.0f; //TODO THIS IS AN ARBITRARY NUMBER

	SDL_Rect rect;

	if (cameraUse == true)
	{
		scale = app->win->GetScale();
		rect.x = (int)(camera.x * speedX) + (x - (pivotX * additionalScale)) * scale;
		rect.y = (int)(camera.y * speedY) + (y - (pivotY * additionalScale)) * scale;
	}
	else
	{
		rect.x = (int)x * scale;
		rect.y = (int)y * scale;
	}

	if (alpha != 0)
	{
		SDL_SetTextureAlphaMod(texture, alpha);
	}
	if (alpha < 0) {
		alpha = 0;
	}

	if (r != 255 || b != 255 || g != 255)
	{
		SDL_SetTextureColorMod(texture, r, g, b);
	}

	if (section != NULL)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}


	rect.w *= (scale * additionalScale);
	rect.h *= (scale * additionalScale);


	SDL_Point* p = NULL;
	SDL_Point rotPivot;


	if (rotpivot_x != INT_MAX && rotpivot_y != INT_MAX)
	{
		rotPivot.x = rotpivot_x;
		rotPivot.y = rotpivot_y;
		p = &rotPivot;
	}


	if (fliped)
	{
		rect.x = rect.x - rect.w + pivotX;
		rect.y = rect.y - rect.h + pivotY;


		if (SDL_RenderCopyEx(renderer, texture, section, &rect, angle, p, SDL_FLIP_HORIZONTAL) != 0)
		{
			LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
			ret = false;
		}


	}
	else
	{

		if (SDL_RenderCopyEx(renderer, texture, section, &rect, angle, p, SDL_FLIP_NONE) != 0)
		{
			//LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError()); //TODO solve this
			ret = false;
		}
	}

	if (alpha != 0)
	{
		SDL_SetTextureAlphaMod(texture, 255);
	}

	if (r != 255 || b != 255 || g != 255)
	{
		SDL_SetTextureColorMod(texture, 255, 255, 255);
	}


	return ret;
}


bool ModuleRender::MinimapBlit(SDL_Texture* texture, int x, int y, const SDL_Rect* section, float scale) const
{
	bool ret = true;

	SDL_Rect rect;
	rect.x = (int)x * scale;
	rect.y = (int)y * scale;

	if (section != NULL)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w *= scale;
	rect.h *= scale;

	if (SDL_RenderCopyEx(renderer, texture, section, &rect, NULL, NULL, SDL_FLIP_NONE) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}


bool ModuleRender::DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool filled, bool use_camera)
{
	camera.x = currentCamX + cameraOffset.x;
	camera.y = currentCamY + cameraOffset.y;
	bool ret = true;
	float scale = app->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	SDL_Rect rec(rect);


	if (use_camera)
	{
		rec.x = (int)(camera.x + rect.x * scale);
		rec.y = (int)(camera.y + rect.y * scale);
		rec.w *= scale;
		rec.h *= scale;
	}


	int result = (filled) ? SDL_RenderFillRect(renderer, &rec) : SDL_RenderDrawRect(renderer, &rec);


	if (result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}


	return ret;
}


bool ModuleRender::DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera)
{
	camera.x = currentCamX + cameraOffset.x;
	camera.y = currentCamY + cameraOffset.y;
	bool ret = true;
	float scale = app->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;


	if (use_camera)
		result = SDL_RenderDrawLine(renderer, camera.x + x1 * scale, camera.y + y1 * scale, camera.x + x2 * scale, camera.y + y2 * scale);
	else
		result = SDL_RenderDrawLine(renderer, x1 * scale, y1 * scale, x2 * scale, y2 * scale);


	if (result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}


	return ret;
}


bool ModuleRender::DrawCircle(int x, int y, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera)
{
	camera.x = currentCamX + cameraOffset.x;
	camera.y = currentCamY + cameraOffset.y;
	bool ret = true;
	float scale = app->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;
	SDL_Point points[360];

	float factor = (float)M_PI / 180.0f;


	for (uint i = 0; i < 360; ++i)
	{
		points[i].x = (int)(x + radius * cos(i * factor));
		points[i].y = (int)(y + radius * sin(i * factor));
	}


	result = SDL_RenderDrawPoints(renderer, points, 360);


	if (result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}


	return ret;
}

void ModuleRender::GetCameraMeasures(int& w, int& h)
{
	w = camera.w;
	h = camera.h;
}

void ModuleRender::AssignCameraMeasures()
{
	camera.w = app->win->screenSurface->w;
	camera.h = app->win->screenSurface->h;
}

const int ModuleRender::GetCameraX()
{
	return camera.x;
}

const int ModuleRender::GetCameraY()
{
	return camera.y;
}


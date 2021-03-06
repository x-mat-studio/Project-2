#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "Module.h"

struct SDL_Window;
struct SDL_Surface;

enum class RESOLUTION_MODE
{
	FULLSCREEN, //use this for fullscreen
	BORDERLESS,
	RESIZABLE,
	FULLSCREEN_WINDOW,
	STATIC //this for windowed
};

class ModuleWindow : public Module
{
public:

	ModuleWindow();

	// Destructor
	virtual ~ModuleWindow();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before quitting
	bool CleanUp();

	//Called each loop
	bool Update(float dt);

	//Change screen resolution functions
	SDL_Window* ResizeWindow(RESOLUTION_MODE stateResolution);

	bool AssignSurface(SDL_Window* window);

	bool ChangeWindow(RESOLUTION_MODE stateResolution);

	int SetResolutionFlag(RESOLUTION_MODE stateResolution);

	bool ChangeResolution(RESOLUTION_MODE newResolution);

	// Changae title
	void SetTitle(const char* new_title);

	// Retrive window size
	void GetWindowSize(uint& width, uint& height) const;

	// Retrieve window scale
	float GetScale() const;
	float GetScaleFactor()const;
	void GetScaleRange(float& minScale, float& maxScale)const;
	float GetUIScale() const;

	float SetScale(float newScale);

	float  AddScale(float addedScale);

private:

	void ExecuteEvent(EVENT_ENUM eventId);

public:
	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screenSurface;

	uint		width;
	uint		height;

	RESOLUTION_MODE stateResolution;
	

private:
	P2SString	title;
	float		scale;
	float minScaleValue;
	float maxScaleValue;
	float scaleFactor;
};

#endif // __WINDOW_H__
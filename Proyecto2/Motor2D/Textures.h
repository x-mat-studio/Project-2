#ifndef __TEXTURES_H__
#define __TEXTURES_H__

#include "Module.h"
#include <vector>

struct SDL_Texture;
struct SDL_Surface;

class ModuleTextures : public Module
{
public:

	ModuleTextures();

	// Destructor
	virtual ~ModuleTextures();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called before quitting
	bool CleanUp();

	// Load Texture
	SDL_Texture* const	Load(const char* path);
	SDL_Texture* const	LoadSurface(SDL_Surface* surface);
	bool				UnLoad(SDL_Texture* texture);
	void				GetSize(const SDL_Texture* texture, uint& width, uint& height) const;

public:

	std::vector<SDL_Texture*>	textures;
	
};

class TexturesListener : public Listener
{
private:

	void onNotify(const Module& module, Event event);

};

class TexturesSpeaker : public Speaker
{
private:
	Listener* listeners_[MAX_LISTENERS]; //Array, we're not expecting to add/remove a lot

	int numListeners;

};

#endif // __TEXTURES_H__
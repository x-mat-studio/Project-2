#ifndef __AUDIO_H__
#define __AUDIO_H__

#include "Module.h"
#include "SDL_mixer\include\SDL_mixer.h"

#define DEFAULT_MUSIC_FADE_TIME 2.0f
#define MAX_VOLUME 128
#define MAX_SFX 75

struct _Mix_Music;

struct Mix_Chunk;

enum class DIRECTION {
	FRONT,
	FRONT_RIGHT,
	RIGHT,
	BACK_RIGHT,
	BACK,
	BACK_LEFT,
	LEFT,
	FRONT_LEFT
};

enum LOUDNESS {
	LOUD,
	NORMAL,	
	QUIET,
	SILENCE
};

class ModuleAudio : public Module
{
public:
	ModuleAudio();


	// Destructor
	virtual ~ModuleAudio();


	// Called before render is available
	bool Awake(pugi::xml_node&);


	// Called before quitting
	bool CleanUp();


	// Play a music file
	bool PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME, int volume = 180, int loop = -1);


	// Load a WAV in memory
	unsigned int LoadFx(const char* path);


	// Play a previously loaded WAV
	bool PlayFx(unsigned int fx, int repeat = 0, int channel = 1, LOUDNESS loudness = LOUDNESS::LOUD, DIRECTION direction = DIRECTION::FRONT, bool overrideChannel = false);

	void SetVolume(float volume);

	//Configure Channel
	bool ConfigureChannel(unsigned int channel, int volume, float angle);

	bool ConfigureChannel(unsigned int channel,LOUDNESS loudness, DIRECTION direction);

	void SilenceAllChannels(int id);

	void SilenceAll();

private:

	P2SString fxFolder;

	_Mix_Music* music;

	
	std::vector<Mix_Chunk*>	fx;  //We need a Array of Numbers: sfx are stored as unsigned int

public:

	int volumeAdjustment;
	int musicVolume;

};

#endif // __AUDIO_H__
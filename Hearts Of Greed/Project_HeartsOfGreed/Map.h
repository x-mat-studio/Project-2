#ifndef __MAP_H__
#define __MAP_H__

#include "PugiXml/src/pugixml.hpp"

#include "Module.h"
#include "SDL/include/SDL.h"

// ----------------------------------------------------

struct Properties
{
	P2SString name;
	int value;

	Properties();
};
struct MapLayer
{
	P2SString name;
	uint width;
	uint height;
	uint* gid;
	std::vector<Properties*> layerPropVector;
	MapLayer();
	~MapLayer();
};

struct Object
{
	SDL_Rect boundingBox;
	int type;
	uint id;

	Object();
};



struct ObjectGroup
{
	uint id;
	P2SString name;
	std::vector<Object*> objectVector;
	std::vector<Properties*> propVector;

	ObjectGroup();
};

// ----------------------------------------------------
struct TileSet
{

	P2SString			name;

	SDL_Texture*		texture;

	int					firstGid;
	int					margin;
	int					spacing;
	int					tileWidth;
	int					tileHeight;
	int					texWidth;
	int					texHeight;
	int					numTilesWidth;
	int					numTilesHeight;
	int					offsetX;
	int					offsetY;
	int					columns;

	TileSet();
	~TileSet();
};

enum class MAP_TYPES
{
	MAP_TYPE_UNKNOWN = 0,
	MAP_TYPE_ORTHOGONAL,
	MAP_TYPE_ISOMETRIC,
	MAP_TYPE_STAGGERED
};
// ----------------------------------------------------
struct MapData
{
	int					width;
	int					height;
	int					tileWidth;
	int					tileHeight;
	SDL_Color			backgroundColor;
	MAP_TYPES			type;
	std::vector<TileSet*>	tilesets;
	std::vector<MapLayer*>   layers;
	std::vector<ObjectGroup*> objGroups;
	P2SString           name;
	P2SString           path;
	P2SString			musicPath;

	MapData();
};

// ----------------------------------------------------
class ModuleMap : public Module
{
public:

	ModuleMap();

	// Destructor
	virtual ~ModuleMap();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
	void Draw();
	
	void DrawMinimap();

	// Called before quitting
	bool CleanUp();

	//Save/Load
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	// Load new map
	bool LoadNew(const char* path);
	bool ReloadMap(P2SString newmap);
	inline uint Get(int x, int y, MapLayer* currentlayer) const;
	void MapToWorldCoords(int posX, int posY, MapData& dat, float& outX, float& outY);
	void WorldToMapCoords(int x, int y, MapData& dat, int& outX, int& outY) const;
	iMPoint MapToWorld(int x, int y) const;
	iMPoint WorldToMap(float x, float y) const;
	bool InsideCamera(float& posX, float& posY) const;
	bool EntityInsideCamera(float& posX, float& posY, float& w, float& h) const;
	bool CreateWalkabilityMap(int& width, int& height, uchar** buffer);

private:

	bool LoadMap(P2SString path, P2SString name);

	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool UnloadTilesetImage(SDL_Texture* texture);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadObjGroup(pugi::xml_node& node, ObjectGroup* group);

	SDL_Rect RectFromTileId(uint tile_id, TileSet* currenttileset);

	TileSet* GetTilesetFromTileId(int id) const;

public:

	MapData data;

	P2SString mapName;
private:

	pugi::xml_document mapFile;
	P2SString folder;

	bool mapLoaded;
	
};

#endif // __MAP_H__
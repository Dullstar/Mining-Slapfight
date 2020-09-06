#pragma once
#include <allegro5/allegro.h>
#include "Tileset.h"
#include <vector>
#include <memory>
#include <string>

class WorldGen;
// In the interest of not having absurd numbers of files, we're going to put all the object classes
// together...

struct Stats
{
public:
	int evade;
	int armor;
	int attack;
	int hit;
	int hp;
	int move;
	bool action;
	Stats();
};

class ObjectGraphic
{
private:
	ALLEGRO_BITMAP* bitmap;
public:
	ObjectGraphic(const char* filename);
	~ObjectGraphic();
	void draw(int objectX, int objectY, int screenX, int screenY);
};

typedef std::shared_ptr<ObjectGraphic> OG;

struct ObjectGraphicPointerCollection
{
public:
	OG main_building_player_1;
	OG main_building_player_2;
	OG resource_wood;
	OG resource_iron;
	OG resource_gold;
	OG mine_wood_player_1;
	OG mine_ore_player_1;
	OG mine_wood_player_2;
	OG mine_ore_player_2;
	OG city_player_1;
	OG city_player_2;
	OG cursor_friendly;
	OG cursor_neutral;
	OG cursor_enemy;
	OG factory_player_1;
	OG factory_player_2;
	OG ranger_bot_player_1;
	OG ranger_bot_player_2;
	OG puncher_bot_player_1;
	OG puncher_bot_player_2;
	OG worker_player_1;
	OG worker_player_2;
	ObjectGraphicPointerCollection(Tileset& tileset);
};

typedef ObjectGraphicPointerCollection OGPC;

enum class object_type
{
	object,
	resource_node,
	factory,
	main_building,
	ranger_bot,
	puncher_bot,
	worker
};

class Object
{
protected:
	std::shared_ptr<ObjectGraphic> graphic;
	int x;
	int y;
	char player;
	object_type objectType;
	static std::vector<int> collision_area_1x1(int _x, int _y);
	static std::vector<int> collision_area_2x2(int _x, int _y);
	static std::vector<int> collision_area_3x3(int _x, int _y);
	static std::vector<int> catchment_area_1x1(int _x, int _y);
	static std::vector<int> catchment_area_2x2(int _x, int _y);
	static std::vector<int> catchment_area_3x3(int _x, int _y);
public:
	Object(int x, int y, char player, OG graphic, std::string name);
	virtual ~Object() = default;
	virtual void draw(int screenX, int screenY);
	virtual void ui_draw(int x, int y);
	int get_x();
	int get_y();
	char get_player();
	object_type get_type();
	virtual std::vector<int> collision_area() = 0;
	virtual std::vector<int> catchment_area() = 0;
	virtual std::vector<int> get_moves();
	void move(int new_x, int new_y);
	void move_relative(int dx, int dy);
	std::string name;
	Stats stats;
	friend class WorldGen;
};

typedef std::vector<std::unique_ptr<Object>> vecObject;
class MainBuilding : public Object
{
private:
	OG graphic2;
public:
	MainBuilding(int x, int y, char player, OG graphic);
	std::vector<int> collision_area() override;
	std::vector<int> catchment_area() override;
};

enum class Resource
{
	iron,
	gold,
	wood
};

class ResourceNode : public Object
{
private:
	Resource type;
	OG mine_graphic;
	OG mine_graphicP2;
	int amount;
public:
	ResourceNode(int x, int y, Resource _type, int _amount, OG _graphic, OG _mine_graphic1, OG _mine_graphic2);
	std::vector<int> collision_area() override;
	std::vector<int> catchment_area() override;
	void draw(int screenX, int screenY) override;
	void ui_draw(int x, int y) override;
	void create_mine(char player);
	void capture_mine(char player);
	Resource get_node_type();
	int get_value();
};
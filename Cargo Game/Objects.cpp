#include "Objects.h"
#include "constants.h"
#include <iostream>

ObjectGraphic::ObjectGraphic(const char* filename)
	: bitmap(al_load_bitmap(filename))
{
}

ObjectGraphic::~ObjectGraphic()
{
	al_destroy_bitmap(bitmap);
}

// THIS FUNCTION DOES NOT OPERATE IN TILE SPACE!
// It's easy to forget when implementing new objects, but we don't want to have to use
// a separate function if we want to implement, for example, smooth movement of mobile
// objects between tiles!
// (Hint: use constants::tilesize() if your coordinates are in tile space)
void ObjectGraphic::draw(int objectX, int objectY, int screenX, int screenY)
{
	al_draw_bitmap(bitmap, objectX - screenX, objectY - screenY, 0);
}



ObjectGraphicPointerCollection::ObjectGraphicPointerCollection(Tileset& tileset)
{
	main_building_player_1 = std::make_shared<ObjectGraphic>("graphics/p1mainbuilding.png");
	main_building_player_2 = std::make_shared<ObjectGraphic>("graphics/p2mainbuilding.png");
	resource_wood = std::make_shared<ObjectGraphic>("graphics/resource_tree.png");
	resource_gold = std::make_shared<ObjectGraphic>("graphics/resource_gold.png");
	resource_iron = std::make_shared<ObjectGraphic>("graphics/resource_iron.png");
	mine_ore_player_1 = std::make_shared<ObjectGraphic>("graphics/p1_mine_ore.png");
	mine_ore_player_2 = std::make_shared<ObjectGraphic>("graphics/p2_mine_ore.png");
	mine_wood_player_1 = std::make_shared<ObjectGraphic>("graphics/p1_mine_wood.png");
	mine_wood_player_2 = std::make_shared<ObjectGraphic>("graphics/p2_mine_wood.png");
	city_player_1 = std::make_shared<ObjectGraphic>("graphics/p1city.png");
	city_player_2 = std::make_shared<ObjectGraphic>("graphics/p2city.png");
	cursor_friendly = std::make_shared<ObjectGraphic>("graphics/cursor_friendly.png");
	cursor_neutral = std::make_shared<ObjectGraphic>("graphics/cursor_neutral.png");
	cursor_enemy = std::make_shared<ObjectGraphic>("graphics/cursor_enemy.png");
	factory_player_1 = std::make_shared<ObjectGraphic>("graphics/p1factory.png");
	factory_player_2 = std::make_shared<ObjectGraphic>("graphics/p2factory.png");
	ranger_bot_player_1 = std::make_shared<ObjectGraphic>("graphics/p1rangerobot.png");
	ranger_bot_player_2 = std::make_shared<ObjectGraphic>("graphics/p2rangerobot.png");
	puncher_bot_player_1 = std::make_shared<ObjectGraphic>("graphics/p1puncherbot.png");
	puncher_bot_player_2 = std::make_shared<ObjectGraphic>("graphics/p2puncherbot.png");
	worker_player_1 = std::make_shared<ObjectGraphic>("graphics/p1workerbot.png");
	worker_player_2 = std::make_shared<ObjectGraphic>("graphics/p2workerbot.png");
}

std::vector<int> Object::collision_area_1x1(int _x, int _y)
{
	return std::vector<int>{ constants::get_index(_x, _y) };
}

std::vector<int> Object::collision_area_2x2(int _x, int _y)
{
	std::vector<int> output
	{
		constants::get_index(_x, _y),
		constants::get_index(_x + 1, _y),
		constants::get_index(_x, _y + 1),
		constants::get_index(_x + 1, _y + 1)
	};
	return output;
}

std::vector<int> Object::collision_area_3x3(int _x, int _y)
{
	std::vector<int> output
	{
		constants::get_index(_x, _y),
		constants::get_index(_x + 1, _y),
		constants::get_index(_x + 2, _y),
		constants::get_index(_x, _y + 1),
		constants::get_index(_x + 1, _y + 1),
		constants::get_index(_x + 2, _y + 1),
		constants::get_index(_x, _y + 2),
		constants::get_index(_x + 1, _y + 2),
		constants::get_index(_x + 2, _y + 2)
	};
	return output;
}

std::vector<int> Object::catchment_area_1x1(int _x, int _y)
{
	std::vector<int> output
	{
		constants::get_index(_x - 1, _y),
		constants::get_index(_x + 1, _y),
		constants::get_index(_x, _y - 1),
		constants::get_index(_x, _y + 1)
	};
	return output;
}

std::vector<int> Object::catchment_area_2x2(int _x, int _y)
{
	std::vector<int> output
	{
		constants::get_index(_x - 1, _y),
		constants::get_index(_x - 1, _y + 1),
		constants::get_index(_x, _y - 1),
		constants::get_index(_x + 1, _y - 1),
		constants::get_index(_x + 2, _y),
		constants::get_index(_x + 2, _y + 1),
		constants::get_index(_x, _y + 2),
		constants::get_index(_x + 1, _y + 2)
	};
	return output;
}

std::vector<int> Object::catchment_area_3x3(int _x, int _y)
{
	std::vector<int> output
	{
		constants::get_index(_x - 1, _y),
		constants::get_index(_x - 1, _y + 1),
		constants::get_index(_x - 1, _y + 2),
		constants::get_index(_x, _y - 1),
		constants::get_index(_x + 1, _y - 1),
		constants::get_index(_x + 2, _y - 1),
		constants::get_index(_x + 3, _y),
		constants::get_index(_x + 3, _y + 1),
		constants::get_index(_x + 3, _y + 2),
		constants::get_index(_x, _y + 3),
		constants::get_index(_x + 1, _y + 3),
		constants::get_index(_x + 2, _y + 3),
	};
	return output;
}

Object::Object(int x, int y, char player, OG graphic, std::string name)
	: x(x), y(y), player(player), graphic(graphic), name(name), objectType(object_type::object)
{
}

void Object::draw(int screenX, int screenY)
{
	graphic->draw(constants::tilesize() * x, constants::tilesize() * y, screenX, screenY);
}

void Object::ui_draw(int x, int y)
{
	graphic->draw(x, y, 0, 0);
}

int Object::get_x() { return x; }

int Object::get_y() { return y; }

char Object::get_player()
{
	return player;
}

object_type Object::get_type()
{
	return objectType;
}

// Returns an empty vector usually. Units overrides this.
// This function exists purely for convenient usage of the object interface.
// The default is thus intended to do basically nothing.
std::vector<int> Object::get_moves()
{
	return std::vector<int>();
}

void Object::move(int new_x, int new_y)
{
	x = new_x;
	y = new_y;
}

void Object::move_relative(int dx, int dy)
{
	x += dx;
	y += dy;
}


MainBuilding::MainBuilding(int x, int y, char player, OG graphic)
	: Object(x, y, player, graphic, "Main Factory")
{
	objectType = object_type::main_building;
	stats.armor = 10;
	stats.hp = 100;
}

std::vector<int> MainBuilding::collision_area()
{
	return collision_area_3x3(x, y);
}

std::vector<int> MainBuilding::catchment_area()
{
	return catchment_area_3x3(x, y);
}

ResourceNode::ResourceNode(int x, int y, Resource _type, int _amount, OG _graphic, OG _mine_graphic1, OG _mine_graphic2)
	: Object(x, y, 0, _graphic, "???"), type(_type), amount(_amount), mine_graphic(_mine_graphic1), mine_graphicP2(_mine_graphic2)
{
	objectType = object_type::resource_node;
	if (type == Resource::wood) name = "Wood";
	else if (type == Resource::iron) name = "Iron";
	else if (type == Resource::gold) name = "Gold";
}

std::vector<int> ResourceNode::collision_area()
{
	return collision_area_1x1(x, y);
}

std::vector<int> ResourceNode::catchment_area()
{
	return catchment_area_1x1(x, y);
}

void ResourceNode::draw(int screenX, int screenY)
{
	graphic->draw(constants::tilesize() * x, constants::tilesize() * y, screenX, screenY);
	if (player == 1) mine_graphic->draw(constants::tilesize() * x, constants::tilesize() * y, screenX, screenY);
	else if (player == 2) mine_graphicP2->draw(constants::tilesize() * x, constants::tilesize() * y, screenX, screenY);
}

void ResourceNode::ui_draw(int x, int y)
{
	graphic->draw(x, y, 0, 0);
	if (player == 1) mine_graphic->draw(x, y, 0, 0);
	else if (player == 2) mine_graphicP2->draw(x, y, 0, 0);
}

void ResourceNode::create_mine(char _player)
{
	if (type == Resource::wood) name = "Sawmill";
	else if (type == Resource::iron) name = "Iron Mine";
	else if (type == Resource::gold) name = "Gold Mine";
	player = _player;
}

void ResourceNode::capture_mine(char _player)
{
	player = _player;
}

Resource ResourceNode::get_node_type()
{
	return type;
}

int ResourceNode::get_value()
{
	return amount;
}

Stats::Stats()
	: evade(0), armor(0), attack(0), hit(0), hp(0), move(0), action(false)
{
}
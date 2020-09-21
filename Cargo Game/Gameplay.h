#pragma once
#include "GameState.h"
#include <vector>
#include "Tileset.h"
#include "Controller.h"
#include "Objects.h"
#include <memory>
#include "UI.h"

class Gameplay : public GameState
{
private:
	int sizeX;
	int sizeY;
	std::vector<char> terrain;
	std::vector<char> track;
	std::vector<std::unique_ptr<Object>> objects;
	vecObject units;
	OGPC ogPointers;
	Tileset terrain_tiles;
	Tileset track_tiles;
	int animate_water;
	char water_frame;
	UI ui;
	std::unique_ptr<ActionsMenu> actions_menu;
	std::unique_ptr<AttackScreen> attack_screen;
	RNG rng;
	std::unique_ptr<VictoryScreen> victory_screen;
public:
	Gameplay(int _sizeX, int _sizeY);
	~Gameplay();
	void update(
		std::array<bool, Controller::total_commands>& inputs, 
		std::array<bool, Controller::total_commands>& key_downs,
		std::array<bool, Controller::total_commands>& key_chars) override;
	void draw() override;
};
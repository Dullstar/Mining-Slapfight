#pragma once
#include "Controller.h"
class GameState
{
public:
	GameState();
	virtual ~GameState();
	virtual void update(
		std::array<bool, Controller::total_commands>& inputs, 
		std::array<bool, Controller::total_commands>& key_downs,
		std::array<bool, Controller::total_commands>& key_chars) = 0;
	virtual void draw(ALLEGRO_BITMAP* buffer1, ALLEGRO_BITMAP* buffer2, ALLEGRO_BITMAP* buffer3) = 0;
};


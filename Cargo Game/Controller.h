#pragma once
#include <allegro5/allegro.h>
#include <array>

enum class command
{
	up = 0,
	right = 1,
	down = 2,
	left = 3,
	cursor_up = 4,
	cursor_right = 5,
	cursor_down = 6,
	cursor_left = 7,
	select = 8,
	unselect = 9,
	none
};



class Controller
{
public:
	static const int total_commands = 10;
	static const int total_keys = ALLEGRO_KEY_MAX;
	static const unsigned char KEY_SEEN = 1;
	static const unsigned char KEY_RELEASED = 2;
	Controller();
	std::array<bool, total_commands> interpret(std::array<unsigned char, Controller::total_keys>& inputs);
	std::array<bool, Controller::total_commands> interpret_keydown(std::array<unsigned char, Controller::total_keys>& inputs);
	std::array<bool, Controller::total_commands> interpret_keychar(std::array<unsigned char, Controller::total_keys>& inputs);
private:
	std::array<command, total_keys> translation_table;
};


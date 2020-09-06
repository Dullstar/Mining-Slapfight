#include "Controller.h"
#include "constants.h"
#include "MainGame.h"
#include <iostream>


Controller::Controller()
{
	for (auto& entry : translation_table)
	{
		entry = command::none;
	}
	// Rebinding isn't currently supported, but it's designed so that this piece of the code
	// is the only thing that will have to change (well, not including whatever we need to add)

	// Assign commands to some specific keys
	translation_table[ALLEGRO_KEY_UP] = command::cursor_up; //command::up;
	translation_table[ALLEGRO_KEY_RIGHT] = command::cursor_right; //command::right;
	translation_table[ALLEGRO_KEY_DOWN] = command::cursor_down; //command::down;
	translation_table[ALLEGRO_KEY_LEFT] = command::cursor_left; //command::left;
	translation_table[ALLEGRO_KEY_W] = command::cursor_up;
	translation_table[ALLEGRO_KEY_A] = command::cursor_left;
	translation_table[ALLEGRO_KEY_S] = command::cursor_down;
	translation_table[ALLEGRO_KEY_D] = command::cursor_right;
	translation_table[ALLEGRO_KEY_LSHIFT] = command::select;
	translation_table[ALLEGRO_KEY_LCTRL] = command::unselect;
	translation_table[ALLEGRO_KEY_RSHIFT] = command::select;
	translation_table[ALLEGRO_KEY_RCTRL] = command::unselect;
}

// Warning: This method alters the raw inputs sent in,
// by updating their seen/released bits with &= KEY_SEEN
std::array<bool, Controller::total_commands> Controller::interpret(std::array<unsigned char, Controller::total_keys>& inputs)
{
	std::array<bool, total_commands> commands{ false };
	for (int i = 0; i < total_keys; i++)
	{
		if (inputs[i] && translation_table[i] != command::none)
		{
			commands[(int)translation_table[i]] = true;
		}
		inputs[i] &= KEY_SEEN;
	}
	return commands;
}

// Warning: clears the raw inputs sent in.
std::array<bool, Controller::total_commands> Controller::interpret_keydown(std::array<unsigned char, Controller::total_keys>& inputs)
{
	std::array<bool, total_commands> commands{ false };
	for (int i = 0; i < total_keys; i++)
	{
		if (inputs[i] && translation_table[i] != command::none)
		{
			commands[(int)translation_table[i]] = true;
		}
		inputs[i] = false;
	}
	return commands;
}

// Warning: clears the raw inputs sent in.
std::array<bool, Controller::total_commands> Controller::interpret_keychar(std::array<unsigned char, Controller::total_keys>& inputs)
{
	std::array<bool, total_commands> commands{ false };
	for (int i = 0; i < total_keys; i++)
	{
		if (inputs[i] && translation_table[i] != command::none)
		{
			commands[(int)translation_table[i]] = true;
		}
		inputs[i] = false;
	}
	return commands;
}
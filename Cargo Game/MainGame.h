#pragma once

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include "Gameplay.h"
#include "GameState.h"
#include "Controller.h"
// Override base class with your custom functionality
class MainGame
{
private:
	ALLEGRO_TIMER* timer;
	ALLEGRO_EVENT_QUEUE* queue;
	ALLEGRO_DISPLAY* display;
	ALLEGRO_FONT* font;
	Gameplay gameplay;
	GameState* currentState;
	Controller controller;
	std::array<bool, Controller::total_commands> translated_inputs;
	std::array<bool, Controller::total_commands> translated_key_downs;
	std::array<bool, Controller::total_commands> translated_key_chars;
	std::array<unsigned char, Controller::total_keys> raw_inputs;
	std::array<unsigned char, Controller::total_keys> raw_key_downs;
	std::array<unsigned char, Controller::total_keys> raw_key_chars;
	ALLEGRO_BITMAP* buffer1;
	ALLEGRO_BITMAP* buffer2;
	ALLEGRO_BITMAP* buffer3;
public:
	MainGame(int x, int y);
	~MainGame();
	void GameLoop();
};

void initialize(bool test, const char* what);
int main();
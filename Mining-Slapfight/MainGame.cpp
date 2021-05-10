#include "MainGame.h"
#include "constants.h"
#include "Config.h"
#include <iostream>


void initialize(bool test, const char* what)
{
	if (!test)
	{
		std::cerr << "Error: Failed to initialize " << what << std::endl;
		exit(EXIT_FAILURE);
	}
}

MainGame::MainGame(int x, int y)
	: gameplay(constants::mapsizeX(), constants::mapsizeY()), currentState(&gameplay), 
	raw_inputs{ 0 }, translated_inputs{ 0 }, raw_key_downs{ 0 }, translated_key_downs{ 0 },
	raw_key_chars{ 0 }, translated_key_chars{ 0 }
{
	timer = al_create_timer(1.0 / 60.0);
	initialize(timer, "timer");
	queue = al_create_event_queue();
	initialize(queue, "event queue");

	display = al_create_display(x * constants::pixelsize() + constants::ui_size_x() * constants::pixelsize(), y * constants::pixelsize());
	initialize(display, "display");
	font = al_create_builtin_font();
	constants::font = font;
	initialize(font, "font");

	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_display_event_source(display));
	al_register_event_source(queue, al_get_timer_event_source(timer));

	al_start_timer(timer);

	// Buffers explanation: Buffer 1: play area during gameplay size
	//                      Buffer 2: ui area during gameplay size
	//                      Buffer 3: whole screen
	// Buffers 1 and 2 are non-overlapping, buffer 3 overlaps everything.
	constants::buffer1 = al_create_bitmap(x, y);
	constants::buffer2 = al_create_bitmap(constants::ui_size_x(), y);
	constants::buffer3 = al_create_bitmap((x + constants::ui_size_x()), y);
}

MainGame::~MainGame()
{
	al_destroy_font(font);
	al_destroy_display(display);
	al_destroy_timer(timer);
	al_destroy_event_queue(queue);
	al_destroy_bitmap(constants::buffer1);
	al_destroy_bitmap(constants::buffer2);
}

void MainGame::GameLoop()
{
	bool redraw = false;
	int ticks = 0;
	ALLEGRO_EVENT event;
	while (true)
	{
		al_wait_for_event(queue, &event);

		switch (event.type)
		{
		case ALLEGRO_EVENT_TIMER:
			ticks++;
			if (ticks == 1)
			{
				translated_inputs = controller.interpret(raw_inputs);
				translated_key_downs = controller.interpret_keydown(raw_key_downs);
				translated_key_chars = controller.interpret_keychar(raw_key_chars);
				currentState->update(translated_inputs, translated_key_downs, translated_key_chars);
				redraw = true;
			}
			break;
		case ALLEGRO_EVENT_KEY_DOWN:
			raw_inputs[event.keyboard.keycode] = Controller::KEY_SEEN | Controller::KEY_RELEASED;
			raw_key_downs[event.keyboard.keycode] = true;
			break;
		case ALLEGRO_EVENT_KEY_CHAR:
			raw_key_chars[event.keyboard.keycode] = true;
			break;
		case ALLEGRO_EVENT_KEY_UP:
			raw_inputs[event.keyboard.keycode] &= Controller::KEY_RELEASED;
			break;
		case ALLEGRO_EVENT_DISPLAY_CLOSE:
			return;
		}

		if (redraw && al_is_event_queue_empty(queue))
		{
			al_set_target_bitmap(constants::buffer1);
			al_clear_to_color(al_map_rgb(0, 0, 0));
			currentState->draw();
			al_set_target_bitmap(al_get_backbuffer(display));
			al_draw_scaled_bitmap(constants::buffer1,
				0, 0, 
				constants::screensizeX(), constants::screensizeY(),
				0, 0, 
				constants::screensizeX() * constants::pixelsize(), constants::screensizeY() * constants::pixelsize(), 
				0);
			al_draw_scaled_bitmap(constants::buffer2,
				0, 0,
				constants::ui_size_x(), constants::screensizeY(),
				constants::screensizeX() * constants::pixelsize(), 0,
				constants::ui_size_x() * constants::pixelsize(), constants::screensizeY() * constants::pixelsize(),
				0);
			al_draw_scaled_bitmap(constants::buffer3,
				0, 0,
				constants::screensizeX() + constants::ui_size_x(), constants::screensizeY(),
				0, 0,
				(constants::screensizeX() + constants::ui_size_x()) * constants::pixelsize(),
				constants::screensizeY() * constants::pixelsize(),
				0);
			al_flip_display();
			ticks = 0;
		}
	}
}

// ENTRY POINT
int main()
{
	initialize(al_init(), "Allegro");
	initialize(al_install_keyboard(), "keyboard.");
	initialize(al_init_image_addon(), "Allegro image add-on.");
	initialize(al_init_primitives_addon(), "Allegro primitives add-on.");
	{
		Config config(8, 3, 30 * 8, 27 * 8, (48 - 30) * 8); // 48 for 1920 x 1080 cleanly; would need 5 pixel size.
		config.apply();
	}
	MainGame Game(constants::screensizeX(), constants::screensizeY());
	Game.GameLoop();
	return 0;
}

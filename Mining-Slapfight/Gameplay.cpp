#include "Gameplay.h"
#include "constants.h"
#include "Controller.h"
#include "MainGame.h"
#include "WorldGen.h"
#include "Units.h"
#include <iostream>

Gameplay::Gameplay(int _sizeX, int _sizeY)
	: sizeX(_sizeX), sizeY(_sizeY),
	terrain_tiles("graphics/Terrain.png"),
	track_tiles("graphics/Tracks.png"),
	animate_water(0),
	ogPointers(ObjectGraphicPointerCollection(terrain_tiles)),
	water_frame(0),
	track(sizeX * sizeY, -1),
	ui(ogPointers, objects, units),
	actions_menu(nullptr),
	attack_screen(nullptr),
	victory_screen(nullptr)
{
	while (true)
	{
		terrain = WorldGen::create(sizeX, sizeY);
		try
		{
			objects = WorldGen::create_objects(terrain, ogPointers);
		}
		catch (const char* error)
		{
			// Essentially, if our terrain doesn't allow the objects to get placed properly,
			// just generate some new terrain.
			std::cerr << error << std::endl;
			continue;
		}
		break;
	}
	ui.scrollX = (objects[0]->get_x() * constants::tilesize())
		+ (3 * constants::tilesize() / 2)
		- (constants::screensizeX() / 2);
	ui.scrollY = (objects[0]->get_y() * constants::tilesize()) 
		+ (3 * constants::tilesize() / 2)
		- (constants::screensizeY() / 2);

	ui.p2_scX = (objects[1]->get_x() * constants::tilesize())
		+ (3 * constants::tilesize() / 2)
		- (constants::screensizeX() / 2);
	ui.p2_scY = (objects[1]->get_y() * constants::tilesize())
		+ (3 * constants::tilesize() / 2)
		- (constants::screensizeY() / 2);

	int temp = objects[0]->collision_area()[5];
	ui.cursor.set_pos(constants::get_x(temp), constants::get_y(temp));
	ui.p2_cursorpos = objects[1]->collision_area()[5];
}

Gameplay::~Gameplay() = default;

void Gameplay::update(
	std::array<bool, Controller::total_commands>& inputs, 
	std::array<bool, Controller::total_commands>& key_downs,
	std::array<bool, Controller::total_commands>& key_chars)
{
	auto check_tile_clear = [&](int check_x, int check_y)
	{
		if (ui.cursor.is_tile_clear(check_x, check_y)
			&& terrain[constants::get_index(check_x, check_y)] == 0)
		{
			return true;
		}
		return false;
	};
	if (actions_menu == nullptr && attack_screen == nullptr && victory_screen == nullptr)
	{
		if (key_chars[(int)command::cursor_up])
			ui.cursor.move(0, -1);
		if (key_chars[(int)command::cursor_right])
			ui.cursor.move(1, 0);
		if (key_chars[(int)command::cursor_down])
			ui.cursor.move(0, 1);
		if (key_chars[(int)command::cursor_left])
			ui.cursor.move(-1, 0);
		if (key_downs[(int)command::select])
			actions_menu = ui.create_actions_menu(0, 0);
	}
	else if (attack_screen != nullptr)
	{
		if (key_downs[(int)command::select])
			attack_screen = nullptr;
	}
	else if (victory_screen != nullptr)
	{
		// Do nothing. Exists so we skip the else branch.
	}
	else
	{
		// Moving, attacking, menu
		if (key_chars[(int)command::cursor_up])
		{
			if (actions_menu->unit_move)
			{
				int move_index = constants::get_index(ui.cursor.get_x(), ui.cursor.get_y() - 1);
				for (auto& move : actions_menu->moves)
				{
					if (move == move_index && check_tile_clear(ui.cursor.get_x(), ui.cursor.get_y() - 1))
					{
						ui.cursor.move(0, -1);
						actions_menu->object->move_relative(0, -1);  // the way we get a handle on this object is kind of disgusting
						// although since we have permanent access to the objects vector we could use the index, but the pointer's still valid,
						// since while it's present, new units can't be created (which would resize the vector).
						// Technically all actions menus have the field, but it's initialized to nullptr and really only used for movement and attacking
						// which abuses the action menu in a very kludgy way that I'm not exactly proud of.
						break;
					}
				}
			}
			else actions_menu->change_selected(-1);
		}	
		if (key_chars[(int)command::cursor_down])
		{
			if (actions_menu->unit_move)
			{
				int move_index = constants::get_index(ui.cursor.get_x(), ui.cursor.get_y() + 1);
				for (auto& move : actions_menu->moves)
				{
					if (move == move_index && check_tile_clear(ui.cursor.get_x(), ui.cursor.get_y() + 1))
					{
						ui.cursor.move(0, 1);
						actions_menu->object->move_relative(0, 1);
						break;
					}
				}
			}
			else actions_menu->change_selected(1);
		}
		// Move/attacking only
		if (key_chars[(int)command::cursor_left])
		{
			if (actions_menu->unit_move)
			{
				int move_index = constants::get_index(ui.cursor.get_x() - 1, ui.cursor.get_y());
				for (auto& move : actions_menu->moves)
				{
					if (move == move_index && check_tile_clear(ui.cursor.get_x() - 1, ui.cursor.get_y()))
					{
						ui.cursor.move(-1, 0);
						actions_menu->object->move_relative(-1, 0);
						break;
					}
				}
			}
		}
		if (key_chars[(int)command::cursor_right])
		{
			if (actions_menu->unit_move)
			{
				int move_index = constants::get_index(ui.cursor.get_x() + 1, ui.cursor.get_y());
				for (auto& move : actions_menu->moves)
				{
					if (move == move_index && check_tile_clear(ui.cursor.get_x() + 1, ui.cursor.get_y()))
					{
						ui.cursor.move(1, 0);
						actions_menu->object->move_relative(1, 0);
						break;
					}
				}
			}
		}

		// Selection
		if (key_downs[(int)command::unselect])
			actions_menu = nullptr;
		if (key_downs[(int)command::select])
		{
			Message message = actions_menu->get_selected_action();
			switch (message.action)
			{
			case Actions::create_worker:
				// In the future, this sort of thing should be handled as a player class/struct,
				// because we have a lot of if player is 1, then blah, else copy/paste switch variables.
				// But, that's a refactor that takes more time than what I have, because it's crunch time!
				if (ui.current_player == 1)
				{
					ui.player1money -= 250;
					units.emplace_back(std::make_unique<Worker>(message.tile_x, message.tile_y, 1, ogPointers.worker_player_1));
					ui.disable_building(objects);
				}
				else
				{
					ui.player2money -= 250;
					units.emplace_back(std::make_unique<Worker>(message.tile_x, message.tile_y, 2, ogPointers.worker_player_2));
					ui.disable_building(objects);
				}
				break;
			case Actions::create_puncher:
				if (ui.current_player == 1)
				{
					ui.player1money -= 1000;
					units.emplace_back(std::make_unique<Puncher>(message.tile_x, message.tile_y, 1, ogPointers.puncher_bot_player_1));
					ui.disable_building(objects);
				}
				else
				{
					ui.player2money -= 1000;
					units.emplace_back(std::make_unique<Puncher>(message.tile_x, message.tile_y, 2, ogPointers.puncher_bot_player_2));
					ui.disable_building(objects);
				}
				break;
			case Actions::end_turn:
				ui.end_turn(units, objects);
				break;
			case Actions::move:
				actions_menu->move_lock = true;
				actions_menu->object->stats.action = true;
				actions_menu = ui.create_actions_followup_menu(0, 0);
				goto skip_actions_menu_deletion; // disgusting, but we need to skip the actions_menu = nullptr,
												 // becuase otherwise the updated one we just made would get deleted.
				break;
			case Actions::build_mine:
			{
				int where = ui.cursor.get_object_at_tile(message.tile_x, message.tile_y);
				ResourceNode* node = dynamic_cast<ResourceNode*>(objects[where].get());
				node->create_mine(ui.current_player);
				break;
			}
			case Actions::capture_mine:
			{
				int where = ui.cursor.get_object_at_tile(message.tile_x, message.tile_y);
				ResourceNode* node = dynamic_cast<ResourceNode*>(objects[where].get());
				node->capture_mine(ui.current_player);
				break;
			}
			case Actions::attack:
			{
				bool was_unit = false;
				Object* target = nullptr;
				Object* attacker = nullptr;
				int where = ui.cursor.get_object_at_tile(message.tile_x, message.tile_y);
				int attacker_where = ui.cursor.get_unit_at_tile(ui.cursor.get_x(), ui.cursor.get_y());
				attacker = units[attacker_where].get();
				if (where != -1) target = objects[where].get();
				else
				{
					where = ui.cursor.get_unit_at_tile(message.tile_x, message.tile_y);
					target = units[where].get();
					was_unit = true;
				}
				// ATTACK!
				attack_screen = std::make_unique<AttackScreen>(al_map_rgb(255, 255, 255), target, attacker, rng);
				bool survived = attack_screen->attack();
				if (!survived)
				{
					if (was_unit) units[where] = nullptr;  // this deletes it, basically.
					else victory_screen = std::make_unique<VictoryScreen>(ui.current_player, al_map_rgb(255, 255, 255));
				}
				break;
			}
			default:
				std::cout << "Unrecognized action." << std::endl;
				break;
			}
			actions_menu = nullptr;
		skip_actions_menu_deletion:;
		}
	}

	// Keep scrolling in world bounds. Not exactly sure what'd happen if the world is small enough to fit onscreen,
	// but since there's no way to set it smaller than that I'm not going to worry about it until after such a
	// feature gets implemented.
	if (ui.scrollX < 0) ui.scrollX = 0;
	else if (ui.scrollX > constants::mapsizeX() * constants::tilesize() - constants::screensizeX())
		ui.scrollX = constants::mapsizeX() * constants::tilesize() - constants::screensizeX();
	if (ui.scrollY < 0) ui.scrollY = 0;
	else if (ui.scrollY > constants::mapsizeY() * constants::tilesize() - constants::screensizeY())
		ui.scrollY = constants::mapsizeY() * constants::tilesize() - constants::screensizeY();
}

void Gameplay::draw()
{
	// Note: because of the calling context for this function, it's okay to
	// mess with the active draw buffer. It will always be set at buffer1 at the start of the function.
	// Then, after this runs, it has to get changed anyway because then want to draw the buffers to the screen.

	// Figure out what's visible
	int startTileX = ui.scrollX / constants::tilesize();
	int startTileY = ui.scrollY / constants::tilesize();
	int endTileX = startTileX + constants::screentilesX() + 1;
	int endTileY = startTileY + constants::screentilesY() + 1;
	if (ui.scrollX % constants::tilesize() != 0) startTileX -= 1;
	if (ui.scrollY % constants::tilesize() != 0) startTileY -= 1;

	// Lambda for indexing
	auto index = [&](int x, int y)
	{
		return (x + (y * sizeX));
	};

	for (int y = startTileY; y < sizeY && y < endTileY; y++)
	{
		for (int x = startTileX; x < sizeX && x < endTileX; x++)
		{
			int draw_x = (x * constants::tilesize()) - ui.scrollX;
			int draw_y = (y * constants::tilesize()) - ui.scrollY;
			// Note: Terrain and Track are the same size,
			// so we can handle them both with one loop.
			if (x >= 0 && y >= 0)
			{
				if (terrain[index(x, y)] >= 1 && terrain[index(x, y)] <= 5 && water_frame == 1)
					terrain[index(x, y)] += 32;
				else if (terrain[index(x, y)] >= 33 && terrain[index(x, y)] <= 37 && water_frame == 0)
					terrain[index(x, y)] -= 32;
				terrain_tiles.drawTile(draw_x, draw_y, terrain[index(x, y)]);
				// track_tiles.drawTile(draw_x, draw_y, track[index(x, y)]);
			}
		}
	}
	if (animate_water == 40)
	{
		animate_water = -1;
		water_frame == 1 ? water_frame = 0 : water_frame = 1;
	}
	animate_water++;

	for (auto& object : objects)
		object->draw(ui.scrollX, ui.scrollY);
	for (auto& unit : units)
	{
		if (unit.get() != nullptr) unit->draw(ui.scrollX, ui.scrollY);
	}

	// note that the buffer gets changed internally by this function
	ui.draw(constants::buffer2);

	// Finally take care of buffer3
	al_set_target_bitmap(constants::buffer3);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));
	if (actions_menu != nullptr) actions_menu->draw(&ui, units, objects, terrain, constants::buffer1);
	if (attack_screen != nullptr) attack_screen->draw();
	if (victory_screen != nullptr) victory_screen->draw();
}

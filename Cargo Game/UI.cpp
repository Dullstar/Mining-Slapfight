#include "UI.h"
#include <iostream>
#include <allegro5/allegro_primitives.h>
#include <sstream>
#include <string>
#include "Controller.h"

void Cursor::draw(int object_index)
{
	char player_num;
	if (object_index == -1) player_num = 0;
	else player_num = objects[object_index]->get_player();

	if (player_num == 1)
		friendly->draw(tile_x * constants::tilesize(), tile_y * constants::tilesize(), ui->scrollX, ui->scrollY);
	else if (player_num == 2)
		enemy->draw(tile_x * constants::tilesize(), tile_y * constants::tilesize(), ui->scrollX, ui->scrollY);
	else
		neutral->draw(tile_x * constants::tilesize(), tile_y * constants::tilesize(), ui->scrollX, ui->scrollY);
}

// Why is this an int? While it probably would have been okay due to the expected lifetime,
// an Object* would have been risky due to the objects being stored in a std::vector.
// However, all our UI objects that need this information have access to that vector!
// Thus, what we're returning is the index of the object so it can be accessed from the vector.
// A result of -1 means that no object matched the result. Be careful with this!
int Cursor::get_selected_object()
{
	for (size_t i = 0; i < objects.size(); i++)
	{
		for (auto& index : objects[i]->collision_area())
		{
			if (constants::get_x(index) == tile_x && constants::get_y(index) == tile_y)
			{
				return i;
			}
		}
	}
	// If we got here, that means that there's no object on the square we're looking at.
	return -1;
}

int Cursor::get_selected_unit()
{
	for (size_t i = 0; i < units.size(); i++)
	{
		if (units[i] == nullptr) continue;
		for (auto& index : units[i]->collision_area())
		{
			if (constants::get_x(index) == tile_x && constants::get_y(index) == tile_y)
			{
				return i;
			}
		}
	}
	// If we got here, that means that there's no object on the square we're looking at.
	return -1;
}

Cursor::Cursor(UI* parent_ui, OG _friendly, OG _neutral, OG _enemy, const vecObject& _objects, const vecObject& _units, int tile_x, int tile_y)
	: ui(parent_ui), friendly(_friendly), neutral(_neutral), enemy(_enemy), objects(_objects), tile_x(tile_x), tile_y(tile_y), units(_units)
{
}

void Cursor::move(int dx, int dy)
{
	// We have to check the position a bunch to update things without things going out of bounds or offscreen,
	// so we'll save a few checks on average by being lazy and only bothering to do all the checks if the cursor is actually moving.
	// Becuase of how the controls are checked, only one of the two is likely to be != 0 on each call anyway.
	if (dx != 0)
	{
		tile_x += dx;
		if (tile_x < 0) tile_x = 0;
		else if (tile_x >= constants::mapsizeX()) tile_x = constants::mapsizeX() - 1;
		if (tile_x * constants::tilesize() - ui->scrollX < 0) ui->scrollX = tile_x * constants::tilesize();
		else if (tile_x * constants::tilesize() - ui->scrollX >= constants::tilesize() * constants::screentilesX())
			ui->scrollX = (tile_x + 1 - constants::screentilesX()) * constants::tilesize();
	}
	if (dy != 0)
	{
		tile_y += dy;
		if (tile_y < 0) tile_y = 0;
		else if (tile_y >= constants::mapsizeY()) tile_y = constants::mapsizeY() - 1;
		if (tile_y * constants::tilesize() - ui->scrollY < 0) ui->scrollY = tile_y * constants::tilesize();
		else if (tile_y * constants::tilesize() - ui->scrollY >= constants::tilesize() * constants::screentilesY())
			ui->scrollY = (tile_y + 1 - constants::screentilesY()) * constants::tilesize();
	}

}

void Cursor::set_pos(int x, int y, bool update_scrolling)
{
	// It would likely make sense to automatically move the cursor later,
	// with updated scrolling, but for starting out, we won't be doing any updates.
	// The update_scrolling parameter currently doesn't do anything except output
	// a warning if it's set to true, but we won't need to modify the signature to implement
	// it later.

	tile_x = x;
	tile_y = y;

	if (update_scrolling)
	{
		std::cout << "Warning: update scrolling in Cursor::set_pos is not implemented yet.\n"
			<< "Because whatever called this set it to true, unexpected behavior may occur once it's implemented.\n";
	}
}

int Cursor::get_x()
{
	return tile_x;
}

int Cursor::get_y()
{
	return tile_y;
}

int Cursor::get_index()
{
	return constants::get_index(tile_x, tile_y);
}

bool Cursor::is_tile_clear(int x, int y)
{
	int store_tile_x = tile_x;
	int store_tile_y = tile_y;
	tile_x = x;
	tile_y = y;
	bool output = true;
	if (get_selected_object() != -1 || get_selected_unit() != -1) output = false;
	tile_x = store_tile_x;
	tile_y = store_tile_y;
	return output;
}

int Cursor::get_object_at_tile(int x, int y)
{
	int store_tile_x = tile_x;
	int store_tile_y = tile_y;
	tile_x = x;
	tile_y = y;
	int output = get_selected_object();
	tile_x = store_tile_x;
	tile_y = store_tile_y;
	return output;
}

int Cursor::get_unit_at_tile(int x, int y)
{
	int store_tile_x = tile_x;
	int store_tile_y = tile_y;
	tile_x = x;
	tile_y = y;
	int output = get_selected_unit();
	tile_x = store_tile_x;
	tile_y = store_tile_y;
	return output;
}
// Yes, this emits warnings. But it shouldn't be an issue: those values get initialized eventually
// before they're used, just not at object creation (we don't know enough yet).
// ideally a default value should be chosen, but there's <24 hr remaining.
UI::UI(OGPC& ogpc, const vecObject& _objects, const vecObject& _units)
	: ogpc(ogpc),
	cursor(this, ogpc.cursor_friendly, ogpc.cursor_neutral, ogpc.cursor_enemy, _objects, _units, 0, 0),
	scrollX(0), scrollY(0), // these get initialized later, since we don't yet know enough to do it when this is created
	current_player(1),
	text(al_map_rgb(255, 255, 255)),
	dark_text(al_map_rgb(200, 200, 200)),
	player1name("Player 1"),
	player2name("Player 2"),
	player1money(500),
	player2money(500)
{
}

void UI::end_turn(vecObject& units, vecObject& objects)
{
	if (current_player == 1)
	{
		p1_scX = scrollX;
		p1_scY = scrollY;
		current_player = 2;
		scrollX = p2_scX;
		scrollY = p2_scY;
		p1_cursorpos = constants::get_index(cursor.tile_x, cursor.tile_y);
		cursor.set_pos(constants::get_x(p2_cursorpos), constants::get_y(p2_cursorpos));
	}
	else
	{
		p2_scX = scrollX;
		p2_scY = scrollY;
		current_player = 1;
		scrollX = p1_scX;
		scrollY = p1_scY;
		p2_cursorpos = constants::get_index(cursor.tile_x, cursor.tile_y);
		cursor.set_pos(constants::get_x(p1_cursorpos), constants::get_y(p1_cursorpos));
	}
	for (int i = (int)units.size() - 1; i >= 0; i--)
	{
		if (units[i] == nullptr)
		{
			units.erase(cursor.units.begin() + i);
			continue;
		}
		units[i]->stats.action = false; // free it up for another action on the next turn!
	}
	for (auto& object : objects)
	{
		object->stats.action = false;
		if (object->get_player() > 0 && object->get_type() == object_type::resource_node)
		{
			// Keep in mind we've already switched the player number
			ResourceNode* node = dynamic_cast<ResourceNode*>(object.get());
			if (current_player == 1 && node->get_player() == 2) player2money += node->get_value();
			else if (current_player == 2 && node->get_player() == 1) player1money += node->get_value();
		}
	}
}

void UI::draw(ALLEGRO_BITMAP* buffer2)
{
	// UI elements that need the scrolling information already know it, so we don't need to pass it in!
	int selected_unit_index = cursor.get_selected_unit();
	int selected_object_index = cursor.get_selected_object();
	cursor.draw(selected_object_index);
	al_set_target_bitmap(buffer2);
	al_clear_to_color(al_map_rgb(127, 127, 127));
	// Sidebar elements go here

	const int& tilesize = constants::tilesize(); // save ourselves some typing
	std::stringstream stat;
	if (current_player == 1) stat << player1name;
	else stat << player2name;
	stat << "'s Turn!";
	text.draw(stat.str().c_str(), tilesize, tilesize);
	text.draw("Press shift", tilesize, tilesize * (constants::screentilesY() - 2));
	text.draw("for actions...", tilesize, tilesize * (constants::screentilesY() - 1));
	
	stat.str("");
	dark_text.draw("Money:", tilesize, tilesize * (constants::screentilesY() - 4));
	if (current_player == 1) stat << player1money;
	else stat << player2money;
	text.draw(stat.str().c_str(), tilesize * 8, tilesize * (constants::screentilesY() - 4));

	// This awful mess of how units vs. other objects are handles is a result of the fact that I realized that
	// units needed to be prioritized over buildings and had to figure out a way to shoehorn it in.
	// Should definitely be a major refactoring target.
	if (selected_unit_index >= 0)
	{
		cursor.units[selected_unit_index]->ui_draw(tilesize, tilesize * 4);
		text.draw(cursor.units[selected_unit_index]->name.c_str(), tilesize, tilesize * 3);
		dark_text.draw("Owner:", tilesize * 4, tilesize * 4);
		if (cursor.units[selected_unit_index]->get_player() == 1)
			text.draw(player1name.c_str(), tilesize * 4, tilesize * 5);
		else if (cursor.units[selected_unit_index]->get_player() == 2)
			text.draw(player2name.c_str(), tilesize * 4, tilesize * 5);
		else text.draw("Neutral", tilesize * 4, tilesize * 5);  // this should never happen for a unit

		stat.str("");
		stat << cursor.units[selected_unit_index]->stats.hp;
		dark_text.draw("HP: ", tilesize, tilesize * 7);
		text.draw(stat.str().c_str(), tilesize * 5, tilesize * 7);
		stat.str("");
		stat << "1d" << cursor.units[selected_unit_index]->stats.hit;
		dark_text.draw("Hit: ", tilesize, tilesize * 8);
		text.draw(stat.str().c_str(), tilesize * 6, tilesize * 8);
		stat.str("");
		stat << "1d" << cursor.units[selected_unit_index]->stats.attack;
		dark_text.draw("Damage: ", tilesize, tilesize * 9);
		text.draw(stat.str().c_str(), tilesize * 9, tilesize * 9);
		stat.str("");
		stat << "1d" << cursor.units[selected_unit_index]->stats.evade;
		dark_text.draw("Evade: ", tilesize, tilesize * 10);
		text.draw(stat.str().c_str(), tilesize * 8, tilesize * 10);
		stat.str("");
		stat << cursor.units[selected_unit_index]->stats.move;
		dark_text.draw("Movement: ", tilesize, tilesize * 12);
		text.draw(stat.str().c_str(), tilesize * 11, tilesize * 12);
	}
	else if (selected_object_index >= 0)
	{
		// It probably doesn't really make sense to access the objects vector via the cursor class,
		// but the class already has a reference to the objects vector and we're allowed to access it from here.
		// Trying to give THIS class the reference as well failed for some reason - maybe due to moves happening
		// during initialization? If I'd moved that reference here, then cursor could use it, assuming it wouldn't stop working,
		// but this works even if it's questionable design, so we'll just accept this for now since it's a code jam project.
		// An unnecessary but nice refactor moving it to this class can wait until after the jam if I decide to continue with this.
		cursor.objects[selected_object_index]->ui_draw(tilesize, tilesize * 4);
		text.draw(cursor.objects[selected_object_index]->name.c_str(), tilesize, tilesize * 3);
		dark_text.draw("Owner:", tilesize * 4, tilesize * 4);
		if (cursor.objects[selected_object_index]->get_player() == 1)
			text.draw(player1name.c_str(), tilesize * 4, tilesize * 5);
		else if (cursor.objects[selected_object_index]->get_player() == 2)
			text.draw(player2name.c_str(), tilesize * 4, tilesize * 5);
		else text.draw("Neutral", tilesize * 4, tilesize * 5);

		object_type objectType = cursor.objects[selected_object_index]->get_type();
		// Now we need to make draw decisions based on what we've got selected.
		if (objectType == object_type::main_building)
		{
			if (current_player == cursor.objects[selected_object_index]->get_player())
				text.draw("Don't lose this!", tilesize, tilesize * 8);
			else text.draw("Destroy this!", tilesize, tilesize * 8);

			dark_text.draw("HP: ", tilesize, tilesize * 10);
			stat.str("");
			stat << cursor.objects[selected_object_index]->stats.hp;
			text.draw(stat.str().c_str(), tilesize * 5, tilesize * 10);
			dark_text.draw("Armor: ", tilesize, tilesize * 11);
			stat.str("");
			stat << "1d" << cursor.objects[selected_object_index]->stats.armor;
			text.draw(stat.str().c_str(), tilesize * 8, tilesize * 11);
		}
		if (objectType == object_type::worker
			|| objectType == object_type::puncher_bot
			|| objectType == object_type::ranger_bot)
		{
			dark_text.draw("HP: ", tilesize, tilesize * 10);
			std::stringstream HP;
			HP << cursor.objects[selected_object_index]->stats.hp;
			dark_text.draw(HP.str().c_str(), tilesize * 5, tilesize * 10);
		}
		if (objectType == object_type::resource_node)
		{
			ResourceNode* temp = dynamic_cast<ResourceNode*>(cursor.objects[selected_object_index].get());
			stat.str("");
			stat << temp->get_value();
			dark_text.draw("Value: ", tilesize, tilesize * 8);
			text.draw(stat.str().c_str(), tilesize * 8, tilesize * 8);
		}
	}
	
}

std::unique_ptr<ActionsMenu> UI::create_actions_menu(int x, int y)
{
	std::vector<Message> str;
	int current_money;
	if (current_player == 1) current_money = player1money;
	else current_money = player2money;
	bool unit_move = false;
	bool unit_attack = false;

	int object = cursor.get_selected_object();
	int unit = cursor.get_selected_unit();
	if (unit >= 0)
	{
		if (cursor.units[unit]->get_player() == current_player && cursor.units[unit]->stats.action == false)
		{
			str.emplace_back(Message(Actions::move, "Move, then press shift", 0, 0));
			unit_move = true;
		}
	}
	else if (object >= 0)
	{
		int msg_x = -1;
		int msg_y = -1;
		// Is this a main building?
		if (cursor.objects[object]->get_type() == object_type::main_building
			&& cursor.objects[object]->get_player() == current_player
			&& cursor.objects[object]->stats.action == false
			&& current_money >= 250)
		{
			for (auto& index : cursor.objects[object]->catchment_area())
			{
				if (cursor.is_tile_clear(constants::get_x(index), constants::get_y(index)) == true)
				{
					msg_x = constants::get_x(index);
					msg_y = constants::get_y(index);
					break;
				}
			}
			if (msg_x >= 0 && msg_y >= 0) // if we don't have this, it indicates no valid placement was found.
			{
				str.emplace_back(Message(Actions::create_worker, "Build Worker (250)", msg_x, msg_y));
				if (current_money >= 1000) str.emplace_back(Message(Actions::create_puncher, "Build Puncher (1000)", msg_x, msg_y));
			}
		}
	}


	// This one always goes in last.
	str.emplace_back(Message(Actions::end_turn, "End Turn", 0, 0));
	return std::make_unique<ActionsMenu>(x, y, str, al_map_rgb(255, 255, 255), unit_move, unit_attack);
}

std::unique_ptr<ActionsMenu> UI::create_actions_followup_menu(int x, int y)
{
	std::vector<Message> str;
	int unit = cursor.get_selected_unit();
	// We don't bother checking this time, because if there's no unit there something has gone horribly wrong
	// (and if it becomes a problem I can just keep that pointer alive a bit longer)...
	std::vector<int> unit_catchment_area = cursor.units[unit]->catchment_area();
	std::stringstream labels;

	auto add_direction = [&](command dir)
	{
		switch (dir)
		{
		case command::up:
			labels << "(N)";
			break;
		case command::down:
			labels << "(S)";
			break;
		case command::left:
			labels << "(W)";
			break;
		case command::right:
			labels << "(E)";
			break;
		}
	};

	if (cursor.units[unit]->get_type() == object_type::worker)
	{
		for (auto& tile : unit_catchment_area)
		{
			labels.str("");
			int object_index = cursor.get_object_at_tile(constants::get_x(tile), constants::get_y(tile));
			// Where is this?
			if (object_index >= 0)
			{
				command direction;
				if (constants::get_y(tile) == cursor.units[unit]->get_y())
				{
					// This would indicate that the object is either to the left or the right.
					if (constants::get_x(tile) + 1 == cursor.units[unit]->get_x()) direction = command::left;
					else direction = command::right;
				}
				else
				{
					// This would indicate it's either up or down.
					if (constants::get_y(tile) + 1 == cursor.units[unit]->get_y()) direction = command::up;
					else direction = command::down;
				}
				if (cursor.objects[object_index]->get_type() == object_type::resource_node
					&& cursor.objects[object_index]->get_player() == 0)
				{
					// 1st time trying something like this...
					ResourceNode* node = dynamic_cast<ResourceNode*>(cursor.objects[object_index].get());
					labels << "Build ";
					if (node->get_node_type() == Resource::wood)
					{
						labels << "Sawmill ";
					}
					else if (node->get_node_type() == Resource::iron)
					{
						labels << "Iron Mine ";
					}
					else if (node->get_node_type() == Resource::gold)
					{
						labels << "Gold Mine ";
					}
					add_direction(direction);
				}
				if (labels.str().size() > 0)
					str.emplace_back(Actions::build_mine, labels.str().c_str(), constants::get_x(tile), constants::get_y(tile));
			}
		}
	}
	else if (cursor.units[unit]->get_type() == object_type::puncher_bot)
	{
		for (auto& tile : unit_catchment_area)
		{
			labels.str("");
			int object_index = cursor.get_object_at_tile(constants::get_x(tile), constants::get_y(tile));
			// Where is this?
			if (object_index >= 0)
			{
				command direction;
				if (constants::get_y(tile) == cursor.units[unit]->get_y())
				{
					// This would indicate that the object is either to the left or the right.
					if (constants::get_x(tile) + 1 == cursor.units[unit]->get_x()) direction = command::left;
					else direction = command::right;
				}
				else
				{
					// This would indicate it's either up or down.
					if (constants::get_y(tile) + 1 == cursor.units[unit]->get_y()) direction = command::up;
					else direction = command::down;
				}
				if (cursor.objects[object_index]->get_type() == object_type::resource_node
					&& cursor.objects[object_index]->get_player() != 0
					&& cursor.objects[object_index]->get_player() != current_player)
				{
					// 1st time trying something like this...
					ResourceNode* node = dynamic_cast<ResourceNode*>(cursor.objects[object_index].get());
					labels << "Capture ";
					if (node->get_node_type() == Resource::wood)
					{
						labels << "Sawmill ";
					}
					else if (node->get_node_type() == Resource::iron)
					{
						labels << "Iron Mine ";
					}
					else if (node->get_node_type() == Resource::gold)
					{
						labels << "Gold Mine ";
					}
					add_direction(direction);
					if (labels.str().size() > 0)
						str.emplace_back(Actions::capture_mine, labels.str().c_str(), constants::get_x(tile), constants::get_y(tile));
				}
				else if (cursor.objects[object_index]->get_type() == object_type::main_building
					&& cursor.objects[object_index]->get_player() != current_player)
				{
					labels << "Attack Building ";
					add_direction(direction);
					str.emplace_back(Actions::attack, labels.str().c_str(), constants::get_x(tile), constants::get_y(tile));
				}
			}
			int unit_index = cursor.get_unit_at_tile(constants::get_x(tile), constants::get_y(tile));
			if (unit_index >= 0)
			{
				command direction;
				if (constants::get_y(tile) == cursor.units[unit]->get_y())
				{
					// This would indicate that the object is either to the left or the right.
					if (constants::get_x(tile) + 1 == cursor.units[unit]->get_x()) direction = command::left;
					else direction = command::right;
				}
				else
				{
					// This would indicate it's either up or down.
					if (constants::get_y(tile) + 1 == cursor.units[unit]->get_y()) direction = command::up;
					else direction = command::down;
				}
				if (cursor.units[unit_index]->get_player() != current_player)
				{
					labels << "Attack ";
					add_direction(direction);
					str.emplace_back(Actions::attack, labels.str().c_str(), constants::get_x(tile), constants::get_y(tile));
				}
			}
		}
	}
	if (str.size() == 0) return nullptr;
	str.emplace_back(Actions::none, "Do Nothing", 0, 0);  // we don't present this option if it's the only one available.
	return std::make_unique<ActionsMenu>(x, y, str, al_map_rgb(255, 255, 255));
}

void UI::disable_building(vecObject& objects)
{
	objects[cursor.get_selected_object()]->stats.action = true;
}

Text::Text(ALLEGRO_COLOR color)
	: color(color)
{
}

void Text::draw(const char* text, int x, int y)
{
	al_draw_text(constants::font, color, x, y, 0, text);
}

StaticText::StaticText(std::string text, int x, int y, ALLEGRO_COLOR color)
	: text(color), x(x), y(y), string(text)
{
}

void StaticText::draw()
{
	text.draw(string.c_str(), x, y);
}

ActionsMenu::ActionsMenu(int x, int y, std::vector<Message> _actions, ALLEGRO_COLOR color, bool unit_move, bool unit_attack)
	: actions(_actions), height(actions.size()* constants::tilesize()), width(0), text(color), selected(0), x(x), y(y), 
	unit_move(unit_move), unit_attack(unit_attack), unit_index(-1), move_lock(false), object(nullptr), x_mem(-1), y_mem(-1)
{
	// Start out by treating width in tile space...
	for (auto& str : actions)
	{
		if (str.action_str.size() > width) width = str.action_str.size();
	}
	// ...then adjust it here.
	if (!unit_move && !unit_attack) width += 2;  // gives room to show what's selected.
	else(height -= constants::tilesize()); // these options suppress end turn, which tbh should probably not have been added at all... refactor later
	width *= constants::tilesize();
}

ActionsMenu::~ActionsMenu()
{
	if (unit_move && !move_lock && unit_index >= 0 && x_mem >= 0 && y_mem >= 0)
	{
		object->move(x_mem, y_mem);
	}
}

void ActionsMenu::change_selected(int delta)
{
	selected += delta;
	if (selected >= (int)actions.size()) selected = 0;
	else if (selected < 0) selected = (int)actions.size() - 1;
}

void ActionsMenu::draw(UI* ui, vecObject& units, vecObject& objects, std::vector<char> terrain, ALLEGRO_BITMAP* buffer1)
{
	int text_draw_y = y;
	al_draw_filled_rectangle(x, y, x + width, y + height, al_map_rgb(127, 127, 127));
	if (unit_attack);
	else if (unit_move)
	{
		text.draw(actions[0].action_str.c_str(), x, text_draw_y);
		if (unit_index == -1)
		{
			unit_index = ui->cursor.get_selected_unit(); // cache this!
			object = units[unit_index].get(); // gross, but the destructor needs it so it can reset the position if we cancel movement
			x_mem = object->get_x();
			y_mem = object->get_y();
		}
		if (moves.size() == 0)
		{
			moves = units[unit_index]->get_moves();
		}
		ALLEGRO_BITMAP* current_draw_target = al_get_target_bitmap();
		al_set_target_bitmap(buffer1);
		for (auto& move : moves)
		{
			if (terrain[move] == 0 && 
				(constants::get_index(units[unit_index]->get_x(), units[unit_index]->get_y()) == move 
					|| ui->cursor.is_tile_clear(constants::get_x(move), constants::get_y(move))))
			{
				int draw_x = (constants::get_x(move) * constants::tilesize()) - ui->scrollX;
				int draw_y = (constants::get_y(move) * constants::tilesize()) - ui->scrollY;
				al_draw_filled_rectangle(draw_x, draw_y, draw_x + constants::tilesize(), draw_y + constants::tilesize(), al_map_rgba(0, 255, 127, 100));
			}
		}
		// Finally redraw the object on top...
		object->draw(ui->scrollX, ui->scrollY);
		al_set_target_bitmap(current_draw_target);
	}
	else
	{
		for (size_t i = 0; i < actions.size(); i++)
		{
			if (i == selected) text.draw("->", x, text_draw_y);
			text.draw(actions[i].action_str.c_str(), x + (constants::tilesize() * 2), text_draw_y);
			text_draw_y += constants::tilesize();
		}
	}
}

Message ActionsMenu::get_selected_action()
{
	return actions[selected];
}

Message::Message(Actions action, std::string action_str, int tileX, int tileY)
	: action(action), tile_x(tileX), tile_y(tileY), action_str(action_str)
{
}

AttackScreen::AttackScreen(ALLEGRO_COLOR color, Object* target, Object* attacker, RNG& rng)
	: text(color), target(target), attacker(attacker), rng(rng), width(0), height(0)
{
	text_buffer = al_create_bitmap(constants::screensizeX(), constants::screensizeY());
}

AttackScreen::~AttackScreen()
{
	al_destroy_bitmap(text_buffer);
}

// Returns true if the target survives, false if it does not.
bool AttackScreen::attack()
{
	int draw_x = 0;
	int draw_y = 0;
	ALLEGRO_BITMAP* current_buffer = al_get_target_bitmap();
	al_set_target_bitmap(text_buffer);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));
	std::stringstream stream;
	int hit = rng.random_number(attacker->stats.hit);
	int evade = rng.random_number(target->stats.evade);
	stream << "Attacker Hit: " << hit;
	text.draw(stream.str().c_str(), draw_x, draw_y);
	draw_y += constants::tilesize();
	stream.str("");
	stream << "vs. Target Evade: " << evade;
	text.draw(stream.str().c_str(), draw_x, draw_y);
	draw_y += constants::tilesize();
	stream.str("");
	if (hit >= evade) text.draw("It hits!", draw_x, draw_y);  // This cannot be wider, so we don't need to check.
	else if (hit == evade)
	{
		stream.str("");
		stream << "It hits, but barely. 1/2 damage.";
		text.draw(stream.str().c_str(), draw_x, draw_y);
	}
	else
	{
		text.draw("It misses...", draw_x, draw_y);
		draw_y += constants::tilesize() * 3;
		text.draw("Press shift to continue...", draw_x, draw_y);
		al_set_target_bitmap(current_buffer);
		return true;
	}
	draw_y += constants::tilesize();
	int damage = rng.random_number(attacker->stats.attack);
	if (hit == evade) damage /= 2;
	int armor = rng.random_number(target->stats.armor);
	stream << "Attacker Damage: " << damage;
	text.draw(stream.str().c_str(), draw_x, draw_y);
	draw_y += constants::tilesize();
	stream.str("");
	stream << "vs. Target Armor: " << armor;
	text.draw(stream.str().c_str(), draw_x, draw_y);
	draw_y += constants::tilesize();
	stream.str("");
	int total_dmg = damage - armor;
	if (total_dmg < 0) total_dmg = 0;
	stream << "Total damage: " << total_dmg;
	text.draw(stream.str().c_str(), draw_x, draw_y);
	stream.str("");
	draw_y += constants::tilesize();
	target->stats.hp -= total_dmg;
	if (target->stats.hp > 0)
	{
		stream << "Target survives with " << target->stats.hp << " HP";
		text.draw(stream.str().c_str(), draw_x, draw_y);
		draw_y += constants::tilesize() * 3;
		text.draw("Press shift to continue...", draw_x, draw_y);
		al_set_target_bitmap(current_buffer);
		return true;
	}
	else
	{
		text.draw("The target was destroyed.", draw_x, draw_y);
		draw_y += constants::tilesize() * 3;
		text.draw("Press shift to continue...", draw_x, draw_y);
		al_set_target_bitmap(current_buffer);
		return false;
	}
}

void AttackScreen::draw()
{
	al_draw_filled_rectangle(0, 0, constants::screensizeX(), 10 * constants::tilesize(), al_map_rgb(127, 127, 127));
	al_draw_bitmap(text_buffer, 0, 0, 0);
}

VictoryScreen::VictoryScreen(char winner, ALLEGRO_COLOR color)
	: winner(winner), text(color) {}

void VictoryScreen::draw()
{
	std::stringstream stream;
	stream << "Player " << (int)winner << " is victorious!";
	al_clear_to_color(al_map_rgb(127, 127, 127));
	text.draw(stream.str().c_str(), constants::tilesize() * 3, constants::tilesize() * 3);
}

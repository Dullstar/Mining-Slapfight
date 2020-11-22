#pragma once

#include <memory>
#include <string>
#include <allegro5/allegro.h>
#include "constants.h"
#include "Objects.h"
#include "Units.h"
#include "RNG.h"
// File contains UI elements and a container for them.
// As with objects, this is so we don't have a bunch of files that are only a few lines each.

struct UI;
class ObjectGraphic;
typedef std::shared_ptr<ObjectGraphic> OG;
class Object;
class ActionsMenu;

class Cursor
{
private:
	UI* ui;
	OG friendly;
	OG neutral;
	OG enemy;
	int tile_x;
	int tile_y;
	const std::vector<std::unique_ptr<Object>>& objects;
	const vecObject& units;
	void draw(int object_index);
	int get_selected_object();
	int get_selected_unit();
public:
	Cursor(UI* parent_ui, OG _friendly, OG _neutral, OG _enemy, const vecObject& _objects, const vecObject& _units, int tile_x = 0, int tile_y = 0);
	void move(int dx, int dy);
	void set_pos(int x, int y, bool update_scrolling = false);
	int get_x();
	int get_y();
	int get_index();
	bool is_tile_clear(int x, int y);
	int get_object_at_tile(int x, int y);
	int get_unit_at_tile(int x, int y);
	friend struct UI;
	friend class ActionsMenu;
};

struct Text
{
public:
	ALLEGRO_COLOR color;
	Text(ALLEGRO_COLOR color);
	void draw(const char* text, int x, int y);
};

// This one's mostly for text that doesn't change much.
// It'll probably only end up getting used by the main menu when it's created.
class StaticText
{
private:
	Text text;
public:
	std::string string;
	int x, y;
	StaticText(std::string text, int x, int y, ALLEGRO_COLOR color);
	void draw();
};

enum class Actions
{
	exit_menu,
	end_turn,
	create_worker,
	create_puncher,
	build_mine,
	capture_mine,
	move,
	attack,
	none
};

struct Message
{
public:
	int tile_x, tile_y;
	Actions action;
	std::string action_str;
	Message(Actions action, std::string action_str, int tileX, int tileY);
};
class ActionsMenu
{
private:
	std::vector<Message> actions;
	int width;
	int height;
	Text text;
	int x, y;
	int selected;
	int unit_index;
	int x_mem, y_mem;
public:
	Object* object;
	std::vector<int> moves;
	bool unit_move, unit_attack, move_lock;
	ActionsMenu(int x, int y, std::vector<Message> _actions, ALLEGRO_COLOR color, bool unit_move = false, bool unit_attack = false);
	~ActionsMenu();
	void change_selected(int delta);
	void draw(UI* ui, vecObject& units, vecObject& objects, std::vector<char> terrain, ALLEGRO_BITMAP* buffer1);
	Message get_selected_action();
};


// Container for UI stuff. And also game variables, because they needed to go somewhere,
// and this needed to access them, so they just kinda ended up here.
struct UI
{
private:
	OGPC& ogpc;
	std::string player1name;
	std::string player2name;
public:
	// These are for turn switching bookkeeping. Don't use them in gameplay,
	// because they are only updated when the turns change.
	int p1_scX, p1_scY;
	int p2_scX, p2_scY;
	int p1_cursorpos;
	int p2_cursorpos;
	// End stuff for turn switching bookkeeping.
	char current_player;
	UI(OGPC& ogpc, const vecObject& _objects, const vecObject& _units);
	void end_turn(vecObject& units, vecObject& objects);
	int scrollX;
	int scrollY;
	Cursor cursor;
	Text text;
	Text dark_text;
	int player1money;
	int player2money;
	void draw(ALLEGRO_BITMAP* buffer2);
	std::unique_ptr<ActionsMenu> create_actions_menu(int x, int y);
	std::unique_ptr<ActionsMenu> create_actions_followup_menu(int x, int y);
	void disable_building(vecObject& objects);
};

class AttackScreen
{
private:
	int width, height;
	Object* target;
	Object* attacker;
	ALLEGRO_BITMAP* text_buffer;
	RNG& rng;
public:
	Text text;
	AttackScreen(ALLEGRO_COLOR color, Object* target, Object* attacker, RNG& rng);
	~AttackScreen();
	bool attack();
	void draw();
};

class VictoryScreen
{
private:
	char winner;
	ALLEGRO_COLOR color;
public:
	Text text;
	VictoryScreen(char winner, ALLEGRO_COLOR color);
	void draw();
};
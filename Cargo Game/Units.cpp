#include "Units.h"
#include "constants.h"
#include <iostream>

Worker::Worker(int x, int y, char player, OG graphic)
	: Unit(x, y, player, graphic, "Worker")
{
	objectType = object_type::worker;
	stats.hp = 10;
	stats.move = 10;
	stats.evade = 10;
}

Unit::Unit(int x, int y, char player, OG graphic, std::string name)
	: Object(x, y, player, graphic, name)
{
}

std::vector<int> Unit::collision_area()
{
	return Object::collision_area_1x1(x, y);
}

std::vector<int> Unit::catchment_area()
{
	return Object::catchment_area_1x1(x, y);
}

// Note that this doesn't run any collision detection
// or check the terrain or anything like that. It really should,
// and that will happen during the refactor if I continue with the project,
// because a consequence of this is that it's very likely that while you can't
// stop on an object or water, you can pass over it - at least in the display.
// The implementation for actually inputting movement commands would probably stop it.
std::vector<int> Unit::get_moves()
{
	std::vector<int> moves;
	if (stats.action == true) return moves;
	std::vector<int> visited;
	int total_moves = stats.move;
	visited.emplace_back(constants::get_index(x, y));
	moves.emplace_back(constants::get_index(x, y));

	auto clamp = [&](int index)
	{
		// This is definitely not correct, but it does its most important function of preventing a crash
		// but really we need to treat x and y separately...
		if (index < 0) return 0;
		if (index >= constants::mapsizeX() * constants::mapsizeY())
			return (constants::mapsizeX() * constants::mapsizeY()) - 1;
        return index;
	};

	// This seems really inefficient, but I can't think of a better way right now
	auto was_visited = [&](int index)
	{
		for (auto& item : visited)
		{
			if (index == item) return true;
		}
		return false;
	};

	auto emplace_helper = [&](int index)
	{
		if (!was_visited(index))
		{
			visited.emplace_back(index);
			moves.emplace_back(index);
		}
	};
	while (total_moves > 0)
	{
		size_t size = moves.size();  // I haven't actually checked what happens if the vector grows during a for loop
		// using its size as the max, but let's go ahead and get the current value now just to be safe.
		for (size_t i = 0; i < size; i++)
		{
			int neighbor_up = constants::get_index(constants::get_x(moves[i]), constants::get_y(moves[i]) - 1);
			neighbor_up = clamp(neighbor_up);
			int neighbor_right = constants::get_index(constants::get_x(moves[i]) + 1, constants::get_y(moves[i]));
			neighbor_right = clamp(neighbor_right);
			int neighbor_down = constants::get_index(constants::get_x(moves[i]), constants::get_y(moves[i]) + 1);
			neighbor_down = clamp(neighbor_down);
			int neighbor_left = constants::get_index(constants::get_x(moves[i]) - 1, constants::get_y(moves[i]));
			neighbor_left = clamp(neighbor_left);

			emplace_helper(neighbor_up);
			emplace_helper(neighbor_right);
			emplace_helper(neighbor_down);
			emplace_helper(neighbor_left);
		}
		total_moves--;
	}
	return moves;
}

Puncher::Puncher(int x, int y, char player, OG graphic)
	: Unit(x, y, player, graphic, "Puncher")
{
	objectType = object_type::puncher_bot;
	stats.hp = 20;
	stats.move = 8;
	stats.evade = 4;
	stats.hit = 20;
	stats.attack = 20;
}
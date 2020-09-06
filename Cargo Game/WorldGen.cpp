#include "WorldGen.h"
#include <iostream>
#include <array>
#include "Controller.h"
#include <limits>
#include "constants.h"

std::vector<char> WorldGen::create(int x, int y)
{
	// Lambda for indexing
	auto index = [&](int _x, int _y)
	{
		return (_x + (_y * y));
	};

	auto get_x = [&](int _index)
	{
		return _index % x;
	};

	auto get_y = [&](int _index)
	{
		return _index / y;
	};

	auto get_smallest = [&](int a, int b)
	{
		if (a < b) return a;
		else return b;
	};

	auto distance_from_edge = [&](int _x, int _y)
	{
		// It may be a bit confusing, but _x and _y are the
		// thing we're looking at, while x and y are the total size.
		// I probably could have designed that better.

		// initialize!
		int dist = get_smallest(x, y);
		// Check the x axis
		dist = get_smallest(dist, _x);
		dist = get_smallest(dist, x - _x);
		dist = get_smallest(dist, _y);
		dist = get_smallest(dist, y - _y);
		return dist;
	};


	

	RNG rng = RNG();
	int start_x = x / 2;
	int start_y = y / 2;
	int larger_dist;
	if (start_x > start_y) larger_dist = start_x;
	else larger_dist = start_y;

	std::vector<int> world;
	std::vector<bool> visited;  // for later use, but initializing here to save a for loop
	world.reserve(x * y);
	visited.reserve(x * y);
	for (int i = 0; i < x * y; i++)
	{
		world.emplace_back(0);
		visited.emplace_back(false);
	}

	auto num_to_beat = [&](int dist)
	{
		float slope = 19.0f / (1 - (float)larger_dist);
		float result = (slope * ((float)dist - (float)larger_dist)) + 1;
		return (int)result;
	};

	std::queue<int> queue;
	float map_coverage = 0;
	// Goal: simple world generation that's good enough. Since we don't need very many complex features
	// (the terrain is just either land or water after all), and we want an island, I've implemented something
	// much simpler than, say, Perlin noise.

	// Solution: we place a piece of terrain in the center, then use it as a seed to create more terrain.
	world[index(start_x, start_y)] = INT_MAX - 1; // the -1 makes sure we have room to initialize a temp variable one larger
	queue.push(index(start_x, start_y));

	std::vector<std::pair<int, int>> relative_neighbor_index;
	relative_neighbor_index.emplace_back(std::make_pair(0, 1));
	relative_neighbor_index.emplace_back(std::make_pair(1, 0));
	relative_neighbor_index.emplace_back(std::make_pair(0, -1));
	relative_neighbor_index.emplace_back(std::make_pair(-1, 0));
	while (map_coverage < (float)x * (float)y * 0.9f)
	{
		if (queue.empty()) break;
		int current_index = queue.front();
		queue.pop();

		int current_x = get_x(current_index);
		int current_y = get_y(current_index);

		int current = world[current_index];


		for (auto& pair : relative_neighbor_index)
		{
			int next_x = current_x + pair.first;
			int next_y = current_y + pair.second;
			if (map_coverage < (float)x * (float)y * 0.05f || rng.random_number(20) > num_to_beat(distance_from_edge(current_x, current_y)))
			{
				int next_index = index(next_x, next_y);
				// Bounds checking!
				if (next_x >= x || next_x < 0 || next_y >= y || next_y < 0) continue;
				if (world[next_index] == 0 || world[next_index] < current - 1)
				{
					queue.push(next_index);
					map_coverage++;
					world[next_index] = current - 1;
				}
			}
		}
	}

	// Some other stuff goes here

	// Now we can turn this into the correct form.
	std::vector<char> output;
	output.reserve(world.size());
	for (size_t i = 0; i < world.size(); i++)
	{
		// Note to future self: this is probably about where we'll clean stuff up. Rivers can be -1 to differentiate from regular water.
		if (world[i] > 0) output.emplace_back(0);
		else 
		{	
			// Get the surrounding land blocks
			std::array<bool, 4> surrounding_blocks{ false };
			// conveniently the command enum will work here for representing directions
			// even if it wasn't designed that way intentionally
			int number_of_neighbors = 0;
			// Check Up
			if (get_y(i) > 0 && world[index(get_x(i), get_y(i) - 1)] > 0)
			{
				surrounding_blocks[(int)command::up] = true;
				number_of_neighbors++;
			}
			// Check Right
			if (get_x(i) < x - 1 && world[index(get_x(i) + 1, get_y(i))] > 0)
			{
				surrounding_blocks[(int)command::right] = true;
				number_of_neighbors++;
			}
			// Check Down
			if (get_y(i) < y - 1 && world[index(get_x(i), get_y(i) + 1)] > 0)
			{
				surrounding_blocks[(int)command::down] = true;
				number_of_neighbors++;
			}
			// Check Left
			if (get_x(i) > 0 && world[index(get_x(i) - 1, get_y(i))] > 0)
			{
				surrounding_blocks[(int)command::left] = true;
				number_of_neighbors++;
			}
			// Now we do something with this information!
			if (number_of_neighbors == 2)
			{
				// Cases we DEFINITELY want to handle
				if (surrounding_blocks[(int)command::up] && surrounding_blocks[(int)command::left])
					output.emplace_back(2);
				else if (surrounding_blocks[(int)command::up] && surrounding_blocks[(int)command::right])
					output.emplace_back(3);
				else if (surrounding_blocks[(int)command::down] && surrounding_blocks[(int)command::left])
					output.emplace_back(4);
				else if (surrounding_blocks[(int)command::down] && surrounding_blocks[(int)command::right])
					output.emplace_back(5);
				// could arguably put water or land
				else
					output.emplace_back(1);
			}
			else if (number_of_neighbors == 4)
				output.emplace_back(0);
			else
				output.emplace_back(1);
		}
	}

	// Rivers - commented out since it wasn't working right and I probably shouldn't sink any more
	// time into it, since it is not a part of the minimum viable product.
	/*
	int rivers = 8; // target number of rivers
	while (rivers > 0)
	{
		auto rand_sign = [&]()
		{
			int temp = rng.random_number(1);
			if (temp == 1) return 1;
			else return -1;
		};
		//int rand_x = rng.random_number(x - 1);
		//int rand_y = rng.random_number(y - 1);
		int rand_x = start_x + (rand_sign() * (rng.random_number(x / 10)));
		int rand_y = start_y + (rand_sign() * (rng.random_number(y / 10)));

		if (world[index(rand_x, rand_y)] > 0)
		{
			output[index(rand_x, rand_y)] = 1;
			visited[index(rand_x, rand_y)] = true;
			while (world[index(rand_x, rand_y)] > 0)
			{
				// Figure out what direction leads to the water (using the downward slopes
				// of values we made earlier)
				int smallest = INT_MAX;
				int new_smallest = INT_MAX;
				command smallest_dir = command::none; // since command already has up/down/left/right
				// we initialize to command::none to get rid of warnings, but it's not really needed...
				if (rand_y > 0 && !visited[index(rand_x, rand_y - 1)])
				{
					new_smallest = get_smallest(smallest, world[index(rand_x, rand_y - 1)]);
					if (smallest != new_smallest)
						smallest_dir = command::up;
					smallest = new_smallest;
				}
				if (rand_x < x - 1 && !visited[index(rand_x + 1, rand_y)])
				{
					new_smallest = get_smallest(smallest, world[index(rand_x + 1, rand_y)]);
					if (smallest != new_smallest)
						smallest_dir = command::right;
					smallest = new_smallest;
				}
				if (rand_y < y - 1 && !visited[index(rand_x, rand_y + 1)])
				{
					new_smallest = get_smallest(smallest, world[index(rand_x, rand_y + 1)]);
					if (smallest != new_smallest)
						smallest_dir = command::down;
					smallest = new_smallest;
				}
				if (rand_x > 0 && !visited[index(rand_x, rand_y - 1)])
				{
					new_smallest = get_smallest(smallest, world[index(rand_x - 1, rand_y)]);
					if (smallest != new_smallest)
						smallest_dir = command::left;
					smallest = new_smallest;
				}
				if (smallest == INT_MAX)
				{
					// This should never happen, but if it does, we've got to prevent an infinite loop
					break;
				}

				// Now move to the appropriate direction and update that tile.
				switch (smallest_dir)
				{
				case command::up:
					rand_y--;
					break;
				case command::right:
					rand_x++;
					break;
				case command::down:
					rand_y++;
					break;
				case command::left:
					rand_x--;
					break;
				}
				output[index(rand_x, rand_y)] = 0;
				visited[index(rand_x, rand_y)] = true;
			}
			rivers--;
		}
	}*/
	return output;
}

std::vector<std::unique_ptr<Object>> WorldGen::create_objects(std::vector<char>& terrain, OGPC& ogpc)
{
	std::vector<std::unique_ptr<Object>> objects;

	auto const mapX = constants::mapsizeX();
	auto const mapY = constants::mapsizeY();

	std::vector<bool> occupied(terrain.size(), false);

	auto place_main_building = [&](int x, int y, char player, int dx, int dy)
	{
		for (int i = 0; i < mapX && i < mapY; i++)
		{
			// We will DEFINITELY escape the bounds here, but that's okay, because we'll check to see.
			bool works = true;
			x += dx;
			y += dy;
			std::vector<int> terrain_to_check = Object::collision_area_3x3(x, y);
			{
				std::vector<int> more_terrain_to_check = Object::catchment_area_3x3(x, y);
				for (auto& tile : more_terrain_to_check)
				{
					terrain_to_check.emplace_back(tile);
				}
			}
			for (auto& index : terrain_to_check)
			{
				// I've padded the border a little to ensure that that the catchment function
				// won't cause us to access out of bounds vector indices if I forget to account for it later.
				// EDIT LATER: this shouldn't be a problem anymore, since the catchment is now checked during generation,
				// but I'm choosing to leave the padding anyway; a little extra safety around the boundaries won't hurt,
				// and it's highly unlikely this will end up excluding any valid spawn locations due to the way the
				// terrain generation works.
				if (index < 1 || index > (int)terrain.size() - 2 || terrain[index] != 0 || occupied[index] == true)
				{
					works = false;
					break;
				}
			}
			if (works == true)
			{
				if (player == 1)
					objects.emplace_back(std::make_unique<MainBuilding>(x, y, 1, ogpc.main_building_player_1));
				else
					objects.emplace_back(std::make_unique<MainBuilding>(x, y, 2, ogpc.main_building_player_2));
				for (auto& index : terrain_to_check)
				{
					occupied[index] = true;
				}
				return true;
			}
		}
		return false;
	};

	bool placedp1 = place_main_building(0, mapY, 1, 1, -1);
	if (placedp1 == false) throw ("Object generation failure.");
	bool placedp2 = place_main_building(mapX, 0, 2, -1, 1);
	if (placedp2 == false) throw ("Object generation failure.");

	// Now we need to generate the resource nodes.
	RNG rng;
	for (size_t i = 0; i < terrain.size(); i++)
	{
		if (terrain[i] == 0 && rng.random_number(100) < 4)
		{
			int x = constants::get_x(i);
			int y = constants::get_y(i);
			bool works = true;
			for (auto& index : Object::catchment_area_1x1(x, y))
			{
				if (terrain[index] != 0 || occupied[index] == true) { works = false; break; }
			}
			if (works == false) continue;
			int type = rng.random_number(10);
			switch (type)
			{
			case 1:
			case 2:
			case 3:
			case 7:
			case 8:
			case 9:
			case 10:
				// wood
				objects.emplace_back(std::make_unique<ResourceNode>(x, y, Resource::wood, 75, ogpc.resource_wood, ogpc.mine_wood_player_1, ogpc.mine_wood_player_2));
				break;
			case 4:
			case 5:
				// iron
				objects.emplace_back(std::make_unique<ResourceNode>(x, y, Resource::iron, 250, ogpc.resource_iron, ogpc.mine_ore_player_1, ogpc.mine_ore_player_2));
				break;
			case 6:
				// gold
				objects.emplace_back(std::make_unique<ResourceNode>(x, y, Resource::gold, 750, ogpc.resource_gold, ogpc.mine_ore_player_1, ogpc.mine_ore_player_2));
				break;
			}
			occupied[i] = true;
		}
	}
	return objects;
}

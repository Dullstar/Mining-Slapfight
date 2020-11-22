#pragma once

#include <string>

#include "Objects.h"

class Player
{
	// Originally, this class didn't exist, which was definitely a mistake.
	// As a consequence, the calling code for this handles some stuff that probably
	// should be handled by this class. Should eventually refactor, but this is already
	// an improvement.
public:
	int scrollX;
	int scrollY;
	int money;
	std::string name;
	Player(char player_num, const OGPC& ogpc, int scrollX = 0, int scrollY = 0);
	char get_player_num() const;
	OG get_unit_graphic(object_type _object_type);
private:
	char player_num_m;
	const OGPC& ogPointers;
};


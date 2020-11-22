#include "Player.h"

Player::Player(char player_num, const OGPC& ogpc, int _scrollX, int _scrollY)
	: scrollX(_scrollX), scrollY(_scrollY), player_num_m(player_num), ogPointers(ogpc)
{
	assert(player_num_m == 1 || player_num_m == 2);
}

char Player::get_player_num() const
{
	return player_num_m;
}

// This design still isn't really ideal, but it's an improvement over what it replaced,
// since at least all these are in the one place now.
OG Player::get_unit_graphic(object_type _object_type)
{
	auto select = [&](auto p1, auto p2)
	{
		if (player_num_m == 1) return p1;
		else return p2;
	};
	switch (_object_type) {
	case object_type::factory:
		return select(ogPointers.factory_player_1, ogPointers.factory_player_2);
	case object_type::main_building:
		return select(ogPointers.main_building_player_1, ogPointers.main_building_player_2);
	case object_type::puncher_bot:
		return select(ogPointers.puncher_bot_player_1, ogPointers.puncher_bot_player_2);
	case object_type::ranger_bot:
		return select(ogPointers.ranger_bot_player_1, ogPointers.ranger_bot_player_2);
	case object_type::worker:
		return select(ogPointers.worker_player_1, ogPointers.worker_player_2);
	default:
		// this should never happen, but this at least means it won't crash
		return ogPointers.placeholder;
	}
}

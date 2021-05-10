#include "Tileset.h"
#include "MainGame.h"
#include "constants.h"
#include <iostream>

Tileset::Tileset(const char* filename)
{
	graphic = al_load_bitmap(filename);
	if (graphic == NULL) std::cerr << "Error: couldn't load file " << filename << std::endl;
}

Tileset::~Tileset()
{
	al_destroy_bitmap(graphic);
}

void Tileset::drawTile(int x, int y, char tile_index)
{
	if (tile_index >= 0)
	{
		auto get_x = [&](int index)
		{
			return ((index % 8) * constants::tilesize());
		};

		auto get_y = [&](int index)
		{
			return ((index / 8) * constants::tilesize());
		};
		//std::cout << "Tried to draw tile at (" << x << "," << y << ")\n";
		//std::cout << "which is tile index " << tile_index << "\n";
		//std::cout << "located at (" << get_x(tile_index) << "," << get_y(tile_index) << ") in the tileset.\n";
		//constants::maingame->DrawPartialSprite(x, y, graphic.get(), get_x(tile_index), get_y(tile_index), constants::tilesize(), constants::tilesize());
		//constants::maingame->DrawSprite(0, 0, graphic.get());
		al_draw_bitmap_region(
			graphic,
			get_x(tile_index), get_y(tile_index),
			constants::tilesize(), constants::tilesize(),
			x, y, 0
			);
	}
}

void Tileset::testDraw()
{
	al_draw_bitmap(graphic, 0, 0, 0);
}
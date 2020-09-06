#pragma once
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <memory>
#include <vector>

class Tileset
{
private:
	// Allegro Bitmaps need to be destroyed with the al_destroy_bitmap function,
	// so this can't be a smart pointer.
	ALLEGRO_BITMAP* graphic;
public:
	Tileset(const char* filename);
	~Tileset();
	void drawTile(int x, int y, char tile_index);
	void testDraw();
};


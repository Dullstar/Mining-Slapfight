#pragma once
#include <allegro5/allegro_font.h>
//#include "RNG.h"
class Config;
class MainGame;

// Container class for stuff that should remain constant during play.
// However, it is not completely immutable: the friend class Config
// exists to provide a way for these values to be changed.
class constants
{
private:
	static int tilesize_m;
	static int screensizeX_m;
	static int screensizeY_m;
	static int screentilesX_m;
	static int screentilesY_m;
	static int pixelsize_m;
	static int mapsize_x_m;
	static int mapsize_y_m;
	static int ui_size_x_m;
	constants();
public:
	static MainGame* maingame;
	static int tilesize();
	static int screensizeX();
	static int screensizeY();
	static int screentilesX();
	static int screentilesY();
	static int pixelsize();
	static int mapsizeX();
	static int mapsizeY();
	static int get_x(int index);
	static int get_y(int index);
	static int get_index(int x, int y);
	static int ui_size_x();
	static ALLEGRO_FONT* font;
	//static RNG rng;
	friend class Config;
};

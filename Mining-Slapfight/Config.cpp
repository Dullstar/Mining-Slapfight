#include "Config.h"
#include "constants.h"

Config::Config(int tilesize_, int pixelsize_, int screensizeX_, int screensizeY_, int uisizeX_)
	: tilesize(tilesize_), pixelsize(pixelsize_), screensizeX(screensizeX_), screensizeY(screensizeY_), uisizeX(uisizeX_)
{
}

void Config::apply()
{
	constants::tilesize_m = tilesize;
	constants::screensizeX_m = screensizeX;
	constants::screensizeY_m = screensizeY;
	constants::screentilesX_m = screensizeX / tilesize;
	constants::screentilesY_m = screensizeY / tilesize;
	constants::pixelsize_m = pixelsize;
	// screentiles was created for use by a method in Gameplay that determines
	// what tiles are visible; thus, if screensize isn't evenly divisible by the tilesize,
	// we want to count the partial tile.
	if (screensizeX % tilesize != 0) constants::screentilesX_m += 1;
	if (screensizeY % tilesize != 0) constants::screentilesY_m += 1;

	// Realistically this should be configurable, but for now it's hardcoded.
	constants::mapsize_x_m = 100;
	constants::mapsize_y_m = 100;

	constants::ui_size_x_m = uisizeX;
}

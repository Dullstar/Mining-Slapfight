#include "constants.h"

int constants::tilesize()
{
    return tilesize_m;
}

int constants::screensizeX()
{
    return screensizeX_m;
}

int constants::screensizeY()
{
    return screensizeY_m;
}

int constants::screentilesX()
{
    return screentilesX_m;
}

int constants::screentilesY()
{
    return screentilesY_m;
}

int constants::pixelsize()
{
    return pixelsize_m;
}

int constants::mapsizeX()
{
    return mapsize_x_m;
}

int constants::mapsizeY()
{
    return mapsize_y_m;
}

int constants::get_x(int index)
{
    return (index % mapsize_x_m);
}

int constants::get_y(int index)
{
    return (index / mapsize_x_m);
}

int constants::get_index(int x, int y)
{
    return (x + (y * mapsize_x_m));
}

int constants::ui_size_x()
{
    return ui_size_x_m;
}

int constants::tilesize_m;
int constants::screensizeX_m;
int constants::screensizeY_m;
int constants::screentilesX_m;
int constants::screentilesY_m;
int constants::pixelsize_m;
int constants::mapsize_x_m;
int constants::mapsize_y_m;
int constants::ui_size_x_m;
ALLEGRO_FONT* constants::font;
MainGame* constants::maingame = nullptr;
// RNG rng;
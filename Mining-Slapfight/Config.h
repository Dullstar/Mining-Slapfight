#pragma once

class Config
{
private:
	int tilesize;
	int screensizeX;
	int screensizeY;
	int pixelsize;
	int uisizeX;
public:
	Config(int tilesize_, int pixelsize_, int screensizeX_, int screensizeY_, int uisizeX_);
	void apply();
};
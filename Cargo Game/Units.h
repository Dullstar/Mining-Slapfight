#pragma once
#include "Objects.h"

class Unit : public Object
{
public:
	Unit(int x, int y, char player, OG graphic, std::string name);
	std::vector<int> collision_area() override;
	std::vector<int> catchment_area() override;
	std::vector<int> get_moves() override;
};

class Worker : public Unit
{
private:
public:
	Worker(int x, int y, char player, OG graphic);
};

class Puncher : public Unit
{
private:
public:
	Puncher(int x, int y, char player, OG graphic);
};
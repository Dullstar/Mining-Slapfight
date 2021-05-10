#pragma once

#include <vector>
#include <queue>
#include "RNG.h"
#include <memory>
#include "Objects.h"
class WorldGen
{
private:

public:
	static std::vector<char> create(int x, int y);
	static std::vector<std::unique_ptr<Object>> create_objects(std::vector<char>& terrain, OGPC& ogpc);
};


#pragma once
#include <vector>

struct gridState {
	bool active[3][3];
	std::vector<int> percentage[3][3];
};
#pragma once

#include <queue>

struct gridState {
	bool active[3][3];
	std::queue<int> endTimes[3][3];
};

void draw_grid();
void draw_run_data();
void pass_window_size(int width, int height);
void pass_grid_lines_thickness(int thickness);
void pass_border_thickness(int thickness);
void pass_grid_state(gridState newState);
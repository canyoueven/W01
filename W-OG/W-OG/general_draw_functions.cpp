#include <queue>

#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>

#include "general_draw_functions.h"

gridState state;

int windowW, windowH;
int borderThickness, gridLinesThickness;

void pass_window_size(int width, int height) {
	windowW = width;
	windowH = height;
}

void pass_grid_lines_thickness(int thickness) {
	gridLinesThickness = thickness;
}

void pass_border_thickness(int thickness) {
	borderThickness = thickness;
}

void pass_grid_state(gridState newState) {
	state = newState;
}

void draw_grid_lines(int cellSideLength, ALLEGRO_COLOR color, float thickness);
void draw_cell_borders(int cellSideLength, ALLEGRO_COLOR color, float thickness);

void draw_grid() {
	ALLEGRO_COLOR gridLineColor = al_map_rgb(255, 255, 255),
				  borderColor = al_map_rgb(255, 0, 0);
	//const float thickness = 10.0;
	const int cellSideLength = (windowH * 4) / 15;

	draw_grid_lines(cellSideLength, gridLineColor, gridLinesThickness);
	draw_cell_borders(cellSideLength, borderColor, borderThickness);
}

void draw_run_data() {

}

void draw_grid_lines(int cellSideLength, ALLEGRO_COLOR color, float thickness) {
	for (int i = 0; i <= 3; ++i) {
		al_draw_line((windowW - cellSideLength * 3) / 2 + cellSideLength * (i), windowH - cellSideLength * 3 - thickness,
			(windowW - cellSideLength * 3) / 2 + cellSideLength * (i), windowH,
			color, thickness);

		al_draw_line((windowW - cellSideLength * 3) / 2, windowH - (cellSideLength * i) - thickness / 2,
			windowW - (windowW - cellSideLength * 3) / 2, windowH - (cellSideLength * i) - thickness / 2,
			color, thickness);
	}
}

void border(int x, int y, int cellSideLength, ALLEGRO_COLOR color, int thickness) {
	al_draw_rectangle((windowW - cellSideLength * 3) / 2 + cellSideLength * x + thickness / 2, windowH - cellSideLength * (y + 1),
		(windowW - cellSideLength * 3) / 2 + cellSideLength * (x + 1) - thickness / 2, windowH - cellSideLength * y - thickness,
		color, thickness);
}

//void border(int x, int y, int cellSideLength, ALLEGRO_COLOR color, int thickness) {
//	al_draw_rectangle((windowW - cellSideLength * 3) / 2 + cellSideLength * x + thickness / 2, windowH - cellSideLength * (y + 1),
//		(windowW - cellSideLength * 3) / 2 + cellSideLength * (x + 1) - thickness / 2, windowH - cellSideLength * y - thickness,
//		color, thickness);
//}


void draw_cell_borders(int cellSideLength, ALLEGRO_COLOR color, float thickness) {
	for (int i = 2; i > -1; --i) {
		for (int j = 0; j < 3; ++j) {
			if (state.active[i][j]) {
				border(j, i, cellSideLength, color, thickness);
			}
		}
	}
}

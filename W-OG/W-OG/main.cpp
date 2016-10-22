#include <iostream>
#include <queue>
#include <Windows.h>

#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_ttf.h>

//constante provizorii  1280×800, 
const int defaultWindowW = 800, defaultWindowH = 600;

struct gridState {
	bool active[3][3];
	std::queue<int> endTimes[3][3];
} state;

ALLEGRO_EVENT_QUEUE *eventQueue;
ALLEGRO_DISPLAY *window;
ALLEGRO_TIMER *frameTimer;
int windowW, windowH;

void deserialize() {
	windowW = defaultWindowW;
	windowH = defaultWindowH;
}

void serialize() {

}

bool loopEnd;

void initialize() {
	al_init();
	al_install_keyboard();
	al_install_mouse();
	al_install_joystick();

	al_init_primitives_addon();

	deserialize();
	window = al_create_display(windowW, windowH);

	eventQueue = al_create_event_queue();
	al_register_event_source(eventQueue, al_get_keyboard_event_source());

	loopEnd = 0;
}

void end() {
	al_uninstall_joystick();
	al_uninstall_mouse();
	al_uninstall_keyboard();
	al_uninstall_system();

	al_unregister_event_source(eventQueue, al_get_keyboard_event_source());
	al_destroy_event_queue(eventQueue);

	serialize();
}

void input();

void move_along() {

}

void draw_grid();
void draw_run_data();

void draw() {
	ALLEGRO_COLOR background_color = al_map_rgb(0, 0, 0);

	al_clear_to_color(background_color);

	draw_grid();
	draw_run_data();

	al_flip_display();
}

void main_loop() {

	while (!loopEnd) {
		input();
		move_along();
		draw();
	}
}

int main(int argc, char **argv) {
	initialize();
	main_loop();
	end();

	system("pause");
	return 0;
}

void draw_grid_lines(float thickness, int cellSideLength, ALLEGRO_COLOR color) {
	for (int i = 0; i <= 3; ++i) {
		al_draw_line((windowW - cellSideLength * 3) / 2 + cellSideLength * (i), windowH -  cellSideLength * 3 - thickness,
					 (windowW - cellSideLength * 3) / 2 + cellSideLength * (i), windowH,
					 color, thickness);

		al_draw_line((windowW - cellSideLength * 3) / 2, windowH - (cellSideLength * i) - thickness / 2,
					  windowW - (windowW - cellSideLength * 3) / 2, windowH - (cellSideLength * i) - thickness / 2, 
					  color, thickness);
	}
}

void border(int x, int y, int cellSideLength, int thickness) {
	ALLEGRO_COLOR borderColor = al_map_rgb(255, 0, 0);
	al_draw_rectangle((windowW - cellSideLength * 3) / 2 + cellSideLength * x + thickness / 2, windowH - cellSideLength * (y + 1) ,
					  (windowW - cellSideLength * 3) / 2 + cellSideLength * (x+1) - thickness / 2, windowH - cellSideLength * y - thickness,
					  borderColor, thickness);
}

void draw_cell_borders(int cellSideLength, int thickness) {
	for (int i = 2; i > -1; --i) {
		for (int j = 0; j < 3; ++j) {
			if (state.active[i][j]) {
				border(j, i, cellSideLength, thickness);
			}
		}
	}
}

void draw_grid() {
	ALLEGRO_COLOR gridLineColor = al_map_rgb(255, 255, 255);
	const float thickness = 10.0;
	const int cellSideLength = (windowH * 4) / 15;
	
	draw_grid_lines(thickness, cellSideLength, gridLineColor);
	draw_cell_borders(cellSideLength, thickness);
}

void draw_run_data() {

}

void printGridState() {
	system("cls");
	for (int i = 2; i > -1; --i) {
		for (int j = 0; j < 3; ++j) {
			std::cerr << state.active[i][j] << ' ';
		}
		std::cerr << '\n';
	}
}

void input() {
	ALLEGRO_EVENT currentEvent;
	while (!al_event_queue_is_empty(eventQueue)) {
		al_get_next_event(eventQueue, &currentEvent);
		int currentKeycode = currentEvent.keyboard.keycode,
			num1Keycode = 38;
		
		if (currentEvent.type == ALLEGRO_EVENT_KEY_DOWN) {
			state.active[(currentKeycode - num1Keycode) / 3][(currentKeycode - num1Keycode) % 3] = 1;
			
			printGridState();
		}
		if (currentEvent.type == ALLEGRO_EVENT_KEY_UP) {
			state.active[(currentKeycode - num1Keycode) / 3][(currentKeycode - num1Keycode) % 3] = 0;

			printGridState();
		}
	}
}
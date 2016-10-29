#include <iostream>
#include <queue>
#include <Windows.h>

#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_ttf.h>

#include "general_draw_functions.h"

//constante provizorii  1280×800
const int defaultWindowW = 800, defaultWindowH = 600,
		  frameCap = 60;
const float prGridLinesThickness = 10.0, prBorderThickness = 10.0;
const double frameTime = 1 / 60;

gridState generalState;
//struct gridState {
//	bool active[3][3];
//	std::queue<int> endTimes[3][3];
//} generalState;

ALLEGRO_EVENT_QUEUE *eventQueue;
ALLEGRO_DISPLAY *window;
ALLEGRO_TIMER *frameTimer;
int windowWidth, windowHeight;

void deserialize() {
	windowWidth = defaultWindowW;
	windowHeight = defaultWindowH;

	pass_grid_lines_thickness(prGridLinesThickness);
	pass_border_thickness(prBorderThickness);
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
	al_init_ttf_addon();

	deserialize();
	window = al_create_display(windowWidth, windowHeight);

	eventQueue = al_create_event_queue();
	al_register_event_source(eventQueue, al_get_keyboard_event_source());

	loopEnd = 0;

	pass_window_size(windowWidth, windowHeight);
}

void end() {
	al_uninstall_joystick();
	al_uninstall_mouse();
	al_uninstall_keyboard();
	al_uninstall_system();

	al_unregister_event_source(eventQueue, al_get_keyboard_event_source());
	al_destroy_event_queue(eventQueue);

	al_shutdown_primitives_addon();
	al_shutdown_ttf_addon();

	serialize();
}

void handle_input();

void handle_events() {

}

void draw_scene() {
	ALLEGRO_COLOR background_color = al_map_rgb(0, 0, 0);

	al_clear_to_color(background_color);

	pass_grid_state(generalState);
	draw_grid();
	draw_run_data();

	al_flip_display();
}

void main_loop() {

	while (!loopEnd) {
		handle_input();
		handle_events();
		draw_scene();
	}
}

int main(int argc, char **argv) {
	initialize();
	main_loop();
	end();

	system("pause");
	return 0;
}

void printGridState() {
	system("cls");
	for (int i = 2; i > -1; --i) {
		for (int j = 0; j < 3; ++j) {
			std::cerr << generalState.active[i][j] << ' ';
		}
		std::cerr << '\n';
	}
}

void handle_input() {
	ALLEGRO_EVENT currentEvent;
	while (!al_event_queue_is_empty(eventQueue)) {
		al_get_next_event(eventQueue, &currentEvent);
		int currentKeycode = currentEvent.keyboard.keycode,
			num1Keycode = 38;
		
		if (currentEvent.type == ALLEGRO_EVENT_KEY_DOWN) {
			generalState.active[(currentKeycode - num1Keycode) / 3][(currentKeycode - num1Keycode) % 3] = 1;
			
			printGridState();
		}
		if (currentEvent.type == ALLEGRO_EVENT_KEY_UP) {
			generalState.active[(currentKeycode - num1Keycode) / 3][(currentKeycode - num1Keycode) % 3] = 0;

			printGridState();
		}
	}
}
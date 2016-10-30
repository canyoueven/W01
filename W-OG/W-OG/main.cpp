#include <iostream>
#include <queue>
#include <time.h>
#include <Windows.h>

#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_ttf.h>

#include "custom_structs.h"
#include "general_draw_functions.h"

//constante provizorii  1280×800
const int defaultWindowW = 800, defaultWindowH = 600,
		  frameCap = 60,
		  waveSpeed = 500, approachRate = 1, incrementSpeed = 25,
		  maxWaveSize = 2;
const float prGridLinesThickness = 10.0, prBorderThickness = 10.0;
const double frameTime = 1 / 60;

gridState generalState;

ALLEGRO_EVENT_QUEUE *eventQueue;
ALLEGRO_DISPLAY *window;
ALLEGRO_TIMER *frameTimer, *approachTimer, *waveTimer;
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

	srand(time(NULL));
	pass_window_size(windowWidth, windowHeight);

	frameTimer = al_create_timer(1.0 / frameCap);
	approachTimer = al_create_timer(1.0 / 1000);
	waveTimer = al_create_timer(1.0 / 1000);

	al_start_timer(frameTimer);
	al_start_timer(approachTimer);
	al_start_timer(waveTimer);


	al_set_timer_count(waveTimer, waveSpeed);
}

void end() {
	al_uninstall_joystick();
	al_uninstall_mouse();
	al_uninstall_keyboard();
	al_uninstall_system();
	
	//al_unregister_event_source(eventQueue, al_get_keyboard_event_source());
	//al_destroy_event_queue(eventQueue);
	
	//al_destroy_timer(frameTimer);
	//al_destroy_timer(approachTimer);
	//al_destroy_timer(waveTimer);

	al_shutdown_primitives_addon();
	al_shutdown_ttf_addon();

	serialize();
}

void handle_input();

void randomize_matrix(bool (&matrix)[3][3]) {
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			matrix[i][j] = rand() % 2;
			std::cout << matrix[i][j] << ' ';
		}
		std::cout << '\n';
	}
}

/*void generate_next_wave() {
	bool waveMatrix[3][3];
	randomize_matrix(waveMatrix);

	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			if (waveMatrix[i][j]) {
				generalState.percentage[i][j].push_back(1);
			}
		}
	}
}*/

void generate_next_wave() {
	bool waveMatrix[3][3];
	int waveSize = rand() % maxWaveSize + 1;

	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			waveMatrix[i][j] = 0;
		}
	}
	for (int i = 0; i < waveSize; ++i) {
		int x = rand() % 3, y = rand() % 3;
		while (waveMatrix[x][y]) {
			x = rand() % 3, y = rand() % 3;
		}

		waveMatrix[x][y] = 1;
	}

	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			if (waveMatrix[i][j]) {
				generalState.percentage[i][j].push_back(1);
			}
		}
	}
}

void check_fail_state() {
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			if (generalState.percentage[i][j].size() && generalState.percentage[i][j][0] >= 100) {
				//std::cout << i << ' ' << j << '\n';
				loopEnd = 1;
				//system("pause");
			}
		}
	}
}

void increment_waves() {
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			for (int k = 0; k < generalState.percentage[i][j].size(); ++k) {
				generalState.percentage[i][j][k] += approachRate;
			}
		}
	}

	check_fail_state();
}

void handle_events() {
	/*system("cls");
	std::cout << al_get_timer_count(approachTimer) << ' ';
	std::cout << al_get_timer_count(waveTimer) << ' ';
	*/
	if (al_get_timer_count(approachTimer) >= incrementSpeed) {
		al_set_timer_count(approachTimer, al_get_timer_count(approachTimer) - incrementSpeed);

		//std::cout << "waves incremented\n";
		increment_waves();
	}
	
	if (al_get_timer_count(waveTimer) >= waveSpeed) {
		al_set_timer_count(waveTimer, al_get_timer_count(waveTimer) - waveSpeed);

		//std::cout << "next wave\n";
		generate_next_wave();
	}
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

		if (al_get_timer_count(frameTimer) >= 1) {
			al_set_timer_count(frameTimer, al_get_timer_count(frameTimer) - 1);
			
			draw_scene();
		}
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

void numKeyDown(int keycode) {
	int i = keycode / 3,
		j = keycode % 3;

	generalState.active[i][j] = 1;

	if (generalState.percentage[i][j].size()) {
		generalState.percentage[i][j].erase(generalState.percentage[i][j].begin());
	}
}

void handle_input() {
	ALLEGRO_EVENT currentEvent;
	while (!al_event_queue_is_empty(eventQueue)) {
		al_get_next_event(eventQueue, &currentEvent);
		int currentKeycode = currentEvent.keyboard.keycode,
			num1Keycode = 38;
		
		if (currentEvent.type == ALLEGRO_EVENT_KEY_DOWN) {
			if (currentKeycode >= num1Keycode && currentKeycode <= num1Keycode + 9) {
				numKeyDown(currentKeycode - num1Keycode);
			}

			//printGridState();
		}
		if (currentEvent.type == ALLEGRO_EVENT_KEY_UP) {
			if (currentKeycode >= num1Keycode && currentKeycode <= num1Keycode + 9) {
				generalState.active[(currentKeycode - num1Keycode) / 3][(currentKeycode - num1Keycode) % 3] = 0;
			}

			//printGridState();
		}
	}
}
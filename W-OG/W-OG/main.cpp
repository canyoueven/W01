#include <iostream>
#include <queue>
#include <map>
#include <string>
#include <time.h>
#include <Windows.h>

#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_ttf.h>

#include "custom_structs.h"
#include "general_draw_functions.h"
#include "deserializer.h"
#include "serializer.h"

//constante provizorii
int defaultWindowW = 800, defaultWindowH = 600,
		  frameCap = 60,
		  waveSpeed = 500, approachRate = 25, incrementSpeed = 25,
		  maxWaveSize = 2;
const float prGridLinesThickness = 10.0, prBorderThickness = 10.0;
const double frameTime = 1 / 60;

bool play, loopEnd;

gridState generalState;

ALLEGRO_COLOR background_color;

ALLEGRO_EVENT_QUEUE *eventQueue;
ALLEGRO_DISPLAY *window;
ALLEGRO_TIMER *frameTimer, *approachTimer, *waveTimer;


int windowWidth, windowHeight;

void deserialize() {
	deserializer::deserialize();
	
	std::map<std::string, int> settingsData = deserializer::pass_data();

	windowWidth = settingsData["window_width"];
	windowHeight = settingsData["window_height"];
	frameCap = settingsData["frame_cap"];
	waveSpeed = settingsData["wave_speed"];
	maxWaveSize = settingsData["wave_max_size"] - 1;
	if (maxWaveSize < 1) {
		maxWaveSize = 1;
	}
	approachRate = settingsData["approach_speed"];


	background_color = al_map_rgb(0, 0, 0);

	pass_grid_lines_thickness(prGridLinesThickness);
	pass_border_thickness(prBorderThickness);
}

void serialize() {
}

void stop_timers();
void start_timers();
void resume_timers();
void create_timers();

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
	play = 1;

	srand(time(NULL));
	pass_window_size(windowWidth, windowHeight);
	
	create_timers();
	start_timers();
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
				loopEnd = 1;
			}
		}
	}
}

void increment_waves() {
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			for (int k = 0; k < generalState.percentage[i][j].size(); ++k) {
				++generalState.percentage[i][j][k];
			}
		}
	}

	check_fail_state();
}

void handle_events() {
	if (al_get_timer_count(approachTimer) >= approachRate) {
		al_set_timer_count(approachTimer, 0);

		increment_waves();
	}
	
	if (al_get_timer_count(waveTimer) >= waveSpeed) {
		al_set_timer_count(waveTimer, 0);

		generate_next_wave();
	}
}

void draw_scene() {
	al_clear_to_color(background_color);

	pass_grid_state(generalState);
	draw_grid();
	draw_run_data();

	al_flip_display();
}

void game_over_screen();

void restart();

void initialize_grid_state();

void main_loop() {
	start_timers();
	while (!loopEnd) {
		handle_input();
		handle_events();

		if (al_get_timer_count(frameTimer) >= 1) {
			al_add_timer_count(frameTimer, -1);
			
			draw_scene();
		}
	}

	game_over_screen();
}

int main(int argc, char **argv) {
	initialize();

	while (play == 1) {
		main_loop();
	}
	end();

	system("pause");
	return 0;
}

int some_key_down = 0;

void al_wait_for_input() {
	stop_timers();

	while (!some_key_down) {
		handle_input();
	}
}

void game_over_screen() {
	al_wait_for_input();

		/*std::cout << "Restart in 3..\n";
		Sleep(1000);
		std::cout << "2..\n";
		Sleep(1000);
		std::cout << "1..\n";
		Sleep(1000);*/

	restart();
}

void restart() {
	loopEnd = 0;

	initialize_grid_state();
}

void initialize_grid_state() {
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			generalState.active[i][j] = 0;
			while (generalState.percentage[i][j].size() > 0) {
				generalState.percentage[i][j].pop_back();
			}
			//generalState.percentage[i][j].clear();
		}
	}
}

void init_timers() {
	al_set_timer_count(frameTimer, 0);
	al_set_timer_count(waveTimer, 0);
	al_set_timer_count(approachTimer, 0);
}

void stop_timers() {
	al_stop_timer(frameTimer);
	al_stop_timer(waveTimer);
	al_stop_timer(approachTimer);
}

void start_timers() {
	al_start_timer(frameTimer);
	al_start_timer(waveTimer);
	al_start_timer(approachTimer);

	init_timers();
}

void resume_timers() {
	al_resume_timer(frameTimer);
	al_resume_timer(waveTimer);
	al_resume_timer(approachTimer);

	init_timers();
}

void create_timers() {
	frameTimer = al_create_timer(1.0 / frameCap);
	approachTimer = al_create_timer(1.0 / 1000);
	waveTimer = al_create_timer(1.0 / 1000);
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
			some_key_down = 1;
			if (currentKeycode >= num1Keycode && currentKeycode <= num1Keycode + 9) {
				numKeyDown(currentKeycode - num1Keycode);
			}
		}
		if (currentEvent.type == ALLEGRO_EVENT_KEY_UP) {
			some_key_down = 0;
			if (currentKeycode >= num1Keycode && currentKeycode <= num1Keycode + 9) {
				generalState.active[(currentKeycode - num1Keycode) / 3][(currentKeycode - num1Keycode) % 3] = 0;
			}
		}
	}
}
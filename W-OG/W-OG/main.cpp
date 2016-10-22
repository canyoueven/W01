#include <iostream>

#include <Windows.h>

#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_ttf.h>

//constante provizorii  1280×800, 
const int defaultWindowW = 800, defaultWindowH = 600;

bool gridState[3][3];

ALLEGRO_EVENT_QUEUE *queue;
ALLEGRO_DISPLAY *window;
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

	queue = al_create_event_queue();
	al_register_event_source(queue, al_get_keyboard_event_source());

	loopEnd = 0;
}

void end() {
	al_uninstall_joystick();
	al_uninstall_mouse();
	al_uninstall_keyboard();
	al_uninstall_system();

	al_unregister_event_source(queue, al_get_keyboard_event_source());
	al_destroy_event_queue(queue);

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

void draw_grid() {
	ALLEGRO_COLOR gridLineColor = al_map_rgb(255, 255, 255);
	const float thickness = 10.0;
	
	draw_grid_lines(thickness, (windowH * 4) / 15, gridLineColor);
}

void draw_run_data() {

}

void input() {
	ALLEGRO_EVENT currentEvent;
	while (!al_event_queue_is_empty(queue)) {
		al_get_next_event(queue, &currentEvent);
		
		if (currentEvent.type == ALLEGRO_EVENT_KEY_DOWN) {
			int currentKeycode = currentEvent.keyboard.keycode;
			
			std::cerr << currentEvent.keyboard.keycode << '\n';
		}
	}
}
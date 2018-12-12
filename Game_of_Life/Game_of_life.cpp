#include <SDL.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <limits>
#include <ctime>

using namespace std;
using namespace chrono;

void update_array(bool** old_array, bool** new_field, const int max_cols, const int max_rows, const int field_size, const Uint32* chunk_true, const Uint32* chunk_false);
bool check_cell(bool** old_array, int row, int col, const int max_cols, const int max_rows);

SDL_Texture *texture;

int main(int argc, char *argv[]) {

	srand(time(0));

	const int field_size = 1;

	const int max_rows = 1000;
	const int max_cols = 1080;

	const Uint32 color_false = 0;
	const Uint32 color_true = numeric_limits<Uint32>::max();

	int width = max_cols * field_size;
	int height = max_rows * field_size;
	
	Uint32* chunk_true = new Uint32[field_size * field_size];
	Uint32* chunk_false = new Uint32[field_size * field_size];

	for (int i = 0; i < field_size * field_size; i++) {
		chunk_true[i] = color_true;
		chunk_false[i] = color_false;
	}

	bool **old = new bool*[max_rows];
	bool **new_field = new bool*[max_rows];

	for (int i = 0; i < max_cols; ++i) {
		old[i] = new bool[max_cols];
		new_field[i] = new bool[max_cols];
	}

	for (int col = 0; col < max_cols; ++col) {
		for (int row = 0; row < max_rows; ++row) {
			old[row][col] = rand() % 10 == 0;
		}
	}

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window *screen = SDL_CreateWindow("Game of Life", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
	SDL_Renderer *renderer = SDL_CreateRenderer(screen, -1, 0);
	SDL_Event event;
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
	
	for (;;) {


		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				goto exit_app;
			}
		}

		update_array(old, new_field, max_cols, max_rows, field_size, chunk_true, chunk_false);
		old = new_field;

		//Redraw window
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
	}

exit_app:
	return 0;
}

void update_array(bool** old_array, bool** new_field, const int max_cols, const int max_rows, const int field_size, const Uint32* chunk_true, const Uint32* chunk_false) {

	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.h = field_size;
	rect.w = field_size;

	for (int row = 0; row < max_rows; ++row) {
		for (int col = 0; col < max_cols; ++col) {
			new_field[row][col] = check_cell(old_array, row, col, max_rows, max_cols);
			if (new_field[row][col]) {
				SDL_UpdateTexture(texture, &rect, chunk_true, field_size * sizeof(Uint32));
			}
			else {
				SDL_UpdateTexture(texture, &rect, chunk_false, field_size * sizeof(Uint32));
			}
			rect.x = rect.x + field_size;
		}
		rect.y = rect.y + field_size;
		rect.x = 0;
	}
}

bool check_cell(bool** old_array, int row, int col, const int max_rows, const int max_cols) {

	int alive_count = 0;

	bool upper_left = false;
	bool upper_center = false;
	bool upper_right = false;
	bool middle_left = false;
	bool middle_right = false;
	bool bottom_left = false;
	bool bottom_center = false;
	bool bottom_right = false;

	if (row != 0 && row != (max_rows - 1) && col != 0 && col != (max_cols - 1)) {
		upper_left = old_array[row - 1][col - 1];
		upper_center = old_array[row - 1][col];
		upper_right = old_array[row - 1][col + 1];
		middle_left = old_array[row][col - 1];
		middle_right = old_array[row][col + 1];
		bottom_left = old_array[row + 1][col - 1];
		bottom_center = old_array[row + 1][col];
		bottom_right = old_array[row + 1][col + 1];
	}
	else if (row == 0) {
		if (0 < col && col < (max_cols - 1)) {
			upper_left = old_array[(max_rows - 1)][col - 1];
			upper_center = old_array[(max_rows - 1)][col];
			upper_right = old_array[(max_rows - 1)][col + 1];
			middle_left = old_array[row][col - 1];
			middle_right = old_array[row][col + 1];
			bottom_left = old_array[row + 1][col - 1];
			bottom_center = old_array[row + 1][col];
			bottom_right = old_array[row + 1][col + 1];
		}
		else if (col == 0) {
			upper_left = old_array[(max_rows - 1)][(max_cols - 1)];
			upper_center = old_array[(max_rows - 1)][col];
			upper_right = old_array[(max_rows - 1)][col + 1];
			middle_left = old_array[row][(max_cols - 1)];
			middle_right = old_array[row][col + 1];
			bottom_left = old_array[row + 1][(max_cols - 1)];
			bottom_center = old_array[row + 1][col];
			bottom_right = old_array[row + 1][col + 1];
		}
		else if (col == (max_cols - 1)) {
			upper_left = old_array[(max_rows - 1)][col - 1];
			upper_center = old_array[(max_rows - 1)][col];
			upper_right = old_array[(max_rows - 1)][0];
			middle_left = old_array[row][col - 1];
			middle_right = old_array[row][0];
			bottom_left = old_array[row + 1][col - 1];
			bottom_center = old_array[row + 1][col];
			bottom_right = old_array[row + 1][0];
		}
	}
	else if (row == max_rows - 1) {
		if (0 < col && col < max_cols - 1) {
			upper_left = old_array[row - 1][col - 1];
			upper_center = old_array[row - 1][col];
			upper_right = old_array[row - 1][col + 1];
			middle_left = old_array[row][col - 1];
			middle_right = old_array[row][col + 1];
			bottom_left = old_array[0][col - 1];
			bottom_center = old_array[0][col];
			bottom_right = old_array[0][col + 1];
		}
		else if (col == 0) {
			upper_left = old_array[row - 1][(max_cols - 1)];
			upper_center = old_array[row - 1][col];
			upper_right = old_array[row - 1][col + 1];
			middle_left = old_array[row][(max_cols - 1)];
			middle_right = old_array[row][col + 1];
			bottom_left = old_array[0][(max_cols - 1)];
			bottom_center = old_array[0][col];
			bottom_right = old_array[0][col + 1];
		}
		else if (col == (max_cols - 1)) {
			upper_left = old_array[row - 1][col - 1];
			upper_center = old_array[row - 1][col];
			upper_right = old_array[row - 1][0];
			middle_left = old_array[row][col - 1];
			middle_right = old_array[row][0];
			bottom_left = old_array[0][col - 1];
			bottom_center = old_array[0][col];
			bottom_right = old_array[0][0];
		}
	}
	else if (col == 0) {
		upper_left = old_array[row - 1][(max_cols - 1)];
		upper_center = old_array[row - 1][col];
		upper_right = old_array[row - 1][col + 1];
		middle_left = old_array[row][(max_cols - 1)];
		middle_right = old_array[row][col + 1];
		bottom_left = old_array[row + 1][(max_cols - 1)];
		bottom_center = old_array[row + 1][col + 1];
		bottom_right = old_array[row + 1][col + 1];
	}
	else if (col == (max_cols - 1)) {
		upper_left = old_array[row - 1][col - 1];
		upper_center = old_array[row - 1][col];
		upper_right = old_array[row - 1][0];
		middle_left = old_array[row][col - 1];
		middle_right = old_array[row][0];
		bottom_left = old_array[row + 1][col - 1];
		bottom_center = old_array[row + 1][col];
		bottom_right = old_array[row + 1][0];
	}

	if (upper_left) {
		++alive_count;
	}
	if (upper_center) {
		++alive_count;
	}
	if (upper_right) {
		++alive_count;
	}
	if (middle_left) {
		++alive_count;
	}
	if (middle_right) {
		++alive_count;
	}
	if (bottom_left) {
		++alive_count;
	}
	if (bottom_center) {
		++alive_count;
	}
	if (bottom_right) {
		++alive_count;
	}

	if (alive_count < 2) {
		return false;
	}
	if (old_array[row][col] && (alive_count == 2 || alive_count == 3)) {
		return true;
	}
	if (alive_count > 3) {
		return false;
	}
	if (!(old_array[row][col]) && alive_count == 3) {
		return true;
	}
}
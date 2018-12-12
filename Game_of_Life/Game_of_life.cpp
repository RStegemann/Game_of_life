#include <SDL.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <limits>
#include <ctime>
#include "mpi.h"



using namespace std;
using namespace chrono;

void update_array(bool** old_array, bool** new_field);
bool check_cell(bool** old_array, int row, int col);

int max_cols;
int max_rows;
int field_size;

SDL_Texture *texture;
Uint32 color_true;
Uint32 color_false;

int main(int argc, char *argv[]) {

	srand(time(0));

	field_size = 5;

	max_rows = 100;
	max_cols = 100; 
	
	color_false = 0;
	color_true = numeric_limits<Uint32>::max();

	int width = max_cols * field_size;
	int height = max_rows * field_size;

	bool **old = new bool*[max_rows];
	for (int i = 0; i < max_cols; ++i) {
		old[i] = new bool[max_cols];
	}

	bool **new_field = new bool*[max_rows];
	for (int i = 0; i < max_cols; ++i) {
		new_field[i] = new bool[max_cols];
	}

	for (int col = 0; col < max_cols; ++col) {
		for (int row = 0; row < max_rows; ++row) {
			old[row][col] = rand() % 2;
		}
	}

	//this_thread::sleep_for(milliseconds(10000));

	SDL_Window *screen = SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
	SDL_Event event;
	SDL_Renderer *renderer = SDL_CreateRenderer(screen, -1, 0);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);

	/*int numtasks, rank;

	int rc = MPI_Init(NULL, NULL);
	if (rc != MPI_SUCCESS) {
		printf("Error starting MPI program. Terminating.\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	cout << "Greetings from Process: " << rank << "\n";
	MPI_Finalize();*/

	for (;;) {
		
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				goto exit_app;
			}
		}

		update_array(old, new_field);
		old = new_field;

		//Redraw window
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
	}

	exit_app:
		return 0;
}

void update_array(bool** old_array, bool** new_field) {

	Uint32 *pixels = new Uint32[max_cols * field_size];

	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.h = 1;
	rect.w = field_size * max_cols;

	for (int row = 0; row < max_rows; ++row) {
		for (int col = 0; col < max_cols; ++col) {
			cout << col  << "," << row << endl;
			new_field[row][col] = check_cell(old_array, row, col);

			int offset = col * field_size;
			if (new_field[row][col]) {
				for (int i = 0; i < field_size; ++i) {
					pixels[offset + i] = color_true;
				}
			}
			else {
				for (int i = 0; i < field_size; ++i) {
					pixels[offset + i] = color_false;
				}
			}
		}

		for (int i = 0; i < field_size; ++i) {
			SDL_UpdateTexture(texture, &rect, pixels, (field_size * max_cols) * sizeof(Uint32));
			++rect.y;
		}
	}
}

bool check_cell(bool** old_array, int row, int col) {

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
			upper_left = old_array[row - 1][(max_cols -1)];
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
	if (old_array[row][col] && alive_count <= 3) {
		return true;
	}
	if (alive_count > 3) {
		return false;
	}
	if (!(old_array[row][col]) && alive_count == 3) {
		return true;
	}
}
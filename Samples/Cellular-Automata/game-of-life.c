/*

Consult http://libsdl.org/ to get SDL2(zlib license) for your platform.

--------------------------------------
compilation using MSVC(for windows):
--------------------------------------
In developer cmd:
rem Do be wary that you'll need to put the path like D:/foo/bar or D:\\foo\\bar (because windows and escape sequences)
rem also, when doing creating a variable using set, make sure that the = sign is right next to the var name like in the code, otherwise you'll need to add that extra space when using the variable as well eg
rem set var = "Hello"
rem echo %var %

set inc_path="PATH TO THE INCLUDE FOLDER"						
set lib_path="PATH TO THE LIB FOLDER"
set dll_path="PATH TO SDL2.lib"
set exec_name="Game Of Life.exe"

xcopy %dll_path% .\ /Y
cl .\main.c /Fe%exec_name% /I%inc_path% /link /LIBPATH:%lib_path% SDL2.lib SDL2main.lib shell32.lib /subsystem:console

--------------------------------------
compilation using gcc(for linux):
--------------------------------------

gcc main.c -lSDL2 -o main

--------------------------------------
*/

#include <SDL2/SDL.h> //while this is the way you can access SDL headers in linux, windows users will need to create a SDL2 folder where they can put the include files
#include <string.h>
#include <stdlib.h>

#define CELL_WIDTH 8
#define ROW_WIDTH 160
#define COL_WIDTH 100
#define true 1
#define false 0
#define cell(x, y) output[(y)*ROW_WIDTH + (x)]

SDL_Window *_window = NULL;
SDL_Renderer *_renderer = NULL;

int init_sdl_and_create_window()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		return false;
	_window = SDL_CreateWindow("Game of life", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 160 * 8, 100 * 8, 0);
	if (_window == NULL)
		return false;
	_renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
	if (_renderer == NULL)
		return false;

	SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
	SDL_RenderClear(_renderer);
	SDL_RenderPresent(_renderer);
	return true;
}

void clean_up()
{
	SDL_DestroyRenderer(_renderer);
	SDL_DestroyWindow(_window);
	SDL_Quit();
}

//to make creating the cells more simplified
void set(int x, int y, char *string, int *state)
{
	for (int p = 0; *string != '\0'; string++, p++)
		state[y * ROW_WIDTH + x + p] = *string == '#' ? 1 : 0;
}

void reset_screen(int* state, int* output, int size)
{
	memset(output, 0, size);
	memset(state, 0, size);
}

#define BUFFER_SIZE (sizeof(int) * (ROW_WIDTH * COL_WIDTH))

void create_glider_gun(int* state)
{
	set(60, 45, "........................#............", state);
	set(60, 46, "......................#.#............", state);
	set(60, 47, "............##......##............##.", state);
	set(60, 48, "...........#...#....##............##.", state);
	set(60, 49, "##........#.....#...##...............", state);
	set(60, 50, "##........#...#.##....#.#............", state);
	set(60, 51, "..........#.....#.......#............", state);
	set(60, 52, "...........#...#.....................", state);
	set(60, 53, "............##.......................", state);
}

void create_r_pentomino(int* state)
{
	set(80, 50, "  ## ", state);
	set(80, 51, " ##  ", state);
	set(80, 52, "  #  ", state);
}

void create_die_hard(int* state)
{
	set(60, 50, ".......#.", state);
	set(60, 51, ".##......", state);
	set(60, 52, "..#...###", state);
}

void create_acorn(int* state)
{
	set(60, 50, "..#......", state);
	set(60, 51, "....#....", state);
	set(60, 52, ".##..###", state);
}

void create_space_rake(int* state)
{
	set(20, 20, "...........##.....####", state);
	set(20, 21, ".........##.##...#...#", state);
	set(20, 22, ".........####........#", state);
	set(20, 23, "..........##.....#..#.", state);
	set(20, 24, "......................", state);
	set(20, 25, "........#.............", state);
	set(20, 26, ".......##........##...", state);
	set(20, 27, "......#.........#..#..", state);
	set(20, 28, ".......#####....#..#..", state);
	set(20, 29, "........####...##.##..", state);
	set(20, 30, "...........#....##....", state);
	set(20, 31, "......................", state);
	set(20, 32, "......................", state);
	set(20, 33, "......................", state);
	set(20, 34, "..................####", state);
	set(20, 35, "#..#.............#...#", state);
	set(20, 36, "....#................#", state);
	set(20, 37, "#...#............#..#.", state);
	set(20, 38, ".####.................", state);
}

void create_infinite_growth(int* state)
{
	set(20, 50, "########.#####...###......#######.#####", state);
}

void draw_cell(int value, int x, int y, SDL_Rect rect)
{
	SDL_SetRenderDrawColor(_renderer, value * 255, value * 255, value * 255, 255);
	rect.x = x * rect.w;
	rect.y = y * rect.h;
	SDL_RenderFillRect(_renderer, &rect);
}

int main(int argc, char **argv)
{
	if (init_sdl_and_create_window())
	{
		//Creating two cells to make it so that the state of the board wouldn't be changed when interrogating it
		int *output = malloc(BUFFER_SIZE);
		int *state = malloc(BUFFER_SIZE);
		
		reset_screen(state, output, BUFFER_SIZE);

		create_infinite_growth(state);

		int is_running = true;
		int begin = false;

		SDL_Event event;

		while (is_running)
		{
			int mouse_x, mouse_y;
			int left_clicked = false;
			while (SDL_PollEvent(&event))
			{
				SDL_GetMouseState(&mouse_x, &mouse_y);

				left_clicked = (event.type == SDL_MOUSEBUTTONDOWN) && (event.button.button == SDL_BUTTON_LEFT);

				if (event.type == SDL_QUIT)
					is_running = false;
				if (event.type == SDL_KEYDOWN)
				{
					
					switch(event.key.keysym.scancode)
					{
						case SDL_SCANCODE_ESCAPE:
							is_running = false;
							break;
						case SDL_SCANCODE_SPACE:
							begin = !begin;
							break;
						case SDL_SCANCODE_0:
							reset_screen(state, output, BUFFER_SIZE);
							break;
						case SDL_SCANCODE_1:
							reset_screen(state, output, BUFFER_SIZE);
							create_infinite_growth(state);
							break;
						case SDL_SCANCODE_2:
							reset_screen(state, output, BUFFER_SIZE);
							create_r_pentomino(state);
							break;
						case SDL_SCANCODE_3:
							reset_screen(state, output, BUFFER_SIZE);
							create_space_rake(state);
							break;
						case SDL_SCANCODE_4:
							reset_screen(state, output, BUFFER_SIZE);
							create_acorn(state);
							break;
						case SDL_SCANCODE_5:
							reset_screen(state, output, BUFFER_SIZE);
							create_die_hard(state);
							break;
						case SDL_SCANCODE_6:
							reset_screen(state, output, BUFFER_SIZE);
							create_glider_gun(state);
							break;
					}
				}
			}

			SDL_Rect rect = {0, 0, CELL_WIDTH, CELL_WIDTH}; //creating 8x8 cells

			//Pauses the game
			if (begin)
			{				
				for (int i = 0; i < ROW_WIDTH * COL_WIDTH; i++)
					output[i] = state[i];

				SDL_SetRenderDrawColor(_renderer, 255, 0, 0, 255);
				SDL_RenderClear(_renderer);

				for (int y = 1; y < COL_WIDTH - 1; y++)
				{
					for (int x = 1; x < ROW_WIDTH - 1; x++)
					{
						int neighbours = cell(x - 1, y - 1) + cell(x, y - 1) + cell(x + 1, y - 1) +
										 cell(x - 1, y + 0) + 0 + cell(x + 1, y + 0) +
										 cell(x - 1, y + 1) + cell(x, y + 1) + cell(x + 1, y + 1);

						if (cell(x, y) == 1)
							state[y * ROW_WIDTH + x] = neighbours == 2 || neighbours == 3;
						else
							state[y * ROW_WIDTH + x] = neighbours == 3;

						draw_cell(cell(x,y), x, y, rect);
					}
				}
			}
			else
			{
				if(left_clicked)
				{
					state[(mouse_y / CELL_WIDTH) * ROW_WIDTH + (mouse_x / CELL_WIDTH)] = !state[(mouse_y / CELL_WIDTH) * ROW_WIDTH + (mouse_x / CELL_WIDTH)];
				}

				for (int i = 0; i < ROW_WIDTH * COL_WIDTH; i++)
					output[i] = state[i];

				SDL_SetRenderDrawColor(_renderer, 255, 0, 0, 255);
				SDL_RenderClear(_renderer);

				for (int y = 1; y < COL_WIDTH - 1; y++)
					for (int x = 1; x < ROW_WIDTH - 1; x++)
						draw_cell(cell(x,y), x, y, rect);
			}

			SDL_RenderPresent(_renderer);
			SDL_Delay(10);
		}

		clean_up();
	}
	return 0;
}

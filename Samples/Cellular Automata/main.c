/*

Consult http://libsdl.org/ to get SDL2(zlib license) for your platform.

--------------------------------------
compilation using MSVC(for windows):
--------------------------------------
In developer cmd:

set include_path="PATH TO THE INCLUDE FOLDER"
set lib_path="PATH TO THE LIB FOLDER"
set exec_name="Game Of Life.exe"

cl .\main.c /Fe%exec_name% /I%inc_path% /link /LIBPATH:%lib_path% SDL2.lib SDL2main.lib /subsystem:console

--------------------------------------
compilation using gcc(for linux):
--------------------------------------

gcc main.c -lSDL2 -o main

--------------------------------------
*/

#include <SDL2/SDL.h>
#include <string.h>
#include <stdlib.h>

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
void set(int x, int y, char* string, int* state )
{
	for ( int p = 0; *string != '\0'; string++, p++)
		state[y * ROW_WIDTH + x + p] = *string == '#' ? 1 : 0;
}

int main(int argc, char **argv)
{
	if (init_sdl_and_create_window())
	{
		//Creating two cells to make it so that the state of the board wouldn't be changed when interrogating it
		int *output = malloc(sizeof(int) * (ROW_WIDTH * COL_WIDTH));
		int *state = malloc(sizeof(int) * (ROW_WIDTH * COL_WIDTH));
		memset(output, 0, ROW_WIDTH * COL_WIDTH * sizeof(int));
		memset(state, 0, ROW_WIDTH * COL_WIDTH * sizeof(int));
		
		
		// Infinite Growth
		set(20, 50, "########.#####...###......#######.#####", state);
		
		//can uncomment these and test
		/*
		// Gosper Glider Gun
		set(60, 45, "........................#............", state);
		set(60, 46, "......................#.#............", state);
		set(60, 47, "............##......##............##.", state);
		set(60, 48, "...........#...#....##............##.", state);
		set(60, 49, "##........#.....#...##...............", state);
		set(60, 50, "##........#...#.##....#.#............", state);
		set(60, 51, "..........#.....#.......#............", state);
		set(60, 52, "...........#...#.....................", state);
		set(60, 53, "............##.......................", state);
		// R-Pentomino
		set(80, 50, "  ## ", state);
		set(80, 51, " ##  ", state);
		set(80, 52, "  #  ", state);
		//Die Hard
		set(60, 50, ".......#.", state);
		set(60, 51, ".##......", state);
		set(60, 52, "..#...###", state);
		//Acorn
		set(60, 50, "..#......", state);
		set(60, 51, "....#....", state);
		set(60, 52, ".##..###", state);

		//Space Rake
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
		*/
		
		int is_running = true;
		int begin = false;

		SDL_Event event;

		while (is_running)
		{
			while (SDL_PollEvent(&event))
			{
				if (event.type == SDL_QUIT)
					is_running = false;
				if (event.type == SDL_KEYDOWN)
				{
					if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
						is_running = false;
					if(event.key.keysym.scancode == SDL_SCANCODE_SPACE)
						begin = !begin;
				}
			}
			
			//Pauses the game
			if (!begin)
				continue;
			
			for (int i = 0; i < ROW_WIDTH * COL_WIDTH; i++)
				output[i] = state[i];
			
			SDL_SetRenderDrawColor(_renderer, 255, 0, 0, 255);
			SDL_RenderClear(_renderer);
			SDL_Rect rect = {0, 0, 8, 8}; //creating 8x8 cells

			for (int y = 1; y < COL_WIDTH - 1; y++)
			{
				for (int x = 1; x < ROW_WIDTH - 1; x++)
				{
					int neighbours = cell(x - 1, y - 1) + cell(x, y - 1) + cell(x + 1, y - 1) +
									 cell(x - 1, y + 0) + 		0 		 + cell(x + 1, y + 0) +
									 cell(x - 1, y + 1) + cell(x, y + 1)  + cell(x + 1, y + 1);
					
					if (cell(x, y) == 1)
						state[y * ROW_WIDTH + x] = neighbours == 2 || neighbours == 3;
					else
						state[y * ROW_WIDTH + x] = neighbours == 3;
					
					int value = cell(x, y);
					SDL_SetRenderDrawColor(_renderer, value * 255, value * 255, value * 255, 255);
					rect.x = x * rect.w;
					rect.y = y * rect.h;
					SDL_RenderFillRect(_renderer, &rect);
				}
			}

			SDL_RenderPresent(_renderer);
			SDL_Delay(10);
		}

		clean_up();
	}
	return 0;
}

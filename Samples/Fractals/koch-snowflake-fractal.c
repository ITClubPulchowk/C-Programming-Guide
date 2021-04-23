/*
 * Libraries
 *
 * Consult http://libsdl.org/ to get SDL2(zlib license) for your 
 * platform.

*/

// Authors: Pranjal Pokharel (@pranjalpokharel7)

/*
 * Koch Snowflake
 * --------------------------------------------------------------------------- 
 * Koch Snowflake is a popular fractal pattern. You can refer to 
 * wikipidia for a more general introduction:
 * https://en.wikipedia.org/wiki/Koch_snowflake
 *
 * Additionally, you can refer to this short video:
 * https://www.youtube.com/watch?v=xlZHY0srIew
 *
 * This is a very basic implementation of the koch snowflake. 
 * Contributions and suggestions are welcome.
*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_error.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define MAX_ITER 5

enum bool { false, true };
struct Program_Window_Parameters {
  int SCREEN_WIDTH;
  int SCREEN_HEIGHT;
  int _gwindow_open;

  SDL_Window* _gwindow;
  SDL_Renderer* _grenderer;
}main_window;

// sdl parameters init

int init_sdl_and_window(){
  if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    fprintf(stderr, "Failed to initialize : %s", SDL_GetError());
    return false;
  } 

  if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
  {
    fprintf(stderr, "Warning: Linear texture filtering not enabled!" );
  }

  // define main window parameters
  main_window._gwindow = NULL;
  main_window._grenderer = NULL;
  main_window.SCREEN_HEIGHT = 1000;
  main_window.SCREEN_WIDTH = 1000;

  main_window._gwindow = SDL_CreateWindow("SDL_Window", 
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
      main_window.SCREEN_WIDTH, main_window.SCREEN_HEIGHT, 
      SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);

  if(main_window._gwindow == NULL) {
    fprintf(stderr, "Failed to create window : %s", SDL_GetError());
    return false;
  } 

  main_window._grenderer = SDL_CreateRenderer( main_window._gwindow, 
      -1, SDL_RENDERER_ACCELERATED );

  if(main_window._grenderer == NULL)
  {
    fprintf(stderr, "Renderer could not be created! SDL Error: %s\n", 
        SDL_GetError() );
    return false;
  }

  main_window._gwindow_open = true;
  return true;
}

void clean_up_and_close() {
  SDL_DestroyWindow(main_window._gwindow);
  main_window._gwindow = NULL;

  SDL_DestroyRenderer(main_window._grenderer);
  main_window._grenderer = NULL;

  SDL_Quit();
}

// koch curve

struct Vector{
  int x;
  int y;
};
typedef struct Vector vector;

struct KochLine{
  vector start;
  vector end;
};
typedef struct KochLine koch_line;

void rotate_vector(vector* vec, float theta){
  int temp_x = vec->x;
  vec->x = cos(theta) * vec->x - sin(theta) * vec->y;
  vec->y = sin(theta) * temp_x + cos(theta) * vec->y;
}

void set_triangle_vectors(vector *c, vector *d, vector *e, 
    vector start, vector end){

  vector length = { end.x - start.x, end.y - start.y };
  vector one_third_length = { length.x / 3, length.y / 3 };
  vector two_third_length = { 2 * length.x / 3, 
    2 * length.y / 3 };

  c->x = start.x + one_third_length.x;
  c->y = start.y + one_third_length.y;

  e->x = start.x + two_third_length.x;
  e->y = start.y + two_third_length.y;

  // rotate by 60 degrees, angle to be given in radians
  // sign of magnitude subject to change
  rotate_vector(&one_third_length, 1.0472); 
  d->x = c->x + one_third_length.x;
  d->y = c->y + one_third_length.y;
}

void add_line(koch_line *line_array, vector start_point, 
    vector end_point, int line_number){

  koch_line line;
  line.start = start_point;
  line.end = end_point;

  line_array[line_number] = line;
}


void calculate_koch_curve(koch_line *line_array, vector initial_start_point, 
    vector initial_end_point){
  // initialize koch curve with the first base line
  add_line(line_array, initial_start_point, initial_end_point, 0);
  int final_array_size = pow(4, MAX_ITER);
  koch_line *new_line_array = malloc( final_array_size * sizeof *new_line_array);

  for (int i = 0; i < MAX_ITER ; i++){
    int array_size = pow(4, i+1); // each line produces 4 more sub-lines

    for (int j = 0; j < array_size/4; j++){
      vector triangle_base_start, triangle_base_end, triangle_top;
      set_triangle_vectors(&triangle_base_start, 
          &triangle_top, &triangle_base_end, 
          line_array[j].start, line_array[j].end);

      add_line(new_line_array, line_array[j].start, 
          triangle_base_start, 4*j);
      add_line(new_line_array, triangle_base_start, triangle_top,
          4*j+1);
      add_line(new_line_array, triangle_top, triangle_base_end,
          4*j+2);
      add_line(new_line_array, triangle_base_end, 
          line_array[j].end, 4*j+3);
    }

    // overwriting previous line collection
    for (int k = 0; k < array_size; k++) 
      line_array[k] = new_line_array[k];

  }
  free(new_line_array);
}

int main(int argc, char **argv){
  int final_array_size = pow(4, MAX_ITER);

  // instead of creating a single array that accomodates all 3 initial curves,
  // we do them separately as the calculate_koch_curve() is a more general
  // function implementation that can be used for a single line no matter the
  // vector magnitude and direction
  
  koch_line *line1_array = malloc(final_array_size * sizeof *line1_array);
  vector line1_start_point = { 200, 200 };
  vector line1_end_point = { 800, 200 };

  koch_line *line2_array = malloc(final_array_size * sizeof *line2_array);
  vector line2_start_point = { 200, 200 };
  vector line2_end_point = { 500, 200 + 600 * 0.5 * 1.732 }; // height of triangle

  koch_line *line3_array = malloc(final_array_size * sizeof *line3_array);
  vector line3_start_point = { 500, 200 + 600 * 0.5 * 1.732 };
  vector line3_end_point = { 800, 200 };

  calculate_koch_curve(line1_array, line1_start_point, line1_end_point);
  calculate_koch_curve(line2_array, line2_start_point, line2_end_point);
  calculate_koch_curve(line3_array, line3_start_point, line3_end_point);

  SDL_Event event;
  if (init_sdl_and_window()){
    while(main_window._gwindow_open) {
      while(SDL_PollEvent(&event) != 0) {
        if(event.type == SDL_QUIT )  {
          main_window._gwindow_open = false;
        }
      }

      // change background color from here
      SDL_SetRenderDrawColor(main_window._grenderer, 255, 255, 
          255, 255);
      SDL_RenderClear(main_window._grenderer);

      // change koch curve color from here
      SDL_SetRenderDrawColor(main_window._grenderer, 0, 0, 
          0, 255);

      for (int i = 0; i < pow(4, MAX_ITER); i++){
        SDL_RenderDrawLine(main_window._grenderer, 
            line1_array[i].start.x, line1_array[i].start.y,
            line1_array[i].end.x, line1_array[i].end.y);
        SDL_RenderDrawLine(main_window._grenderer, 
            line2_array[i].start.x, line2_array[i].start.y,
            line2_array[i].end.x, line2_array[i].end.y);
        SDL_RenderDrawLine(main_window._grenderer, 
            line3_array[i].start.x, line3_array[i].start.y,
            line3_array[i].end.x, line3_array[i].end.y);
      }

      SDL_RenderPresent(main_window._grenderer);
    }
  }
  clean_up_and_close();
  free(line1_array);
  free(line2_array);
  free(line3_array);

  return 0;
}

#include "noise.h"
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define PI 3.141592654

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

// width and height of screen
#define WIDTH 900
#define HEIGHT 700
// number of particles
#define PNUM 1000
// area in which the aligning vector will act
// has to be a factor of both width and height
#define SIZE 10

bool init() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    return false;
  window = SDL_CreateWindow("Flow Field", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT,
                            SDL_WINDOW_SHOWN);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
  return renderer && window;
}

void endAll() {
  // Deallocate renderer
  SDL_DestroyRenderer(renderer);
  renderer = NULL;
  // Destroy window
  SDL_DestroyWindow(window);
  window = NULL;
  // Quit SDL subsystems
  SDL_Quit();
}

int main(int argc, char **args) {
  // aligning vectors based on perlin noise
  vector2 alignVectors[WIDTH / SIZE][HEIGHT / SIZE];
  float angle;
  for (int x = 0; x < WIDTH / SIZE; x++) {
    for (int y = 0; y < HEIGHT / SIZE; y++) {
      angle = perlin(x / (float)SIZE, y / (float)SIZE) * PI;
      alignVectors[x][y].x = cos(angle);
      alignVectors[x][y].y = sin(angle);
    }
  }

  // create particles
  vector2 particles[PNUM];
  for (int i = 0; i < PNUM; i++) {
    particles[i].x = rand() % WIDTH;
    particles[i].y = rand() % HEIGHT;
  }
  vector2 pos;
  SDL_Rect particle;
  particle.w = 2;
  particle.h = 2;

  if (init()) {
    bool quit = false;
    SDL_Event e;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_Rect rects[PNUM],
        screenFiller = {.x = 0, .y = 0, .w = WIDTH, .h = HEIGHT};

    while (!quit) {
      while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT)
          quit = true;
      }

      for (int i = 0; i < PNUM; i++) {
        // display the particles
        particle.x = particles[i].x;
        particle.y = particles[i].y;
        rects[i] = particle;

        // align the particles
        pos.x = particles[i].x / SIZE;
        pos.y = particles[i].y / SIZE;
        particles[i].x += alignVectors[(int)pos.x][(int)pos.y].x;
        particles[i].y += alignVectors[(int)pos.x][(int)pos.y].y;

        // if particles are out of bound give them a random position inside
        // the bounds
        if (particles[i].x >= WIDTH || particles[i].x < 0 ||
            particles[i].y >= HEIGHT || particles[i].y < 0) {
          particles[i].x = rand() % WIDTH;
          particles[i].y = rand() % HEIGHT;
        }
      }
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 5);
      SDL_RenderFillRect(renderer, &screenFiller);
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      SDL_RenderFillRects(renderer, rects, PNUM);

      SDL_RenderPresent(renderer);
      SDL_Delay(5);
    }
  }
  endAll();
  return 0;
}

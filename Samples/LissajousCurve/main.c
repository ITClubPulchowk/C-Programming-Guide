#include <math.h>
#include <SDL2/SDL.h>

SDL_Window *window;
SDL_Renderer *renderer;

#define TAU 2 * 3.141592654
#define RADIUS 50
#define GAP 40

typedef struct {
    int x, y;
    int a, b;
    float r;
    int prev_x, prev_y;
}Driver;

typedef struct {
    int x, y;
    float a, b;
    int prev_x, prev_y;
}Curve;

void DrawCircle(Driver d){
    int prev_x = d.x + RADIUS, prev_y = d.y;
    for (float i = 0; i <= 1; i += 0.01){
        SDL_RenderDrawLine(renderer, d.x + cos(i * TAU) * RADIUS, d.y + sin(i * TAU) * RADIUS, prev_x, prev_y);
        prev_x = d.x + cos(i * TAU) * RADIUS;
        prev_y = d.y + sin(i * TAU) * RADIUS;
    }
}

int main(){
    if (SDL_Init(SDL_INIT_VIDEO)){
        printf("Could Not Initialize SDL\n");
        return (-1);
    }

    window = SDL_CreateWindow("Lissajous Curve", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 700, 700, SDL_WINDOW_SHOWN);
    if (!window){
        printf("Could Not create Window\n");
        return (-2);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer){
        printf("Could Not create Renderer\n");
        return (-3);
    }

    Driver driver_row [4];
    Driver driver_col [4];

    for (int i = 0; i < 4; i ++){
        driver_row[i].y = RADIUS + GAP / 2;
        driver_row[i].x = 2 * RADIUS + 2.5 * GAP + i * (RADIUS * 2 + GAP);
        driver_row[i].r = i + 1;
        driver_row[i].prev_x = driver_row[i].x + RADIUS;
        driver_row[i].prev_y = driver_row[i].y;

        driver_col[i].x = RADIUS + GAP / 2;
        driver_col[i].y = 2 * RADIUS + 2.5 * GAP + i * (RADIUS * 2 + GAP);
        driver_col[i].r = i + 1;
        driver_col[i].prev_x = driver_col[i].x + RADIUS;
        driver_col[i].prev_y = driver_col[i].y;
    }

    Curve curves[4][4];
    for (int i = 0; i < 4; i ++){
        for (int j = 0; j < 4; j ++){
            curves[i][j].x = driver_row[i].x;
            curves[i][j].y = driver_col[j].y;
            curves[i][j].prev_x = driver_row[i].x + RADIUS;
            curves[i][j].prev_y = driver_col[j].y;
            curves[i][j].a = driver_row[i].r;
            curves[i][j].b = driver_col[j].r;
        }
    }

    float t = 0;

    SDL_Event e;
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET, 700, 700);
    SDL_SetRenderTarget(renderer, NULL);
    while (1){
        while (SDL_PollEvent(&e))
            if (e.type == SDL_QUIT) return 0;

        t += 0.01;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, texture, NULL, NULL);

        SDL_SetRenderDrawColor(renderer, 200, 120, 255, 255);
        for (int i = 0; i < 4; i ++){
            driver_row[i].a = driver_row[i].x + RADIUS * cos(driver_row[i].r * t * TAU);
            driver_row[i].b = driver_row[i].y + RADIUS * sin(driver_row[i].r * t * TAU);
            SDL_RenderFillRect(renderer,&(SDL_Rect){.x = driver_row[i].a - 5, .y = driver_row[i].b - 5, .w = 10, .h = 10});

            driver_col[i].a = driver_col[i].x + RADIUS * cos(driver_col[i].r * t * TAU);
            driver_col[i].b = driver_col[i].y + RADIUS * sin(driver_col[i].r * t * TAU);
            SDL_RenderFillRect(renderer,&(SDL_Rect){.x = driver_col[i].a - 5, .y = driver_col[i].b - 5, .w = 10, .h = 10});
        }

        SDL_SetRenderTarget(renderer, texture);
            if (t > 1) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderClear(renderer);
                t = 0;
            }

            for (int i = 0; i < 4; i ++){
                SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
                for (int j = 0; j < 4; j ++){
                    int tmp_x = curves[i][j].x + cos(curves[i][j].a * t * TAU) * RADIUS;
                    int tmp_y = curves[i][j].y + sin(curves[i][j].b * t * TAU) * RADIUS;
                    SDL_RenderDrawLine(renderer, tmp_x, tmp_y, curves[i][j].prev_x, curves[i][j].prev_y);
                    curves[i][j].prev_x = tmp_x;
                    curves[i][j].prev_y = tmp_y;
                }
                SDL_SetRenderDrawColor(renderer, 100, 180, 255, 255);
                SDL_RenderDrawLine(renderer, driver_row[i].a, driver_row[i].b, driver_row[i].prev_x, driver_row[i].prev_y);
                driver_row[i].prev_x = driver_row[i].a;
                driver_row[i].prev_y = driver_row[i].b;
                SDL_RenderDrawLine(renderer, driver_col[i].a, driver_col[i].b, driver_col[i].prev_x, driver_col[i].prev_y);
                driver_col[i].prev_x = driver_col[i].a;
                driver_col[i].prev_y = driver_col[i].b;
            }
        SDL_SetRenderTarget(renderer, NULL);

        SDL_RenderPresent(renderer);
        SDL_Delay(30);
    }
}

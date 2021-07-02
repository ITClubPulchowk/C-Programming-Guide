#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <math.h>

#define PI 3.1415

SDL_Window* window = NULL; //The main window
SDL_Renderer* renderer = NULL; //The renderer associated with the main window

const int screenwidth = 720;
const int screenheight = 520;

typedef struct {
    int x;
    int y;
    int r;
} circle;

typedef struct {
    int x1;
    int y1;
    int x2;
    int y2;
} line;

bool Init(){
    bool success = true;
    if(SDL_Init(SDL_INIT_VIDEO) < 0 ){                                   //SDL Initialization
        printf("SDL did not initialize. %s\n", SDL_GetError());
        success = false;
    }
    else{
        if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!\n" );
		}
        window = SDL_CreateWindow("Simulation", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenwidth, screenheight, SDL_WINDOW_SHOWN );
        if (window == NULL){
            printf("Failed to create the window. %s\n", SDL_GetError());
            success = false;
        }
        else{
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
            if(renderer == NULL){
                printf("Failed to load the renderer\n");
                success = false;
            }     
            
        }
    }
    return success;
}

//Mid-point circle algorithm to draw the circles
void DrawCircle(SDL_Renderer * renderer, int xCenter, int yCenter, int radius)
{
   const int diameter = (radius * 2);

   int x = (radius - 1);
   int y = 0;
   int tx = 1;
   int ty = 1;
   int error = (tx - diameter);

   while (x >= y)
   {
      //  Each of the following renders an octant of the circle
      SDL_RenderDrawPoint(renderer, xCenter + x, yCenter - y);
      SDL_RenderDrawPoint(renderer, xCenter + x, yCenter + y);
      SDL_RenderDrawPoint(renderer, xCenter - x, yCenter - y);
      SDL_RenderDrawPoint(renderer, xCenter - x, yCenter + y);
      SDL_RenderDrawPoint(renderer, xCenter + y, yCenter - x);
      SDL_RenderDrawPoint(renderer, xCenter + y, yCenter + x);
      SDL_RenderDrawPoint(renderer, xCenter - y, yCenter - x);
      SDL_RenderDrawPoint(renderer, xCenter - y, yCenter + x);

      if (error <= 0)
      {
         y++;
         error += ty;
         ty += 2;
      }

      if (error > 0)
      {
         x--;
         tx += 2;
         error += (tx - diameter);
      }
   }
}

void SDL_RenderFillCircle(SDL_Renderer * renderer, int x, int y, int radius)
{
    int offsetx, offsety, d;

    offsetx = 0;
    offsety = radius;
    d = radius -1;

    while (offsety >= offsetx) {

        SDL_RenderDrawLine(renderer, x - offsety, y + offsetx,
                                     x + offsety, y + offsetx);
        SDL_RenderDrawLine(renderer, x - offsetx, y + offsety,
                                     x + offsetx, y + offsety);
        SDL_RenderDrawLine(renderer, x - offsetx, y - offsety,
                                     x + offsetx, y - offsety);
        SDL_RenderDrawLine(renderer, x - offsety, y - offsetx,
                                     x + offsety, y - offsetx);

        if (d >= 2*offsetx) {
            d -= 2*offsetx + 1;
            offsetx +=1;
        }
        else if (d < 2 * (radius - offsety)) {
            d += 2 * offsety - 1;
            offsety -= 1;
        }
        else {
            d += 2 * (offsety - offsetx - 1);
            offsety -= 1;
            offsetx += 1;
        }
    }

}


circle update(int n, float angle){
    circle new;

    new.x=screenwidth/4;
    new.y=screenheight/2;
    
    if(n>0){
        for (int i=1; i<=n; i++){
            new.x += 60*(4/((2*i-1)*PI ))*cos((2*i-1)*angle);
            new.y += 60*(4/((2*i-1)*PI ))*sin((2*i-1)*angle);        
        }
    }
    new.r = 60*4/(PI*(2*(n+1)-1));
    return new;
}


void close()
{

	//Deallocate renderer
	SDL_DestroyRenderer(renderer);
    renderer = NULL;

	//Destroy window
	SDL_DestroyWindow( window );
	window = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

int main(int argc, char** args){

    if (!Init()){
        printf("Failed to iniatialize\n");

    }
    
    else{
        bool quit = false;
        SDL_Event e;

        float angle;
        float time=0;

        circle circles[10]; 
        
        //Line connecting the tip to the graph
        line connection;       

        //The graph is just a bunch of lines. There must be a better way to do this
        line graph[1000];
        for (int i=0; i<1000; i++){
            graph[i].x1 = screenwidth/4 + 200 + i*2;
            graph[i].x2 = screenwidth/4 + 200 + (i+1)*2;
            graph[i].y1 = screenheight/2;
            graph[i].y2 = screenheight/2;
        }

            
        while(!quit){                            //Main loop
            while(SDL_PollEvent(&e) != 0){       //Event loop
                if(e.type == SDL_QUIT){          
                    printf("Quitted\n");
                    quit = true;
                }
                    
            }

            angle = 2*PI*time;

            SDL_RenderClear(renderer);           //Clear the screen

            //Axes Lines
            SDL_SetRenderDrawColor(renderer, 76, 234, 200, 255);
            SDL_RenderDrawLine(renderer, screenwidth/2, screenheight/2-200, screenwidth/2, screenheight/2 +200);
            SDL_RenderDrawLine(renderer, screenwidth/2, screenheight/2, screenwidth, screenheight/2); 

            for (int i=0; i<10; i++){
                circles[i] = update(i, angle);
                
                //Draw Circle
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                DrawCircle(renderer, circles[i].x, circles[i].y, circles[i].r);

                //Fill circle
                SDL_SetRenderDrawColor(renderer, (i+1)*20, (i+1)*20, (i+1)*25, 100);
                SDL_RenderFillCircle(renderer, circles[i].x, circles[i].y, circles[i].r);
                if(i>0){
                    // Draw Radial Line
                    SDL_SetRenderDrawColor(renderer, 0, 200, 50, 100);
                    SDL_RenderDrawLine(renderer, circles[i-1].x, circles[i-1].y, circles[i].x, circles[i].y);
                }
            }


            //Line connecting the tip to the graph    
            connection.x1 = circles[7].x;
            connection.y1 = circles[7].y;
            connection.x2 = screenwidth/4+200;
            connection.y2 = circles[7].y;

            SDL_SetRenderDrawColor(renderer, 23, 234, 200,255);
            SDL_RenderDrawLine(renderer, connection.x1, connection.y1, connection.x2, connection.y2);

            //Drawing the graph. Again, there must be a better way to do this
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            for (int i=0; i<999; i++){
                SDL_RenderDrawLine(renderer, graph[i].x1, graph[i].y1, graph[i].x2, graph[i].y2);
            }

            graph[0].x1 = connection.x2;
            graph[0].y1 = connection.y2;


            //Shift the graph to the right    
            for (int i=999; i>0; i--){
                graph[i].y2 = graph[i].y1;                
                graph[i].y1 = graph[i-1].y2;                
            }
            graph[0].y2 = graph[0].y1;            

            SDL_SetRenderDrawColor(renderer, 0x30, 0x30, 0x30, 0xFF);         //Set color with which the screen is cleared            
            SDL_RenderPresent(renderer);
            
            time+=0.005;                //arbitrary choice that produces the best result
            _sleep(10);
            
        }
    }
    close();
    
    return 0;
}


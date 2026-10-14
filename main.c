#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>

#include <stdio.h>
#include <sys/types.h>
#include <wchar.h>
#include <math.h>


#define TITLE "uwu"
#define WIDTH 720
#define HEIGHT 720
#define CELL_SIZE 30.0
#define MAX_RAY_DISTANCE 720.0f

#define INITIAL_POSITION_X 0
#define INITIAL_POSITION_Y 0

#define MAP_WIDTH 24
#define MAP_HEIGHT 24

void drawMap(SDL_Renderer* renderer);

int worldMap[MAP_WIDTH][MAP_HEIGHT] =
{
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,1,0,1,0,1,0,0,0,1},
  {1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,1,0,0,0,1},
  {1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,1,1,0,1,1,0,0,0,0,1,0,1,0,1,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

int main(void) {

    // SDL Initializtion Check 
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL Initialization Failed");
        return 1;
    }
    
    //SDL Window Creation with NULL check after creation
    SDL_Window* sdlWindow = SDL_CreateWindow(
        TITLE, 
        INITIAL_POSITION_X, INITIAL_POSITION_Y, 
        WIDTH, HEIGHT, 
        SDL_WINDOW_SHOWN
    );
    if(sdlWindow == NULL) {
        printf("SDL Window Instance is Null: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    //SDL Renderer Creation with NULL check after creation
    SDL_Renderer* sdlRenderer = SDL_CreateRenderer(
        sdlWindow, 
        -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if(sdlRenderer == NULL) {
        printf("SDL Renderer Instance is Null: %s", SDL_GetError());
        SDL_DestroyRenderer(sdlRenderer);
        SDL_Quit();
        return 1;
    }

    //Set the Background to black then reset the renderer
    SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
    SDL_RenderClear(sdlRenderer);


    // Rendering Loop or Game Loop 
    int isRunning = 1;
    SDL_Event sdlEvent;

    float player_pos_x = 30.0;
    float player_pos_y = 30.0;
    float player_direction = 90.0;


    //Game Loop
    while (isRunning) {

 //       printf("X: %i | Y: %i\n", currentPlayerXCell, currentPlayerYCell);


        while (SDL_PollEvent(&sdlEvent)) {

            switch (sdlEvent.type) {
                case SDL_QUIT:
                    isRunning = 0;    
                    break;
                
                //Keydown Events
                case SDL_KEYDOWN: {

                    int playerCurrentCellX = player_pos_x / 30;
                    int playerCurrentCellY = player_pos_y / 30;
                
                    switch (sdlEvent.key.keysym.sym) {
                        case SDLK_w:
                            if(worldMap[playerCurrentCellY- 1][playerCurrentCellX] == 1) {
                                break;
                            }
                            player_pos_y -= 30;
                            break;

                        case SDLK_s:
                            if(worldMap[playerCurrentCellY+ 1][playerCurrentCellX] == 1) {
                                break;
                            }
                            player_pos_y += 30;
                            break;

                        case SDLK_a:
                            if(worldMap[playerCurrentCellY][playerCurrentCellX- 1] == 1) {
                                break;
                            }
                            player_pos_x -= 30;
                            break;

                        case SDLK_d:
                            if(worldMap[playerCurrentCellY][playerCurrentCellX + 1] == 1) {
                                break;
                            }
                            player_pos_x += 30;
                            break;
                        
                        case SDLK_q:
                            player_direction -= 30;
                            if(player_direction < 0) {
                                player_direction = player_direction + 360.0;
                            }
                            break;

                        case SDLK_e:
                            player_direction += 30;
                            if(player_direction >= 360) {
                                player_direction = player_direction - 360;
                            }
                            break;

                        case SDLK_BACKSPACE:
                            isRunning = 0;
                            break;
                    }
                }
            }
        }
        
        SDL_RenderClear(sdlRenderer);

        drawMap(sdlRenderer);
        
        //PLayer Rectangle Initialization
        //Render the updated player rectangle and display it
        SDL_Rect player = {
            player_pos_x,
            player_pos_y,
            30,
            30
        };
        SDL_SetRenderDrawColor(sdlRenderer, 255, 0, 0, 255);
        SDL_RenderFillRect(sdlRenderer, &player);
        
        float playerCenterX = player_pos_x + CELL_SIZE / 2.0;
        float playerCenterY = player_pos_y + CELL_SIZE / 2.0;

        for (int i = -45; i <= 45; i++) {
            //this is the radian
            float angle = (player_direction + i) * (M_PI / 180.0);
            
            //these vars check direction or x and y
            float dirX = cos(angle);
            float dirY = sin(angle);

            //cell markers
            int mapX = playerCenterX / CELL_SIZE;
            int mapY = playerCenterY / CELL_SIZE;

            //determine which way the markers go
            int stepX = dirX > 0 ? 1 : -1;
            int stepY = dirY > 0 ? 1 : -1;

            float nextVerticalLineX   = (dirX > 0) ? (mapX + 1) * CELL_SIZE : mapX * CELL_SIZE;
            float nextHorizontalLineY = (dirY > 0) ? (mapY + 1) * CELL_SIZE : mapY * CELL_SIZE;

            float distToVerticalLine   = fabs((nextVerticalLineX - playerCenterX) / dirX);
            float distToHorizontalLine = fabs((nextHorizontalLineY - playerCenterY) / dirY);

            float tileCostX = fabs(CELL_SIZE / dirX);
            float tileCostY = fabs(CELL_SIZE / dirY);

            float rayLength = MAX_RAY_DISTANCE;
            int hit = 0;

            while (!hit) {
                float crossedAt;

                if (distToVerticalLine < distToHorizontalLine) {
                    crossedAt = distToVerticalLine;
                    if (crossedAt > MAX_RAY_DISTANCE) break;
                    mapX += stepX;
                    distToVerticalLine += tileCostX;
                } else {
                    crossedAt = distToHorizontalLine;
                    if (crossedAt > MAX_RAY_DISTANCE) break;
                    mapY += stepY;
                    distToHorizontalLine += tileCostY;
                }

                if (mapX < 0 || mapX >= MAP_WIDTH || mapY < 0 || mapY >= MAP_HEIGHT) break;

                if (worldMap[mapY][mapX] == 1) {
                    hit = 1;
                    rayLength = crossedAt;
                }
            }

            SDL_SetRenderDrawColor(sdlRenderer, 0, 255, 0, 255);
            SDL_RenderDrawLine(
                sdlRenderer,
                playerCenterX,
                playerCenterY,
                playerCenterX + rayLength * dirX,
                playerCenterY + rayLength * dirY
            );
        }
        SDL_RenderPresent(sdlRenderer);

    }

    //Clean Up
    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();
    
    return 0;
}

void drawMap(SDL_Renderer* renderer) {
    

    //Render the map by looping through the worldMap Array
    for(int row = 0; row < MAP_HEIGHT; row++) {
        for (int col = 0; col < MAP_WIDTH; col++) {
   
            SDL_Rect wall = {
                30 * col,
                30 * row,
                30,
                30
            };        

            //printf("Row:%i Column:%i \n",row, col);
            
            if(worldMap[row][col] == 1) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            } else {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            }

            SDL_RenderFillRect(renderer, &wall);
        }
    }

    // for (int i = 0; i <=720; i += 30) {
    //     SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    //     SDL_RenderDrawLine(renderer, 0, i, 720, i);
    //     SDL_RenderDrawLine(renderer, i, 0, i, 720);
    // }
}

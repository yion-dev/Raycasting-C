#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_shape.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>

#include <stdio.h>
#include <sys/types.h>
#include <wchar.h>
#include <math.h>

#define TITLE "uwu"
#define WIDTH 840
#define HEIGHT 840
#define CELL_SIZE 35.0
#define MAX_RAY_DISTANCE 1000.0f

#define PLAYER_FOV 60
#define PLAYER_WIDTH 5.0
#define PLAYER_HEIGHT 5.0
#define PLAYER_SPEED 120.0f      
#define PLAYER_TURN_SPEED 120.0f

#define COLLISION_MARGIN 8.0f

#define INITIAL_POSITION_X 0
#define INITIAL_POSITION_Y 0

#define MAP_WIDTH 24
#define MAP_HEIGHT 24

struct Ray {
    float rayLength;
    int side;
    int hit;
};

void draw_map(SDL_Renderer* renderer);
void draw_map_3d(SDL_Renderer* renderer);
void draw_player(SDL_Renderer* renderer, float player_pos_x, float player_pos_y);
void draw_player_direction(SDL_Renderer* renderer, float playerCenterX, float playerCenterY, float player_direction);
struct Ray get_ray_length(float playerCenterX, float playerCenterY, float angle);
int is_wall(float x, float y);
int can_stand_at(float centerX, float centerY);
void move_player(float* player_pos_x, float* player_pos_y, float player_direction, float forward, float strafe);

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

    float player_pos_x = 35.0;
    float player_pos_y = 35.0;
    float player_direction = 90.0;


    Uint32 lastTicks = SDL_GetTicks();

    //Game Loop
    while (isRunning) {

        //time since the last frame, so speed is the same on any machine
        Uint32 nowTicks = SDL_GetTicks();
        float dt = (nowTicks - lastTicks) / 1000.0f;
        lastTicks = nowTicks;
        if (dt > 0.05f) dt = 0.05f;

 //       printf("X: %i | Y: %i\n", currentPlayerXCell, currentPlayerYCell);


        while (SDL_PollEvent(&sdlEvent)) {

            switch (sdlEvent.type) {
                case SDL_QUIT:
                    isRunning = 0;    
                    break;
                
                //Keydown Events (only for one-shot keys)
                case SDL_KEYDOWN:
                    if (sdlEvent.key.keysym.sym == SDLK_BACKSPACE ||
                        sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
                        isRunning = 0;
                    }
                    break;
            }
        }

        //held keys are read every frame, so movement is smooth
        const Uint8* keys = SDL_GetKeyboardState(NULL);

        if (keys[SDL_SCANCODE_Q]) player_direction -= PLAYER_TURN_SPEED * dt;
        if (keys[SDL_SCANCODE_E]) player_direction += PLAYER_TURN_SPEED * dt;
        while (player_direction < 0)     player_direction += 360.0;
        while (player_direction >= 360)  player_direction -= 360.0;

        float forward = 0.0f, strafe = 0.0f;
        if (keys[SDL_SCANCODE_W]) forward += PLAYER_SPEED * dt;
        if (keys[SDL_SCANCODE_S]) forward -= PLAYER_SPEED * dt;
        if (keys[SDL_SCANCODE_D]) strafe  += PLAYER_SPEED * dt;
        if (keys[SDL_SCANCODE_A]) strafe  -= PLAYER_SPEED * dt;

        move_player(&player_pos_x, &player_pos_y, player_direction, forward, strafe);

        SDL_RenderClear(sdlRenderer);

        draw_map_3d(sdlRenderer);
        
        float playerCenterX = player_pos_x + CELL_SIZE / 2.0;
        float playerCenterY = player_pos_y + CELL_SIZE / 2.0;


        // one ray per screen column
        for (int x = 0; x < WIDTH; x++) {
            // 1. angle of this column's ray (degrees, then radians)
            float rayDeg = (player_direction - PLAYER_FOV / 2.0) + ((float)x / WIDTH) * PLAYER_FOV;
            float rayAngle = rayDeg * (M_PI / 180.0);

            // 2. cast it
            struct Ray ray = get_ray_length(playerCenterX, playerCenterY, rayAngle);
            if (!ray.hit) continue;

            // 3. fisheye fix: use the angle between this ray and the view center
            float diff = (rayDeg - player_direction) * (M_PI / 180.0);
            float corrected = ray.rayLength * cos(diff);

            // 4. distance -> wall height
            float wallHeight = (CELL_SIZE * HEIGHT) / corrected;
            if (wallHeight > HEIGHT) wallHeight = HEIGHT;

            // 5. shade by side, then draw one vertical strip centered on the screen
            float shade = 1.0f - corrected / MAX_RAY_DISTANCE;
            if(shade < 0.25f) shade = 0.25f;

            int base = (ray.side == 0) ? 255 : 180;
            int color = base * shade;
            SDL_SetRenderDrawColor(sdlRenderer, color, color, color, 255);
            int top = (HEIGHT - wallHeight) / 2;
            SDL_RenderDrawLine(sdlRenderer, x, top, x, top + wallHeight);
        }

        draw_map(sdlRenderer);
        draw_player(sdlRenderer, player_pos_x, player_pos_y);
        draw_player_direction(sdlRenderer, playerCenterX, playerCenterY, player_direction);

        SDL_RenderPresent(sdlRenderer);
    }

    //Clean Up
    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();
    
    return 0;
}

void draw_player(
    SDL_Renderer* renderer,
    float player_pos_x,
    float player_pos_y
) {
    //PLayer Rectangle Initialization
    //Render the updated player rectangle and display it
    SDL_Rect player = {
        player_pos_x / 7,
        player_pos_y / 7,
        PLAYER_WIDTH,
        PLAYER_HEIGHT
    };
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &player);
}

//draws the fan of rays from the player's center (top-down debug view)
void draw_player_direction(
    SDL_Renderer* renderer,
    float playerCenterX,
    float playerCenterY,
    float player_direction
) {
    for (int i = -20; i <= 20; i++) {
        //this is the radian
        float angle = (player_direction + i) * (M_PI / 180.0);

        //these vars check direction of x and y
        float dirX = cos(angle);
        float dirY = sin(angle);

        struct Ray ray = get_ray_length(playerCenterX , playerCenterY , angle);

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderDrawLine(
            renderer,
            playerCenterX / 7,
            playerCenterY / 7,
            (playerCenterX + ray.rayLength * dirX) / 7,
            (playerCenterY + ray.rayLength * dirY) / 7
        );
    }
}

void draw_map(SDL_Renderer* renderer) {

    
    //Render the map by looping through the worldMap Array
    for(int row = 0; row < MAP_HEIGHT; row++) {
        for (int col = 0; col < MAP_WIDTH; col++) {
   
            SDL_Rect wall = {
                5 * col,
                5 * row,
                5,
                5
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

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawLine(renderer, 0, 0, WIDTH / 7, 0);
    SDL_RenderDrawLine(renderer, 0, 0, 0, HEIGHT / 7);
    SDL_RenderDrawLine(renderer, 0, HEIGHT / 7, WIDTH / 7, HEIGHT / 7);
    SDL_RenderDrawLine(renderer, WIDTH / 7, 0, WIDTH / 7, HEIGHT / 7);
}

struct Ray get_ray_length(
    float playerCenterX,
    float playerCenterY,
    float angle
) {

    //cell markers
    int mapX = playerCenterX / CELL_SIZE;
    int mapY = playerCenterY / CELL_SIZE;

    //determine which way the markers go
    float dirX = cos(angle);
    float dirY = sin(angle);

    int stepX = dirX > 0 ? 1 : -1;
    int stepY = dirY > 0 ? 1 : -1;

    float nextVerticalLineX   = (dirX > 0) ? (mapX + 1) * CELL_SIZE : mapX * CELL_SIZE;
    float nextHorizontalLineY = (dirY > 0) ? (mapY + 1) * CELL_SIZE : mapY * CELL_SIZE;

    //absolute values of distance between the player center to the nearest vertical or horizontal line
    float distToVerticalLine   = fabs((nextVerticalLineX - playerCenterX) / dirX);
    float distToHorizontalLine = fabs((nextHorizontalLineY - playerCenterY) / dirY);

    float tileCostX = fabs(CELL_SIZE / dirX);
    float tileCostY = fabs(CELL_SIZE / dirY);

    float rayLength = MAX_RAY_DISTANCE;
    int hit = 0;
    int side = 0;

    //basically the loop calculate the ray length by using the vertical and horizontal lines of each tile 
    //by checking where the ray cross and whether the ray is closer to vertical or horizontal line
    //and set the ray length as long as it doesn't hit anything if it does it set the ray length to last crossed length and break
    while (!hit) {
        float crossedAt;

        if (distToVerticalLine < distToHorizontalLine) {
            side = 0;
            crossedAt = distToVerticalLine;
            if (crossedAt > MAX_RAY_DISTANCE) break;
            mapX += stepX;
            distToVerticalLine += tileCostX;
        } else {
            side = 1;
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

    //order matches the struct: rayLength, side, hit
    struct Ray ray = {
        rayLength,
        side,
        hit
    };

    return ray;
}

static int lerp_int(int a, int b, float t) {
    return a + (b - a) * t;
}

void draw_map_3d(SDL_Renderer* renderer) {
    int half = HEIGHT / 2;

    for (int y = 0; y < HEIGHT; y++) {
        int r, g, b;
        if (y < half) {
            float t = (float)y / half;                 // 0 at top, 1 at horizon
            r = lerp_int(20, 90, t);
            g = lerp_int(25, 100, t);
            b = lerp_int(55, 150, t);
        } else {
            float t = (float)(y - half) / half;        // 0 at horizon, 1 at bottom
            r = lerp_int(70, 20, t);
            g = lerp_int(55, 16, t);
            b = lerp_int(55, 18, t);
        }
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderDrawLine(renderer, 0, y, WIDTH, y);
    }
}

//true if the point is inside a wall cell (or outside the map)
int is_wall(float x, float y) {
    int cellX = x / CELL_SIZE;
    int cellY = y / CELL_SIZE;
    if (x < 0 || y < 0 || cellX >= MAP_WIDTH || cellY >= MAP_HEIGHT) return 1;
    return worldMap[cellY][cellX] == 1;
}

//the player is a small box around the center; all 4 corners must be free
int can_stand_at(float centerX, float centerY) {
    return !is_wall(centerX - COLLISION_MARGIN, centerY - COLLISION_MARGIN)
        && !is_wall(centerX + COLLISION_MARGIN, centerY - COLLISION_MARGIN)
        && !is_wall(centerX - COLLISION_MARGIN, centerY + COLLISION_MARGIN)
        && !is_wall(centerX + COLLISION_MARGIN, centerY + COLLISION_MARGIN);
}

//forward/strafe are distances in pixels for this frame
//x and y are tested separately so the player slides along walls
void move_player(
    float* player_pos_x,
    float* player_pos_y,
    float player_direction,
    float forward,
    float strafe
) {
    float radian = player_direction * (M_PI / 180.0);

    //forward is along the facing direction, strafe is 90 degrees to the right
    float moveX = forward * cos(radian) - strafe * sin(radian);
    float moveY = forward * sin(radian) + strafe * cos(radian);

    float centerX = *player_pos_x + CELL_SIZE / 2.0;
    float centerY = *player_pos_y + CELL_SIZE / 2.0;

    if (can_stand_at(centerX + moveX, centerY)) centerX += moveX;
    if (can_stand_at(centerX, centerY + moveY)) centerY += moveY;

    *player_pos_x = centerX - CELL_SIZE / 2.0;
    *player_pos_y = centerY - CELL_SIZE / 2.0;
}

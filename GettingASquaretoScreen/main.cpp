#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>

#define SCREEN_WIDTH 1280 
#define SCREEN_HEIGHT 720

const float MOVE_SPEED = 400.0f; // Move speed in pixels per second
const int FPS_CAP = 144;
const float FRAME_TIME = 1000.0f / FPS_CAP; // Time per frame in milliseconds

class Square {
    private:
        float x, y; // Use float for precision
        int size;
        SDL_Color color;
        SDL_Rect rect;
    public:
        Square(int x, int y, int size, SDL_Color color)
            : x(x), y(y), size(size), color(color) {}

        void render(SDL_Renderer* renderer) {
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            rect = {(int)x, (int)y, size, size}; // Cast to int for SDL_Rect
            SDL_RenderFillRect(renderer, &rect);
        }

        void move(float dx, float dy, float deltaTime) {
            x += dx * deltaTime;  // Movement based on delta time
            y += dy * deltaTime;

            //printf("Position: x = %d, y = %d\n", x, y);

            if (x < 0) x = 0;
            if (y < 0) y = 0;
            if (x + size > SCREEN_WIDTH) x = SCREEN_WIDTH - size;
            if (y + size > SCREEN_HEIGHT) y = SCREEN_HEIGHT - size;
        }
};

int main(int argc, char** argv) {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Error: SDL failed to initialize\nSDL Error: '%s'\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("SDL Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if(!window) {
        printf("Error: Failed to open window\nSDL Error: '%s'\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer) {
        printf("Error: Failed to create renderer\nSDL Error: '%s'\n", SDL_GetError());
        return 1;
    }

    SDL_Color red = {255, 0, 0, 255};
    Square square(500, 500, 50, red);

    bool running = true;

    Uint32 lastTick = SDL_GetTicks(); // Get the initial time
    Uint32 frameCount = 0;            // To count how many frames have been rendered
    Uint32 fpsTimer = lastTick;       // For timing FPS calculations
    float fps = 0.0f;                 // FPS to be calculated and printed

    while(running) {
        Uint32 frameStart = SDL_GetTicks();  // Record the start time of the frame

        SDL_Event event;
        
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Calculate time elapsed since last frame
        Uint32 currentTick = SDL_GetTicks();
        float deltaTime = (currentTick - lastTick) / 1000.0f; // Convert milliseconds to seconds
        lastTick = currentTick; // Update last tick

        // Clamp deltaTime to avoid large time steps
        if (deltaTime > 0.1f) deltaTime = 0.1f;

        // Get key states using SDL_GetKeyboardState
        const Uint8* keystates = SDL_GetKeyboardState(NULL);

        // Move the square based on key states
        float dx = 0.0f, dy = 0.0f;
        if (keystates[SDL_SCANCODE_UP]) dy = -1.0f;   // Move up
        if (keystates[SDL_SCANCODE_DOWN]) dy = 1.0f;  // Move down
        if (keystates[SDL_SCANCODE_LEFT]) dx = -1.0f; // Move left
        if (keystates[SDL_SCANCODE_RIGHT]) dx = 1.0f; // Move right

        // Normalize diagonal movement
        if (dx != 0 && dy != 0) {
            float length = sqrt(dx * dx + dy * dy);
            dx /= length;
            dy /= length;
        }

        square.move(dx * MOVE_SPEED, dy * MOVE_SPEED, deltaTime);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        square.render(renderer);

        SDL_RenderPresent(renderer);   

        // FPS Calculation
        frameCount++;
        Uint32 elapsed = currentTick - fpsTimer;
        if (elapsed >= 1000) {
            fps = frameCount / (elapsed / 1000.0f);
            printf("Frame count: Frame count = %d\n", frameCount);
            fpsTimer = currentTick;
            frameCount = 0;
        }

                // Cap the frame rate to 60 FPS
        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_TIME) {
            SDL_Delay(static_cast<Uint32>(FRAME_TIME - frameTime));  // Delay to achieve frame cap
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}






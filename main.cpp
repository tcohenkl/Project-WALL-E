#include <SDL2/SDL.h> 
#include "TrashBot.h"
#include <iostream>

enum CellType {WALL, ROAD, SIDEWALK};

const int GRID_WIDTH = 200;
const int GRID_HEIGHT = 200;

CellType grid[GRID_HEIGHT][GRID_WIDTH];

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << "\n";
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("SDL2 Window",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          1000, 1000,
                                          SDL_WINDOW_SHOWN);

    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool running = true;
    SDL_Event event;
    TrashBot bot(1, 0, 0);
    bot.reportPosition();

    bot.move(5, 3);
    bot.reportPosition();
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        bot.move(-5,0);
                        break;
                    case SDLK_RIGHT:
                        bot.move(5,0);
                        break;
                    case SDLK_UP:
                        bot.move(0,-5);
                        break;
                    case SDLK_DOWN:
                        bot.move(0,5);
                        break;
                    case SDLK_ESCAPE:
                        running = false;
                        break;
                    default:
                        break;
                }
            }
        }
        const int CELL_SIZE = 5;

        // Render Grid
        for (int y = 0; y < GRID_HEIGHT; ++y) {
            for (int x = 0; x < GRID_WIDTH; ++x) {
                SDL_Rect cellRect = { x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE };

                switch (grid[y][x]) {
                    case WALL:
                        SDL_SetRenderDrawColor(renderer, 70, 70, 255, 255); break;
                    case ROAD:
                        SDL_SetRenderDrawColor(renderer, 71, 71, 71, 255); break;
                    case SIDEWALK:
                        SDL_SetRenderDrawColor(renderer, 178, 178, 178, 255); break;
                }
                SDL_RenderFillRect(renderer, &cellRect);
            }
        }

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255); // dark gray
        SDL_RenderClear(renderer);

        // Draw red square at position (100, 150) with size 50x50
        SDL_Rect square = {bot.x, bot.y, 10, 10};
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red
        SDL_RenderFillRect(renderer, &square);

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

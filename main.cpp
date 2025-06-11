#include <SDL2/SDL.h> 
#include <SDL2/SDL_ttf.h>
#include "TrashBot.h"
#include <iostream>
#include <fstream>

// Pixel size of square window
const int WINDOWSIZE = 1000;

// Types of cells in background (not entities)
enum CellType {WALL, ROAD, SIDEWALK};

enum AppState {
    MENU,
    PICK_GRID,
    SIMULATION
};

///////////////////////
// Grid IO Functions //
///////////////////////

/**
 * @brief Converts a character to corresponding CellType enum
 *
 * Will return CellType::WALL by default
 *
 * @param c a character
 * ...
 * @return Returns corresponding CellType enum
 * @warning c should be defined
 */
CellType charToCellType(char c) {
    switch (c) {
        case '.': return SIDEWALK;
        case '#': return WALL;
        case '=': return ROAD;
        default: return WALL;
    }
}

/**
 * @brief Loads grid from a text file
 *
 * Will convert ANY text file into a grid, according to charToCellType conversion rules.
 *
 * @param filename is the string of the filename
 * ...
 * @return Returns a vector of vectors (grid) of CellTypes
 * @warning if file isn't found, returns an empty grid.
 */
std::vector<std::vector<CellType>> loadGridFromFile(const std::string& filename) {
    
    std::vector<std::vector<CellType>> grid;
    std::ifstream in(filename);
    
    // If no file, return empty grid
    if (!in) {
        return grid;
    }

    std::string line;

    while (std::getline(in, line)) {
        std::vector<CellType> row; // row of CellTypes
        for (int i = 0; i < line.size(); i++) {
            char c = line[i];
            CellType t = charToCellType(c);
            row.push_back(t);
        }
        grid.push_back(row);
    }
    return grid;
}

/**
 * @brief Generates a grid text file according to specifications
 *
 * @param filename the string of the filename to write to
 * @param housingWidth the horizontal width of housing blocks
 * @param sidewalkWidth the width of sidewalks
 * @param roadWidth the width of roads
 * @param blocksX the number of blocks horizontally
 * @param blocksX the number of blocks vertically
 * ...
 * @return `void`
 * @warning Will overwrite data if file exists.
 */
void generateGrid(const std::string& filename, int housingWidth, int sidewalkWidth, int roadWidth, int blocksX, int blocksY) {
    int blockUnitWidth = roadWidth + sidewalkWidth + housingWidth + sidewalkWidth;
    int width  = blocksX * blockUnitWidth + roadWidth;
    int height = blocksY * blockUnitWidth + roadWidth;

    std::ofstream out(filename);
    if (!out) {
        std::cerr << "Error opening file: " << filename << "\n";
        return;
    }

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int yMod = y % blockUnitWidth;
            int xMod = x % blockUnitWidth;

            if (yMod < roadWidth || xMod < roadWidth) {
                out << '=';  // road
            } else if (
                (yMod < roadWidth + sidewalkWidth || yMod >= roadWidth + sidewalkWidth + housingWidth) ||
                (xMod < roadWidth + sidewalkWidth || xMod >= roadWidth + sidewalkWidth + housingWidth)
            ) {
                out << '.';  // sidewalk
            } else {
                out << '#';  // housing/building
            }
        }
        out << '\n';
    }

    out.close();
}

/////////////////////////
// User Input Handlers //
/////////////////////////

/**
 * @brief Handles user input in the Menu screen
 *
 * Currently handles mouse clicks of buttons and the escape key to exit program.
 * 
 * @param event reference to SDL's event queue
 * @param state reference to the screen the game's on
 * @param running reference to the running variable
 * ...
 * @return `void`
 * 
 */
void handleMenuInput(SDL_Event& event, AppState& state, bool& running) {
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        int mx = event.button.x;
        int my = event.button.y;

        if (mx >= 300 && mx <= 500) {
            if (my >= 300 && my <= 360) {
                state = PICK_GRID;
            } else if (my >= 400 && my <= 460) {
                state = SIMULATION;
            }
        }
    }
     if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_ESCAPE: running = false; break;
        }
    }
}

/**
 * @brief Handles user input in the Grid Picker screen
 *
 * Currently handles the escape key to exit program.
 * 
 * @param event reference to SDL's event queue
 * @param state reference to the screen the game's on
 * ...
 * @return `void`
 * 
 */
void handleGridPickerInput(SDL_Event& event, AppState& state) {
     if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_ESCAPE: state = MENU; break;
        }
    }
}

/**
 * @brief Handles user input in the Simulation screen
 *
 * Currently handles the arrow keys for bot movement and escape key to return to menu.
 * 
 * @param event reference to SDL's event queue
 * @param bot reference to the bot object
 * @param running reference to whether the app is running
 * @param state reference to the screen the game's on
 * ...
 * @return `void`
 * 
 */
void handleSimulationInput(SDL_Event& event, TrashBot& bot, bool& running, AppState& state) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_LEFT:  bot.move(-10,  0); break;
            case SDLK_RIGHT: bot.move( 10,  0); break;
            case SDLK_UP:    bot.move(  0, -10); break;
            case SDLK_DOWN:  bot.move(  0,  10); break;
            case SDLK_ESCAPE: state = MENU; break;
        }
    }
}

////////////////////////////
// Screen Render Functions//
////////////////////////////

/**
 * @brief Helper function to render text on Menu screen
 * 
 * @param renderer reference to SDL's canvas
 * @param font reference to the font object
 * @param text string of text to be displayed
 * @param color 3-dimensional SDL color object 
 * @param rect 4-dimensional SDL_Rect object that specifies position and size
 * ...
 * @return `void`
 * 
 */
void renderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color, SDL_Rect rect) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    
    int textW, textH;
    SDL_QueryTexture(texture, NULL, NULL, &textW, &textH);
    SDL_Rect dst = {
        rect.x + (rect.w - textW) / 2,
        rect.y + (rect.h - textH) / 2,
        textW, textH
    };

    SDL_RenderCopy(renderer, texture, NULL, &dst);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

/**
 * @brief Render function for menu
 * 
 * @param renderer reference to SDL's canvas
 * @param font reference to the font object
 * ...
 * @return `void`
 * 
 */
void renderMenu(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_Color white = {255, 255, 255};
    SDL_SetRenderDrawColor(renderer, 40, 40, 60, 255); // dark bluish background
    SDL_RenderClear(renderer);

    // Draw title bar (just a colored rectangle)
    SDL_Rect titleBar = {(WINDOWSIZE - 400)/2, 100, 400, 60};
    SDL_SetRenderDrawColor(renderer, 80, 80, 120, 255);
    SDL_RenderFillRect(renderer, &titleBar);
    renderText(renderer, font, "WALLE SIMULATOR", white, titleBar);

    // Draw button 1: "Pick Grid"
    SDL_Rect button1 = {(WINDOWSIZE - 200)/2, 300, 200, 60};
    SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255); // blue
    SDL_RenderFillRect(renderer, &button1);

    // Draw button 2: "Run Simulation"
    SDL_Rect button2 = {(WINDOWSIZE - 200)/2, 400, 200, 60};
    SDL_SetRenderDrawColor(renderer, 0, 153, 0, 255); // green
    SDL_RenderFillRect(renderer, &button2);

    renderText(renderer, font, "Pick Grid", white, button1);
    renderText(renderer, font, "Run Simulation", white, button2);
}

/**
 * @brief Render function for simulation
 * 
 * @param renderer reference to SDL's canvas
 * @param grid reference to the grid object
 * @param bot reference to the bot object
 * @param cellSize pixel width of each cell
 * ...
 * @return `void`
 * 
 */
void renderSimulation(SDL_Renderer* renderer, const std::vector<std::vector<CellType>>& grid, const TrashBot& bot, int cellSize = 10) {
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255); // background
    SDL_RenderClear(renderer);

    size_t height = grid.size();
    size_t width  = height > 0 ? grid[0].size() : 0;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            SDL_Rect cellRect = { x * cellSize, y * cellSize, cellSize, cellSize };

            switch (grid[y][x]) {
                case WALL:
                    SDL_SetRenderDrawColor(renderer, 70, 70, 255, 255); break;
                case ROAD:
                    SDL_SetRenderDrawColor(renderer, 71, 71, 71, 255); break;
                case SIDEWALK:
                    SDL_SetRenderDrawColor(renderer, 178, 178, 178, 255); break;
                default:
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); break;
            }

            SDL_RenderFillRect(renderer, &cellRect);
        }
    }

    // Draw bot
    SDL_Rect botRect = { bot.x, bot.y, cellSize, cellSize };
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &botRect);
}

int main() {

    // SDL improper initialisation warnings
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << "\n";
        return 1;
    }

    if (TTF_Init() < 0) {
        std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << "\n";
        return 1;
    }

    AppState state = MENU; // App starts at the menu
    TTF_Font* font = TTF_OpenFont("./fonts/arial.ttf", 28); // Default font

    SDL_Window* window = SDL_CreateWindow("SDL2 Window",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WINDOWSIZE, WINDOWSIZE,
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
    SDL_Event event; // SDL's event Queue
    TrashBot bot(1, 0, 0);
    std::vector<std::vector<CellType>> grid = loadGridFromFile("./grid.txt");

    // Main Render Loop
    while (running) {
        SDL_PollEvent(&event);
        
        if (event.type == SDL_QUIT) running = false;

        switch (state) {
            case MENU:
                handleMenuInput(event, state, running);
                renderMenu(renderer, font);
                break;
            case PICK_GRID:
                handleGridPickerInput(event, state);
                // renderGridPicker(renderer);
                break;
            case SIMULATION:
                handleSimulationInput(event, bot, running, state);
                renderSimulation(renderer, grid, bot);
                break;
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60 FPS
    }

    // SDL Quitting Functions
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    TTF_CloseFont(font);
    TTF_Quit();
    return 0;
}
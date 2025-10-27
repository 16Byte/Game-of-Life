#include "raylib.h"
#include <vector>
#include <deque>
#include <cstdlib>
#include <ctime>
#include <cstdio>

const int GRID_WIDTH = 80;
const int GRID_HEIGHT = 60;
const int CELL_SIZE = 10;
const int SCREEN_WIDTH = GRID_WIDTH * CELL_SIZE;
const int SCREEN_HEIGHT = GRID_HEIGHT * CELL_SIZE;
const int MAX_HISTORY = 100; // Maximum number of states to keep in history

enum GameState {
    MAIN_MENU,
    SEED_MENU,
    SIMULATION
};

int countNeighbors(const std::vector<std::vector<bool>>& grid, int x, int y) {
    int count = 0;
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue;
            
            int nx = x + dx;
            int ny = y + dy;
            
            if (nx >= 0 && nx < GRID_WIDTH && ny >= 0 && ny < GRID_HEIGHT) {
                if (grid[ny][nx]) count++;
            }
        }
    }
    return count;
}

void updateGrid(std::vector<std::vector<bool>>& grid, std::vector<std::vector<bool>>& nextGrid) {
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            int neighbors = countNeighbors(grid, x, y);
            
            if (grid[y][x]) {
                nextGrid[y][x] = (neighbors == 2 || neighbors == 3);
            } else {
                nextGrid[y][x] = (neighbors == 3);
            }
        }
    }
    
    grid = nextGrid;
}

void randomSeed(std::vector<std::vector<bool>>& grid, float density = 0.3f) {
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            grid[y][x] = (GetRandomValue(0, 100) < (int)(density * 100));
        }
    }
}

void clearGrid(std::vector<std::vector<bool>>& grid) {
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            grid[y][x] = false;
        }
    }
}

bool drawButton(const char* text, int x, int y, int width, int height, bool isHovered) {
    Color buttonColor = isHovered ? DARKGRAY : GRAY;
    DrawRectangle(x, y, width, height, buttonColor);
    DrawRectangleLines(x, y, width, height, WHITE);
    
    int textWidth = MeasureText(text, 30);
    DrawText(text, x + (width - textWidth) / 2, y + (height - 30) / 2, 30, WHITE);
    
    return isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

bool isMouseOver(int x, int y, int width, int height) {
    Vector2 mousePos = GetMousePosition();
    return mousePos.x >= x && mousePos.x <= x + width &&
           mousePos.y >= y && mousePos.y <= y + height;
}

int main() {
    SetExitKey(0);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Conway's Game of Life");
    SetTargetFPS(60);
    
    srand(time(NULL));
    SetRandomSeed(time(NULL));
    
    std::vector<std::vector<bool>> grid(GRID_HEIGHT, std::vector<bool>(GRID_WIDTH, false));
    std::vector<std::vector<bool>> nextGrid(GRID_HEIGHT, std::vector<bool>(GRID_WIDTH, false));
    
    // History for back stepping
    std::deque<std::vector<std::vector<bool>>> history;
    
    GameState currentState = MAIN_MENU;
    bool paused = true;
    int frameCounter = 0;
    
    while (!WindowShouldClose()) {
        if (currentState == MAIN_MENU) {
            // Main menu
        }
        else if (currentState == SEED_MENU) {
            // Seed menu
        }
        else if (currentState == SIMULATION) {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                Vector2 mousePos = GetMousePosition();
                int x = mousePos.x / CELL_SIZE;
                int y = mousePos.y / CELL_SIZE;
                if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
                    grid[y][x] = true;
                }
            }
            
            if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
                Vector2 mousePos = GetMousePosition();
                int x = mousePos.x / CELL_SIZE;
                int y = mousePos.y / CELL_SIZE;
                if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
                    grid[y][x] = false;
                }
            }
            
            if (IsKeyPressed(KEY_SPACE)) {
                paused = !paused;
            }
            
            // Step forward one frame when paused
            if (paused && IsKeyPressed(KEY_RIGHT)) {
                // Save current state to history before stepping forward
                history.push_back(grid);
                if (history.size() > MAX_HISTORY) {
                    history.pop_front();
                }
                updateGrid(grid, nextGrid);
            }
            
            // Step backward one frame when paused
            if (paused && IsKeyPressed(KEY_LEFT)) {
                if (!history.empty()) {
                    grid = history.back();
                    history.pop_back();
                }
            }
            
            if (IsKeyPressed(KEY_C)) {
                clearGrid(grid);
                history.clear(); // Clear history when grid is cleared
            }
            
            if (IsKeyPressed(KEY_ESCAPE)) {
                currentState = MAIN_MENU;
                paused = true;
                clearGrid(grid);
                history.clear(); // Clear history when returning to menu
            }
            
            if (!paused) {
                frameCounter++;
                if (frameCounter >= 6) {
                    // Save current state to history before updating
                    history.push_back(grid);
                    if (history.size() > MAX_HISTORY) {
                        history.pop_front();
                    }
                    updateGrid(grid, nextGrid);
                    frameCounter = 0;
                }
            }
        }
        
        BeginDrawing();
        ClearBackground(BLACK);
        
        if (currentState == MAIN_MENU) {
            const char* title = "CONWAY'S GAME OF LIFE";
            int titleWidth = MeasureText(title, 40);
            DrawText(title, (SCREEN_WIDTH - titleWidth) / 2, 150, 40, WHITE);
            
            int buttonWidth = 300;
            int buttonHeight = 60;
            int buttonX = (SCREEN_WIDTH - buttonWidth) / 2;
            
            bool startHovered = isMouseOver(buttonX, 250, buttonWidth, buttonHeight);
            bool quitHovered = isMouseOver(buttonX, 330, buttonWidth, buttonHeight);
            
            if (drawButton("START", buttonX, 250, buttonWidth, buttonHeight, startHovered)) {
                currentState = SEED_MENU;
            }
            
            if (drawButton("QUIT", buttonX, 330, buttonWidth, buttonHeight, quitHovered)) {
                CloseWindow();
                return 0;
            }
        }
        else if (currentState == SEED_MENU) {
            const char* title = "SELECT INITIAL SEED";
            int titleWidth = MeasureText(title, 40);
            DrawText(title, (SCREEN_WIDTH - titleWidth) / 2, 100, 40, WHITE);
            
            int buttonWidth = 300;
            int buttonHeight = 60;
            int buttonX = (SCREEN_WIDTH - buttonWidth) / 2;
            
            bool blankHovered = isMouseOver(buttonX, 220, buttonWidth, buttonHeight);
            bool randomHovered = isMouseOver(buttonX, 300, buttonWidth, buttonHeight);
            
            if (drawButton("BLANK", buttonX, 220, buttonWidth, buttonHeight, blankHovered)) {
                clearGrid(grid);
                history.clear();
                currentState = SIMULATION;
                paused = false;
            }
            
            if (drawButton("RANDOM SEED", buttonX, 300, buttonWidth, buttonHeight, randomHovered)) {
                clearGrid(grid);
                randomSeed(grid);
                history.clear();
                currentState = SIMULATION;
                paused = false;
            }
            
            DrawText("Press ESC to go back", 10, SCREEN_HEIGHT - 30, 20, GRAY);
            if (IsKeyPressed(KEY_ESCAPE)) {
                currentState = MAIN_MENU;
            }
        }
        else if (currentState == SIMULATION) {
            for (int y = 0; y < GRID_HEIGHT; y++) {
                for (int x = 0; x < GRID_WIDTH; x++) {
                    if (grid[y][x]) {
                        DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE - 1, CELL_SIZE - 1, WHITE);
                    }
                }
            }
            
            const char* status = paused ? "PAUSED (SPACE: play | LEFT/RIGHT: step back/forward)" : "RUNNING (SPACE to pause)";
            DrawText(status, 10, 10, 20, GREEN);
            
            // Show history count when paused
            if (paused) {
                char historyText[32];
                snprintf(historyText, sizeof(historyText), "History: %zu states", history.size());
                DrawText(historyText, 10, 35, 16, LIGHTGRAY);
            }
            
            DrawText("Left Click: Draw | Right Click: Erase | C: Clear | ESC: Menu", 10, SCREEN_HEIGHT - 30, 20, GRAY);
        }
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}

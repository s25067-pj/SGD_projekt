#include <iostream>
#include <SDL.h>
#include <ctime>
#include <vector>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int CELL_SIZE = 20;
const int GRID_WIDTH = SCREEN_WIDTH / CELL_SIZE;
const int GRID_HEIGHT = SCREEN_HEIGHT / CELL_SIZE;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* appleTexture = NULL;
SDL_Texture* snakeTexture = NULL;

struct Segment {
    int x, y;
};

std::vector<Segment> snake;
int foodX, foodY;
enum Direction { UP, DOWN, LEFT, RIGHT };
Direction dir = UP;
bool accelerate = false;
double velocity = 2.0;
double accelerationRate = 0.5;

void drawRect(int x, int y, int w, int h, SDL_Color color) {
    SDL_Rect rect = { x, y, w, h };
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_RenderFillRect(renderer, &rect);
}

void drawTexture(SDL_Texture* texture, int x, int y, int w, int h) {
    SDL_Rect dstRect = { x * CELL_SIZE, y * CELL_SIZE, w, h };
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);
}

void drawGrid() {
    for (int i = 0; i < GRID_HEIGHT; i++) {
        for (int j = 0; j < GRID_WIDTH; j++) {
            if (i == 0 || j == 0 || i == GRID_HEIGHT - 1 || j == GRID_WIDTH - 1) {
                drawRect(j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE, { 128, 0, 32 });
            }
        }
    }
    for (int i = 0; i < snake.size(); i++) {
        if (i == 0) {
            // Rysowanie głowy węża
            drawRect(snake[i].x * CELL_SIZE, snake[i].y * CELL_SIZE, CELL_SIZE, CELL_SIZE, { 0, 255, 0 }); // Głowa
            // Rysowanie oczu
            int eyeSize = CELL_SIZE / 5;
            int eyeOffsetY = CELL_SIZE / 3;
            int eyeSpacing = CELL_SIZE / 4;
            int eyeOffsetXRight = CELL_SIZE / 2;
            int eyeOffsetXLeft = CELL_SIZE / 2 + eyeSpacing;
            drawRect(snake[i].x * CELL_SIZE + eyeOffsetXLeft, snake[i].y * CELL_SIZE + eyeOffsetY, eyeSize, eyeSize, { 0, 0, 0 }); // Lewe oko
            drawRect(snake[i].x * CELL_SIZE + CELL_SIZE - eyeOffsetXRight - eyeSize, snake[i].y * CELL_SIZE + eyeOffsetY, eyeSize, eyeSize, { 0, 0, 0 }); // Prawe oko

        } else {
            drawTexture(snakeTexture, snake[i].x, snake[i].y, CELL_SIZE, CELL_SIZE); // Ogon
        }
    }
    drawTexture(appleTexture, foodX, foodY, CELL_SIZE, CELL_SIZE); // Jedzenie
}

void initSnake() {
    snake.push_back({ GRID_WIDTH / 2, GRID_HEIGHT / 2 });
    snake.push_back({ GRID_WIDTH / 2, GRID_HEIGHT / 2 + 1 });
    snake.push_back({ GRID_WIDTH / 2, GRID_HEIGHT / 2 + 2 });
}

void placeFood() {
    srand(time(0));
    foodX = rand() % (GRID_WIDTH - 2) + 1;
    foodY = rand() % (GRID_HEIGHT - 2) + 1;
}

void updateSnake(double dt) {
    if (accelerate) {
        velocity += accelerationRate * dt;
    }
    static double accumulatedTime = 0.0;
    accumulatedTime += dt * velocity;
    if (accumulatedTime < 1.0) {
        return;
    }
    accumulatedTime = 0.0;

    Segment newHead = snake[0];
    switch (dir) {
        case UP: newHead.y--; break;
        case DOWN: newHead.y++; break;
        case LEFT: newHead.x--; break;
        case RIGHT: newHead.x++; break;
    }

    if (newHead.x == 0 || newHead.x == GRID_WIDTH - 1 || newHead.y == 0 || newHead.y == GRID_HEIGHT - 1) {
        std::cout << "Kolizja z ramka! Koniec gry." << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(0);
    }

    for (int i = 1; i < snake.size(); i++) {
        if (newHead.x == snake[i].x && newHead.y == snake[i].y) {
            std::cout << "Zjadłeś samego siebie! Koniec gry." << std::endl;
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            exit(0);
        }
    }

    snake.insert(snake.begin(), newHead);
    if (newHead.x == foodX && newHead.y == foodY) {
        placeFood();
    } else {
        snake.pop_back();
    }
}

void handleInput(SDL_Event& e) {
    if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
            case SDLK_UP: if (dir != DOWN) dir = UP; break;
            case SDLK_DOWN: if (dir != UP) dir = DOWN; break;
            case SDLK_LEFT: if (dir != RIGHT) dir = LEFT; break;
            case SDLK_RIGHT: if (dir != LEFT) dir = RIGHT; break;
            case SDLK_p: accelerate = true; break;
        }
    } else if (e.type == SDL_KEYUP) {
        switch (e.key.keysym.sym) {
            case SDLK_p: accelerate = false; velocity = 2.0; break;
        }
    }
}

void cleanup() {
    SDL_DestroyTexture(appleTexture);
    SDL_DestroyTexture(snakeTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        return false;
    }

    // Ładowanie tekstury jabłka
    SDL_Surface* surface = SDL_LoadBMP("C://Users//debis//Downloads//apple.bmp");
    if (!surface) {
        std::cout << "Unable to load image! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        return false;
    }
    appleTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!appleTexture) {
        std::cout << "Unable to create texture! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        return false;
    }

    // Ładowanie tekstury węża
    SDL_Surface* snake_surface= SDL_LoadBMP("C://Users//debis//Downloads//snake.bmp");
    if (!snake_surface) {
        std::cout << "Unable to load image! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        return false;
    }
    snakeTexture = SDL_CreateTextureFromSurface(renderer, snake_surface);
    SDL_FreeSurface(snake_surface);
    if (!snakeTexture) {
        std::cout << "Unable to create texture! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        return false;
    }

    return true;
}

int main(int argc, char* args[]) {
    if (!init()) {
        return -1;
    }

    initSnake();
    placeFood();

    bool quit = false;
    SDL_Event e;

    Uint32 lastUpdateTime = SDL_GetTicks();

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            handleInput(e);
        }

        Uint32 currentTime = SDL_GetTicks();
        double deltaTime = (currentTime - lastUpdateTime) / 1000.0;
        lastUpdateTime = currentTime;

        updateSnake(deltaTime);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //tlo
        SDL_RenderClear(renderer);

        drawGrid();

        SDL_RenderPresent(renderer);
        SDL_Delay(100);
    }

    cleanup();

    return 0;
}

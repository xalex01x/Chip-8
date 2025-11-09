#include <SDL2/SDL.h>
#include <iostream>
#include <time.h>
#include"chip-8.cpp"

extern u8 display[WIDTH*HEIGH];
u8 oldFrame[WIDTH*HEIGH];

SDL_Renderer* renderer;
SDL_Rect rect;

void printDisplay(){
    for(int i = 0; i < HEIGH; i++){
        for(int j = 0; j < WIDTH; j++){
            if(display[i*WIDTH + j] == oldFrame[i*WIDTH + j]) continue;
            oldFrame[i*WIDTH + j] = display[i*WIDTH + j];
            if(display[i*WIDTH + j] == black) SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            else SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            rect.x = j*CELL_SIDE;
            rect.y = i*CELL_SIDE;
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

int main(int argc, char* argv[]) {
    char * rom;
    if(argc < 2) {
        std::cout<< "Specifica il percorso di una cartuccia." << std:: endl;
        return 0;
    } else rom = argv[1];
    rect.w = CELL_SIDE;
    rect.h = CELL_SIDE;
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cout << "Errore nell'inizializzare SDL: " << SDL_GetError() << std::endl;
        return 1;
    }
    SDL_Window* window = SDL_CreateWindow("Chip-8", 
                                          SDL_WINDOWPOS_CENTERED, 
                                          SDL_WINDOWPOS_CENTERED, 
                                          WIDTH*CELL_SIDE, HEIGH*CELL_SIDE, 
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        std::cout << "Errore nella creazione della finestra: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cout << "Errore nella creazione del renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    bool isRunning = true;
    SDL_Event event;

    chipInit(rom);
    memset(oldFrame, black, WIDTH*HEIGH);

    clock_t frameStart;
    float frameTime;

    while (isRunning) {

        frameStart = clock();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
        }
        chipCycle();
        frameTime = (float)(clock() - frameStart) / CLOCKS_PER_SEC;

        SDL_RenderPresent(renderer);

        if (frameTime < RR) {
            int delayTime = (int)((RR - frameTime) * 1000);
            SDL_Delay(delayTime);
        }
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
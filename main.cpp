#include<iostream>
#include <SDL.h>

#include "vDisp.h"

//SDL parameters
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

SDL_Window* window = SDL_CreateWindow("SDLBasic", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);


bool isRunning = true;
SDL_Event event;
const Uint8* keyboardStateArray = SDL_GetKeyboardState(NULL);

vDisp disp(renderer, 20, WINDOW_WIDTH, WINDOW_HEIGHT);

void ProcessEvent() 
{
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_MOUSEWHEEL:
            int x, y;
            SDL_GetMouseState(&x, &y);
            disp.Zoom(event.wheel.y,x,y);
            //std::cout << event.wheel.y << std::endl;
            break;

        case SDL_QUIT:
            isRunning = false;
            break;
        case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE)
        {
            isRunning = false;
        }
        break;
        

        }

        disp.EventHandler(event);
    }
}

void Render() 
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    disp.Render();
    
    SDL_RenderPresent(renderer);
    
}

int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);
    while (isRunning)
    {
        ProcessEvent();
        Render();
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
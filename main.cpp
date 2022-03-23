#include <iostream>
#include <string>
#include <SDL.h>
#include <SDL_ttf.h>

#include "vDisp.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"

//SDL parameters
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Event event;

bool isRunning = true;
const Uint8* keyboardStateArray = SDL_GetKeyboardState(NULL);
vDisp disp;

bool x = true;

int mouseX, mouseY;
SDL_Point worldCords;

TTF_Font* font = nullptr;
int fontSize = 20;
SDL_Surface* messageSurface = nullptr;
SDL_Texture* messageTex = nullptr;
SDL_Rect messageRect;
std::string screenPosMes;

long long int new_time, time_diff, old_time;
int frame_rate = 60, frame_count = 0;

std::string FPSmessage;

bool init()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();

    window = SDL_CreateWindow("SDLBasic", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (window == nullptr)
    {
        std::cout << "Window init failed\n";
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == nullptr)
    {
        std::cout << "Rendrere init failed\n";
        return false;
    }

    font = TTF_OpenFont("assets\\font.ttf", fontSize);
    if (font == nullptr)
    {
        std::cout << TTF_GetError() << std::endl;
        return false;
    }
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    disp.init(renderer, 20, WINDOW_WIDTH, WINDOW_HEIGHT);


    return true;
}

void Imgui_Init()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer_Init(renderer);

}

void Imgui_dest()
{
    // Cleanup
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

}

void ProcessEvent() 
{
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        SDL_GetMouseState(&mouseX, &mouseY);
        worldCords = disp.screenCordsToWorldCords(mouseX, mouseY);
        switch (event.type)
        {
        case SDL_MOUSEWHEEL:
            disp.Zoom(event.wheel.y,mouseX,mouseY);
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


void renderText(const std::string& message, const int& posX, const int& posY)
{
    messageSurface = TTF_RenderText_Solid(font, message.c_str(), { 255,255,255,255 });
    messageTex = SDL_CreateTextureFromSurface(renderer, messageSurface);
    messageRect = { posX, posY, messageSurface->w, messageSurface->h};
    SDL_RenderCopy(renderer, messageTex, NULL, &messageRect);
    SDL_FreeSurface(messageSurface);
    SDL_DestroyTexture(messageTex);
}

void Render() 
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    ImGui::Render();
    disp.Render();

    screenPosMes = "Screen Cords=(" + std::to_string(mouseX) + ", " + std::to_string(mouseY) + ")";
    renderText(screenPosMes, 0, 0);
    screenPosMes = "World Cords=("  + std::to_string(worldCords.x) + ", " + std::to_string(worldCords.y) + ")";
    renderText(screenPosMes, 0, fontSize);

    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());


    FPSmessage = "FPS: " + std::to_string(frame_rate);
    renderText(FPSmessage, 0, 2 * fontSize);

    SDL_RenderPresent(renderer);
    
}

void imgui()
{
    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();

    disp.imgui();
    
}


int main(int argc, char* argv[])
{
    if (!init())
        return -1;
    
    Imgui_Init();

    old_time = SDL_GetTicks64();
    while (isRunning)
    {
        ProcessEvent();

        imgui();

        Render();
        //frame rate
        new_time = SDL_GetTicks64();
        time_diff = new_time - old_time;
        frame_count++;
        if (time_diff >= 1000) {
            frame_rate = ceil(((float)(frame_count * 1000)) / (float)time_diff);
            frame_count = 0;
            old_time = new_time;
        }
    }

    Imgui_dest();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
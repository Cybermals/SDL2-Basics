/*
SDL2 Window Demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>


//Macros
//===========================================================================
#ifndef TRUE
    #define TRUE  1
    #define FALSE 0
#endif

#define SDL_INIT_FLAGS    (SDL_INIT_EVENTS | SDL_INIT_TIMER | SDL_INIT_VIDEO)
#define APP_TITLE         "SDL2 Window"
#define WINDOW_X          SDL_WINDOWPOS_CENTERED
#define WINDOW_Y          SDL_WINDOWPOS_CENTERED
#define WINDOW_WIDTH      800
#define WINDOW_HEIGHT     600

#ifdef __ANDROID__
    #define WINDOW_FLAGS  SDL_WINDOW_FULLSCREEN
#else
    #define WINDOW_FLAGS  0
#endif

#define FPS               60
#define TARGET_FRAME_TIME (1000 / FPS)


//Globals
//===========================================================================
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

int startTime = 0;
int endTime = 0;
int frameTime = 0;


//Functions
//===========================================================================
void Quit(void)
{
    //Destroy renderer and window
    if(renderer)
    {
        SDL_DestroyRenderer(renderer);
    }
    
    if(window)
    {
        SDL_DestroyWindow(window);
    }
    
    //Quit SDL2
    SDL_Quit();
}


//Entry Point
//===========================================================================
int main(int argc, char **argv)
{
    //Init SDL2
    SDL_Log("%s", "Initializing SDL2...");
    
    if(SDL_Init(SDL_INIT_FLAGS) == -1)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
        return 1;
    }
    
    atexit(&Quit);
    
    //Create a window
    SDL_Log("%s", "Creating a window and renderer...");
    
    if(SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_FLAGS, 
        &window, &renderer) == -1)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
        return 1;
    }
    
    SDL_SetWindowTitle(window, APP_TITLE);
    SDL_SetWindowPosition(window, WINDOW_X, WINDOW_Y);
    
    //Main Loop
    SDL_Log("%s", "Starting main loop...");
    
    while(TRUE)
    {
        //Process pending events
        SDL_Event event;
        
        while(SDL_PollEvent(&event))
        {
            //Handle the next event
            switch(event.type)
            {
                //Quit event
            case SDL_QUIT:
                return 0;
            }
        }
        
        //Clear the window
        SDL_SetRenderDrawColor(renderer, 0, 128, 255, 255);
        SDL_RenderClear(renderer);
        
        //Swap buffers
        SDL_RenderPresent(renderer);
        endTime = SDL_GetTicks();
        frameTime = endTime - startTime;
        startTime = endTime;
        
        //Limit framerate to 60 fps.
        if(frameTime < TARGET_FRAME_TIME)
        {
            SDL_Delay(TARGET_FRAME_TIME - frameTime);
        }
    }
    
    return 0;
}
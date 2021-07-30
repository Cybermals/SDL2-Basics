/*
SDL2 Images Demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


//Macros
//===========================================================================
#ifndef TRUE
    #define TRUE  1
    #define FALSE 0
#endif

#define SDL_INIT_FLAGS    (SDL_INIT_EVENTS | SDL_INIT_TIMER | SDL_INIT_VIDEO)
#define APP_TITLE         "SDL2 Images"
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


//Types
//===========================================================================
typedef struct
{
    SDL_Texture *tex;
    SDL_Rect rect;
    SDL_Point velocity;
} Bubble;


//Globals
//===========================================================================
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

SDL_Point windowSize;

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
        //This also frees all textures associated with this renderer
        SDL_DestroyRenderer(renderer);
    }
    
    if(window)
    {
        SDL_DestroyWindow(window);
    }
    
    //Quit SDL2
    IMG_Quit();
    SDL_Quit();
}


SDL_Texture *LoadImage(const char *filename, SDL_Rect *rect)
{
    //Load the image file
    SDL_Surface *img = IMG_Load(filename);
    
    if(!img)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
        return NULL;
    }
    
    //Fill out the given rect
    if(rect)
    {
        rect->x = 0;
        rect->y = 0;
        rect->w = img->w;
        rect->h = img->h;
    }
    
    //Create a texture from the image
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, img);
    
    if(!tex)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
    }
    
    //Free the image and return the texture
    SDL_FreeSurface(img);
    return tex;
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
    
    //Init SDL2_image
    SDL_Log("%s", "Initializing SDL2_image...");
    
    if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
        return 1;
    }
    
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
    
    #ifdef __ANDROID__
    SDL_Rect screen;
    SDL_GetDisplayUsableBounds(0, &screen);
    windowSize.x = screen.w;
    windowSize.y = screen.h;
    #else
    SDL_GetWindowSize(window, &windowSize.x, &windowSize.y);
    #endif
    
    //Create a bubble
    Bubble bubble;
    bubble.tex = LoadImage("data/images/bubble.png", &bubble.rect);
    
    if(!bubble.tex)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", 
            "Failed to create a bubble.");
        return 1;
    }
    
    bubble.rect.w *= 2;
    bubble.rect.h *= 2;
    bubble.velocity.x = 1;
    bubble.velocity.y = 1;
    
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
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        
        //Update the bubble
        bubble.rect.x += bubble.velocity.x;
        bubble.rect.y += bubble.velocity.y;
        
        if(bubble.rect.x < 0 || bubble.rect.x + bubble.rect.w > windowSize.x - 1)
        {
            bubble.velocity.x = -bubble.velocity.x;
        }
        
        if(bubble.rect.y < 0 || bubble.rect.y + bubble.rect.h > windowSize.y - 1)
        {
            bubble.velocity.y = -bubble.velocity.y;
        }
        
        SDL_RenderCopy(renderer, bubble.tex, NULL, &bubble.rect);
        
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
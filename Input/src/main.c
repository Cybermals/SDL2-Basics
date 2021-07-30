/*
SDL2 Input Demo
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


//Macros
//===========================================================================
#ifndef TRUE
    #define TRUE  1
    #define FALSE 0
#endif

#define SDL_INIT_FLAGS    (SDL_INIT_EVENTS | SDL_INIT_TIMER | SDL_INIT_VIDEO)
#define APP_TITLE         "SDL2 Input"
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
} Pin;


typedef struct
{
    SDL_Texture *tex;
    SDL_Rect rect;
    SDL_FPoint pos;
    SDL_FPoint velocity;
    int hp;
} Bubble;


//Globals
//===========================================================================
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

SDL_Point windowSize;

int startTime = 0;
int endTime = 0;
int frameTime = 0;

SDL_Texture *pinTex = NULL;
Pin pin;

int spawnTmr = 60;
SDL_Texture *bubbleTex = NULL;
SDL_Texture *poppingBubbleTex = NULL;
SDL_Rect bubbleRect;
Bubble bubbles[10];


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


int InitPin(void)
{
    //Load pin texture
    pinTex = LoadImage("data/images/pin.png", &pin.rect);
    
    if(!pinTex)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s",
            "Failed to load pin texture.");
        return 1;
    }
    
    pin.rect.w *= 2;
    pin.rect.h *= 2;
    return 0;
}


int InitBubbles(void)
{
    //Initialize bubbles array
    memset(bubbles, 0, sizeof(bubbles));
    
    //Load bubble textures
    bubbleTex = LoadImage("data/images/bubble.png", &bubbleRect);
    
    if(!bubbleTex)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", 
            "Failed to load bubble textures.");
        return 1;
    }
    
    bubbleRect.w *= 2;
    bubbleRect.h *= 2;
    poppingBubbleTex = LoadImage("data/images/popping-bubble.png", NULL);
    
    if(!poppingBubbleTex)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", 
            "Failed to load bubble textures.");
        return 1;
    }
    
    return 0;
}


int Init(void)
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
    
    //Init random numbers
    srand(time(0));
    
    //Init pin
    if(InitPin())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s",
            "Failed to initialize pin.");
        return 1;
    }
    
    //Init bubbles
    if(InitBubbles())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s",
            "Failed to initialize bubbles.");
        return 1;
    }
    
    return 0;
}


int randint(int min, int max)
{
    return min + (rand() % (max - min));
}


float radians(float degrees)
{
    return degrees * (M_PI / 180.0f);
}


void center(SDL_Point *center, const SDL_Rect *rect)
{
    center->x = rect->x + rect->w / 2;
    center->y = rect->y + rect->h / 2;
}


void SpawnBubble(void)
{
    //Update spawn timer
    if(spawnTmr--)
    {
        return;
    }
    
    //Reset spawn timer
    spawnTmr = 60;
    
    //Spawn a bubble
    for(int i = 0; i < sizeof(bubbles) / sizeof(bubbles[0]); i++)
    {
        Bubble *bubble = &bubbles[i];
        
        if(bubble->tex)
        {
            continue;
        }
        
        bubble->tex = bubbleTex;
        memcpy(&bubble->rect, &bubbleRect, sizeof(bubbleRect));
        bubble->pos.x = randint(0, windowSize.x - bubbleRect.w);
        bubble->pos.y = randint(0, windowSize.y - bubbleRect.h);
        bubble->velocity.x = cos(radians(randint(0, 360)));
        bubble->velocity.y = sin(radians(randint(0, 360)));
        bubble->hp = 100;
        return;
    }
}


void SetPinPos(int x, int y)
{
    pin.rect.x = x;
    pin.rect.y = y;
}


void ShowPin(int doShow)
{
    //Show the pin?
    if(doShow)
    {
        pin.tex = pinTex;
    }
    //Hide the pin?
    else
    {
        pin.tex = NULL;
    }
}


void UpdatePin(void)
{
    //Is the pin visible?
    if(!pin.tex)
    {
        return;
    }
    
    //Render the pin
    SDL_RenderCopy(renderer, pin.tex, NULL, &pin.rect);
}


void UpdateBubble(Bubble *bubble)
{
    //Does this bubble exist?
    if(!bubble->tex || !bubble->hp)
    {
        return;
    }
    //Is this bubble popped?
    else if(bubble->hp < 0)
    {
        if(++bubble->hp == 0)
        {
            bubble->tex = NULL;
            return;
        }
    }
    //Is this bubble active?
    else
    {
        //Do collision detection
        if(pin.tex && SDL_HasIntersection(&bubble->rect, &pin.rect))
        {
            bubble->tex = poppingBubbleTex;
            bubble->hp = -30;
        }
        else
        {
            for(int i = 0; i < sizeof(bubbles) / sizeof(bubbles[0]); i++)
            {
                Bubble *bubble2 = &bubbles[i];
            
                //Don't check collision between a bubble and itself or with a bubble
                //that doesn't exist.
                if(bubble == bubble2 || !bubble2->tex || bubble2->hp <= 0)
                {
                    continue;
                }
        
                //Have the bubbles collided
                SDL_Point p1;
                SDL_Point p2;
                center(&p1, &bubble->rect);
                center(&p2, &bubble2->rect);
            
                int dx = p2.x - p1.x;
                int dy = p2.y - p1.y;
            
                if(sqrt((float)(dx * dx + dy * dy)) < 
                    bubble->rect.w / 2 + bubble2->rect.h / 2)
                {
                    bubble->velocity.x = -bubble->velocity.x;
                    bubble->velocity.y = -bubble->velocity.y;
            
                    if(--bubble->hp == 0)
                    {
                        bubble->tex = poppingBubbleTex;
                        bubble->hp = -30;
                    }
            
                    bubble2->velocity.x = -bubble2->velocity.x;
                    bubble2->velocity.y = -bubble2->velocity.y;
            
                    if(--bubble2->hp == 0)
                    {
                        bubble2->tex = poppingBubbleTex;
                        bubble2->hp = -30;
                    }
                }
            }
        }
    
        //Update pos and velocity
        bubble->pos.x += bubble->velocity.x;
        bubble->pos.y += bubble->velocity.y;
    
        if(bubble->pos.x < 0 || bubble->pos.x + bubble->rect.w > windowSize.x)
        {
            bubble->velocity.x = -bubble->velocity.x;
        }
    
        if(bubble->pos.y < 0 || bubble->pos.y + bubble->rect.h > windowSize.y)
        {
            bubble->velocity.y = -bubble->velocity.y;
        }
    
        bubble->rect.x = bubble->pos.x;
        bubble->rect.y = bubble->pos.y;
    }
    
    //Draw the bubble
    SDL_RenderCopy(renderer, bubble->tex, NULL, &bubble->rect);
}


//Entry Point
//===========================================================================
int main(int argc, char **argv)
{
    //Init
    if(Init())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", 
            "Failed to initialize app.");
        return 1;
    }
    
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
                //Quit Event
            case SDL_QUIT:
                return 0;
                
                //Mouse Button Down Event
            case SDL_MOUSEBUTTONDOWN:
                ShowPin(TRUE);
                SetPinPos(event.button.x, event.button.y);
                break;
                
                //Mouse Button Up Event
            case SDL_MOUSEBUTTONUP:
                ShowPin(FALSE);
                break;
                
                //Mouse Motion Event
            case SDL_MOUSEMOTION:
                SetPinPos(event.motion.x, event.motion.y);
                break;
            }
        }
        
        //Clear the window
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        
        //Update pin
        UpdatePin();
        
        //Update bubbles
        SpawnBubble();
        
        for(int i = 0; i < sizeof(bubbles) / sizeof(bubbles[0]); i++)
        {
            UpdateBubble(&bubbles[i]);
        }
        
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
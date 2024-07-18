#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

// GLOBAL VARIABLES
bool is_running = false;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

// USER-DEFINED FUNCTIONS

// setting up the SDL environment
bool initialize_window(void) {
    // initialising the SDL library;  
    // SDL_INIT_EVERYTHING flag initialises all the SDL subsystems
    // if everything goes right, SDL_Init() returns 0
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        fprintf(stderr, "Error initializing SDL \n");
        return false;
    }

    // creating an SDL window
    window = SDL_CreateWindow(
        NULL, // title of the window; we don't want any :)
        SDL_WINDOWPOS_CENTERED, // the x-axis position of the window
        SDL_WINDOWPOS_CENTERED, // the y-axis position of the window
        800, // the width (in pixels)
        600, // the height (in pixels)
        SDL_WINDOW_BORDERLESS // border reference; we want it to be borderless :)
    );

    if(!window){
        fprintf(stderr, "Error creating SDL window \n");
        return false;
    }

    // creating an SDL renderer
    // the FIRST argument (window) is the window to associate the renderer with.
    // the SECOND one (-1) is the index of the rendering driver; 
    // giving -1 lets SDL to choose the first one supporting the requested flags.
    // the THIRD argument is the flags to set for the renderer (0 means no flags).
    renderer = SDL_CreateRenderer(window, -1, 0); 

    if(!renderer){
        fprintf(stderr, "Error creating SDL renderer \n");
        return false;
    }

    return true;
}


// MAIN FUNCTION
int main(void){

    is_running = initialize_window();

    return 0;
}
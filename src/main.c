#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

// GLOBAL VARIABLES
bool is_running = false;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

uint32_t* color_buffer = NULL;

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

void setup(void){
    // setting up the color_buffer
    color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * widow_height);
}

// for input validation and processing
void process_input(void){
    // datatype of SDL_Event stores the information about an event.
    // e.g. keypress, mouse movement, window close, etc. 
    SDL_Event event;
    //SDL_PollEvent polls for currently pending events. 
    // if there are events in the queue, it fills the 'event' variable with the event data
    // and removes it from the queue. 
    SDL_PollEvent(&event);

    // indentifying the input provided
    switch(event.type){
        case SDL_QUIT: // if the user attempts to close the window; is_running = false
            is_running = false;
            break;
        
        case SDL_KEYDOWN: // handles the event where a key is pressed on the keyboard
            if(event.key.keysym.sym == SDLK_ESCAPE) // checks if the key pressed is 'esc'
                is_running = false;
            break;
    }
}

void update(void){}

// handling the rendering process
void render(void){
    // this functions sets the color used for drawing operations in the renderer.
    // 'renderer' is the SDL_Renderer object used for rendering. 
    // '250, 10, 89, 255' ; values for RGB + the opacity
    SDL_SetRenderDrawColor(renderer, 250, 10, 89, 255); 
    // this function clears the current rendering target 
    // with the drawing color set by SDL_SetRenderDrawColor
    SDL_RenderClear(renderer);
    // this function updates the screen with any rendering performed  
    // since the previous call. it swaps the back buffer with the fron buffer,
    // displaying the current rendering result on the screen.
    SDL_RenderPresent(renderer);
}

// MAIN FUNCTION
int main(void){
    is_running = initialize_window();

    setup();

    while(is_running){
        process_input();
        update();
        render();
    }

    return 0;
}
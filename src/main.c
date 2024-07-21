#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

// GLOBAL VARIABLES
bool is_running = false;

int window_width = 800;
int window_height = 600;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

uint32_t* color_buffer = NULL;
SDL_Texture* color_buffer_texture = NULL;

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

    // retrieving the current display mode of the screen.
    // SDL_DisplayMode contains the info about the display mode,
    // such as the width, height, refresh rate, and pixels format
    SDL_DisplayMode display_mode; 
    // this function retrieves the current display mode of the display screen
    // 1st argument is the index of the display device. 
    // 0 index is of the primary device.
    // 2nd argument is the pointer to the SDL_DisplayMode structure
    SDL_GetCurrentDisplayMode(0, &display_mode);

    // updating window width and height
    window_width = display_mode.w;
    window_height = display_mode.h;

    // creating an SDL window
    window = SDL_CreateWindow(
        NULL, // title of the window; we don't want any :)
        SDL_WINDOWPOS_CENTERED, // the x-axis position of the window
        SDL_WINDOWPOS_CENTERED, // the y-axis position of the window
        window_width, // the width (in pixels)
        window_height, // the height (in pixels)
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

    // setting the SDL window to fullscreen.
    // 'window' is the pointer to the SDL_Window structure
    // SDL_WINDOW_FULLSCREEN is a flag that indicates you want --
    // to switch the window to fullscreen mode
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    return true;
}

void setup(void){
    // allocating the memory (in bytes) to hold the color_buffer
    color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height);

    // creating the SDL_Texture that is used to display the color_buffer
    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );
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

// updating the texture with the color_buffer data + rendering it to the screen.
void render_color_buffer(void){
    // this function updates the given texture with new pixel data. 
    SDL_UpdateTexture(
        color_buffer_texture, // the SDL_Texture that will be updated
        NULL, // a pointer to the rectangle of the texture to update. 
        // NULL means the entire texture will be updated. 
        color_buffer, // a pointer to the pixel data that will be copied into the texture.
        (int)(window_width * sizeof(uint32_t)) // this is the pitch (no. of bytes per row)
        // typecasted into (int); for just in case :)
    );
    // this function copies the texture to the current rendering target (SDL_Renderer)
    SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
    // the 3rd agrument is the [source rect], the portion of the texture to copy.
    // NULL means the entire texture.
    // 4th argument is the [destination rect], the portion of the rendering target to copy.
    // NULL means the entire target.
}

// cleaning up the color_buffer
void clear_color_buffer(uint32_t color){
    for(int y = 0; y < window_height; ++y){
        for(int x = 0; x < window_width; ++x){
            // because the color_buffer is a 2D matrix represented using an array
            color_buffer[(window_width * y) + x] = color;
        }
    }
}

// handling the rendering process
void render(void){
    // this functions sets the color used for drawing operations in the renderer.
    // 'renderer' is the SDL_Renderer object used for rendering. 
    // '250, 10, 89, 255' ; values for RGB + the opacity
    SDL_SetRenderDrawColor(renderer, 250, 10, 89, 255); 
    // this function clears the current rendering target 
    // with the drawing color set by SDL_SetRenderDrawColor
    SDL_RenderClear(renderer);

    // comments above ^
    render_color_buffer();
    clear_color_buffer(0xFFFFFF00);

    // this function updates the screen with any rendering performed  
    // since the previous call. it swaps the back buffer with the fron buffer,
    // displaying the current rendering result on the screen.
    SDL_RenderPresent(renderer);
}

// cleanup function
void destroy_window(void){
    // free-up the allocated memory (color_buffer)
    free(color_buffer);
    // destroy the SDL_Renderer
    SDL_DestroyRenderer(renderer);
    // destroy the SDL_Window
    SDL_DestroyWindow(window);
    // deactivating the SDL library 
    SDL_Quit();
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

    destroy_window();

    return 0;
}
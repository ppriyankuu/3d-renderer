#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "headers/display.h"
#include "headers/vector.h"

// GLOBAL VARIABLES
bool is_running = false;

// USER-DEFINED FUNCTIONS

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

// handling the rendering process
void render(void){
    // this functions sets the color used for drawing operations in the renderer.
    // 'renderer' is the SDL_Renderer object used for rendering. 
    // '250, 10, 89, 255' ; values for RGB + the opacity
    SDL_SetRenderDrawColor(renderer, 250, 10, 89, 255); 
    // this function clears the current rendering target 
    // with the drawing color set by SDL_SetRenderDrawColor
    SDL_RenderClear(renderer);

    draw_grid();
    draw_pixel(200, 200, 0xFFFF00FF);
    // draw_rect(400, 500, 400, 500, 0xFF333333);

    // comments above ^
    render_color_buffer();
    clear_color_buffer(0xFF000000);

    // this function updates the screen with any rendering performed  
    // since the previous call. it swaps the back buffer with the fron buffer,
    // displaying the current rendering result on the screen.
    SDL_RenderPresent(renderer);
}

// MAIN FUNCTION
int main(void){
    is_running = initialize_window();

    setup();

    vec3_t my_vector = {2.0, 3.0, 4.0};

    while(is_running){
        process_input();
        update();
        render();
    }

    destroy_window();

    return 0;
}
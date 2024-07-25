#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "headers/display.h"
#include "headers/vector.h"

// GLOBAL VARIABLES
bool is_running = false;

#define N_POINTS (9 * 9 * 9)


// representing a 9 x 9 x 9 cube filled with 3D vectors points
vec3_t cube_points[N_POINTS]; 
vec2_t projected_points[N_POINTS];  

vec3_t camera_view = {.x = 0, .y = 0, .z = -5};
int FOV_factor = 640;

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

    int points_count = 0;

    // loading the array of vectors
    // from -1 to 1 in the graph/plane
    for(float x = -1; x <= 1; x += 0.25){
        for(float y = -1; y <= 1; y += 0.25){
            for(float z = -1; z <= 1; z += 0.24){
                vec3_t new_point = {.x = x, .y = y, .z = z};
                cube_points[points_count++] = new_point;
            }
        }
    }
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

// function that receives a 3D vector and returns a 2D point
vec2_t project(vec3_t point){
    vec2_t projected_point = {
        .x = (FOV_factor * point.x) / point.z, 
        .y = (FOV_factor * point.y) / point.z
    };
    return projected_point;
}

void update(void){
    for(int i = 0; i < N_POINTS; ++i){
        vec3_t point = cube_points[i];

        point.z -= camera_view.z;

        vec2_t projected_point = project(point);
        // saving the projected 2D point in the 'projected_points' array
        projected_points[i] = projected_point;
    }
}

// handling the rendering process
void render(void){
    draw_grid();

    // looping all projected points and rendering them
    for(int i = 0; i < N_POINTS; ++i){
        vec2_t projected_point = projected_points[i];
        draw_rect(
            projected_point.x + (window_width / 2), 
            projected_point.y + (window_height / 2), 
            4, 
            4, 
            0xFFFF00FF
        );
    }

    // draw_pixel(200, 200, 0xFFFF00FF);

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

    while(is_running){
        process_input();
        update();
        render();
    }

    destroy_window();

    return 0;
}
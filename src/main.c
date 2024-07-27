#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "headers/display.h"
#include "headers/vector.h"
#include "headers/mesh.h"

// GLOBAL VARIABLES
bool is_running = false;
int previous_frame_time = 0;

triangle_t triangles_to_render[N_MESH_FACES];

vec3_t camera_view = {.x = 0, .y = 0, .z = -5};
vec3_t cube_rotation = {.x = 0, .y = 0, .z = 0};

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
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
    if(time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) SDL_Delay(time_to_wait);

    previous_frame_time = SDL_GetTicks();

    cube_rotation.x += 0.01;
    cube_rotation.y += 0.01;
    cube_rotation.z += 0.01;

    for(int i = 0; i < N_MESH_FACES; ++i){
        fact_t mesh_face = mesh_faces[i];

        vec3_t face_vertices[3];
        face_vertices[0] = mesh_vertices[mesh_face.a - 1];
        face_vertices[1] = mesh_vertices[mesh_face.b - 1];
        face_vertices[2] = mesh_vertices[mesh_face.c - 1];

        triangle_t projected_triangle;

        for(int j = 0; j < 3; ++j){
            vec3_t transformed_vertex = face_vertices[j];

            transformed_vertex = vec3_rotate_x(transformed_vertex, cube_rotation.x);
            transformed_vertex = vec3_rotate_y(transformed_vertex, cube_rotation.y);
            transformed_vertex = vec3_rotate_z(transformed_vertex, cube_rotation.z);

            transformed_vertex.z -= camera_view.z;

            vec2_t projected_point = project(transformed_vertex);

            projected_point.x += (window_width / 2);
            projected_point.y += (window_height / 2);

            projected_triangle.points[j] = projected_point;
        }

        triangles_to_render[i] = projected_triangle;
    }
}

// handling the rendering process
void render(void){
    draw_grid();

    for(int i = 0; i < N_MESH_FACES; ++i){
        triangle_t triangle = triangles_to_render[i];
        draw_rect(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFF3333FF);
        draw_rect(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFF3333FF);
        draw_rect(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFF3333FF);
    }

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
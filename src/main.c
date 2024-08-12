#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "headers/array.h"
#include "headers/display.h"
#include "headers/vector.h"
#include "headers/mesh.h"
#include "headers/sort.h"
#include "headers/matrix.h"

// GLOBAL VARIABLES
bool is_running = false;
int previous_frame_time = 0;

triangle_t* triangles_to_render = NULL;

vec3_t camera_view = {0, 0, 0};

int FOV_factor = 640;

// USER-DEFINED FUNCTIONS

void setup(void){
    // initialising render mode and triangle culling method
    render_method = RENDER_WIRE;
    cull_method = CULL_BACKFACE; 

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

    load_cube_mesh_data();
    // load_obj_file_data("./assets/cube.obj");
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
            
            if(event.key.keysym.sym == SDLK_1)
                render_method = RENDER_WIRE_VERTEX;

            if(event.key.keysym.sym == SDLK_2)
                render_method = RENDER_WIRE;

            if(event.key.keysym.sym == SDLK_3)
                render_method = RENDER_FILL_TRIANGLE;

            if(event.key.keysym.sym == SDLK_4)
                render_method = RENDER_FILL_TRIANGLE_WIRE;

            if(event.key.keysym.sym == SDLK_c)
                cull_method = CULL_BACKFACE;
            
            if(event.key.keysym.sym == SDLK_d)
                cull_method = CULL_NONE;
            
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

    triangles_to_render = NULL;

    mesh.rotation.x += 0.01;
    mesh.rotation.y += 0.01;
    mesh.rotation.z += 0.01;

    // mesh.scale.x += 0.002;
    // mesh.scale.y += 0.001;
    // mesh.translation.x += 0.01;
    // mesh.translation.y += 0.01;
    mesh.translation.z = 5.0;

    mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
    mat4_t translation_matrix = mat4_make_translation(
                                    mesh.translation.x, 
                                    mesh.translation.y, 
                                    mesh.translation.z
                                );
    mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
    mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh.rotation.y);
    mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh.rotation.z);

    int num_of_faces = array_length(mesh.faces);

    for(int i = 0; i < num_of_faces; ++i){
        face_t mesh_face = mesh.faces[i];

        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a - 1];
        face_vertices[1] = mesh.vertices[mesh_face.b - 1];
        face_vertices[2] = mesh.vertices[mesh_face.c - 1];

        vec4_t transformed_vertices[3];

        for(int j = 0; j < 3; ++j){
            vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

            // creating a world matrix combining scale, rotation, and translation matrices.
            mat4_t world_matrix = mat4_identity();
            
            //////// ORDER of transformation /////////
            // first SCALE
            // then ROTATE
            // lastly TRANSLATE

            world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
            
            world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);

            world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

            transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);

            // saving the transformed vertex in the array of transformed_vertices
            transformed_vertices[j] = transformed_vertex;
        }
        
        if(cull_method == CULL_BACKFACE){
            vec3_t vector_A = vec3_from_vec4(transformed_vertices[0]);
            vec3_t vector_B = vec3_from_vec4(transformed_vertices[1]);
            vec3_t vector_C = vec3_from_vec4(transformed_vertices[2]);

            vec3_t vector_AB = vec3_sub(vector_B, vector_A);
            vec3_t vector_AC = vec3_sub(vector_C, vector_A);
            vec3_normalise(&vector_AB);        
            vec3_normalise(&vector_AC);        
            
            vec3_t vector_normal = vec3_cross(vector_AB, vector_AC);
            vec3_normalise(&vector_normal);        

            vec3_t camera_ray = vec3_sub(camera_view, vector_A);

            float dot_normal_camera = vec3_dot(vector_normal, camera_ray);

            if(dot_normal_camera < 0){
                continue;
            }
        }


        vec2_t projected_points[3];

        // projection loop for the 3 vertices
        for(int j = 0; j < 3; ++j){
            projected_points[j] = project(vec3_from_vec4(transformed_vertices[j]));

            // scaling and translating the projected_points to the middle of the screen
            projected_points[j].x += (window_width / 2);
            projected_points[j].y += (window_height / 2);
        }
        
        // calucating the average depth for each face based on the 
        // vertices after transformation
        float avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3.0; 

        triangle_t projected_triangle = {
            .points = {
                { projected_points[0].x, projected_points[0].y },
                { projected_points[1].x, projected_points[1].y },
                { projected_points[2].x, projected_points[2].y },
            },
            .color = mesh_face.color,
            .avg_depth = avg_depth 
        };
        array_push(triangles_to_render, projected_triangle);
    }

    // sorting the triangles to render by their average depth
    int num_of_triangles = array_length(triangles_to_render);
    sort_triangles(triangles_to_render, num_of_triangles);
    // for(int i = 0; i < num_of_triangles; ++i){
    //     for(int j = i; j < num_of_triangles; ++j){
    //         if(triangles_to_render[i].avg_depth < triangles_to_render[j].avg_depth){
    //             triangle_t temp =  triangles_to_render[i];
    //             triangles_to_render[i] = triangles_to_render[j];
    //             triangles_to_render[j] = temp;
    //         }
    //     }
    // }
}

// handling the rendering process
void render(void){
    SDL_RenderClear(renderer);

    draw_grid();

    int num_of_triangles = array_length(triangles_to_render);
    for(int i = 0; i < num_of_triangles; ++i){
        triangle_t triangle = triangles_to_render[i];

        if(render_method == RENDER_WIRE_VERTEX){
            // drawing vectex points
            draw_rect(triangle.points[0].x - 3, triangle.points[0].y - 3, 6, 6, 0xFFFF00FF);
            draw_rect(triangle.points[1].x - 3, triangle.points[1].y - 3, 6, 6, 0xFFFF00FF);
            draw_rect(triangle.points[2].x - 3, triangle.points[2].y - 3, 6, 6, 0xFFFF00FF);
        }

        if(render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE){
            // drawing filled triangle
            draw_filled_triangle(
                triangle.points[0].x, 
                triangle.points[0].y,
                triangle.points[1].x,
                triangle.points[1].y,
                triangle.points[2].x,
                triangle.points[2].y,
                triangle.color
            );
        }

        if(render_method == RENDER_WIRE || render_method == RENDER_WIRE_VERTEX || render_method == RENDER_FILL_TRIANGLE_WIRE){
            // drawing triangle lines
            draw_triangle(
                triangle.points[0].x, 
                triangle.points[0].y,
                triangle.points[1].x,
                triangle.points[1].y,
                triangle.points[2].x,
                triangle.points[2].y,
                0xFFFFFFFF
            );
        }
    }

    array_free(triangles_to_render);
    
    render_color_buffer();
    clear_color_buffer(0xFF000000);

    // this function updates the screen with any rendering performed  
    // since the previous call. it swaps the back buffer with the fron buffer,
    // displaying the current rendering result on the screen.
    SDL_RenderPresent(renderer);
}

void free_resources(void){
    free(color_buffer);
    array_free(mesh.faces);
    array_free(mesh.vertices);
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
    free_resources();

    return 0;
}
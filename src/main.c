#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "headers/upng.h"
#include "headers/array.h"
#include "headers/display.h"
#include "headers/vector.h"
#include "headers/mesh.h"
#include "headers/sort.h"
#include "headers/matrix.h"
#include "headers/light.h"
#include "headers/triangle.h"
#include "headers/texture.h"

// GLOBAL VARIABLES
bool is_running = false;
int previous_frame_time = 0;

triangle_t* triangles_to_render = NULL;

vec3_t camera_view = {0, 0, 0};
mat4_t projection_matrix;

// USER-DEFINED FUNCTIONS
void setup(void){
    // initialising render mode and triangle culling method
    render_method = RENDER_WIRE;
    cull_method = CULL_BACKFACE; 

    // allocating the memory (in bytes) to hold the color_buffer
    color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * window_width * window_height);
    z_buffer = (float*)malloc(sizeof(float) * window_width * window_height);

    // creating the SDL_Texture that is used to display the color_buffer
    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );

    float fov = 3.141592 / 3.0; // (PI / 3)
    float aspect = (float)window_height / (float)window_width;
    float znear = 0.1;
    float zfar = 100.0;

    // initialising the perspective projection matrix
    projection_matrix = mat4_make_perspective(fov, aspect, znear, zfar);

    // load_cube_mesh_data();
    load_obj_file_data("./assets/f22.obj");
    load_png_texture_data("./assets/f22.png");
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
            
            if(event.key.keysym.sym == SDLK_5)
                render_method = RENDER_TEXTURED;
            
            if(event.key.keysym.sym == SDLK_6)
                render_method = RENDER_TEXTURED_WIRE;

            if(event.key.keysym.sym == SDLK_c)
                cull_method = CULL_BACKFACE;
            
            if(event.key.keysym.sym == SDLK_d)
                cull_method = CULL_NONE;
            
            break;
    }
}

void update(void){
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
    if(time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) SDL_Delay(time_to_wait);

    previous_frame_time = SDL_GetTicks();

    triangles_to_render = NULL;

    // mesh.rotation.x += 0.01;
    mesh.rotation.y += 0.01;
    // mesh.rotation.z += 0.005;

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
        face_vertices[0] = mesh.vertices[mesh_face.a];
        face_vertices[1] = mesh.vertices[mesh_face.b];
        face_vertices[2] = mesh.vertices[mesh_face.c];

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

        if(cull_method == CULL_BACKFACE){
            if(dot_normal_camera < 0){
                continue;
            }
        }


        vec4_t projected_points[3];

        // projection loop for the 3 vertices
        for(int j = 0; j < 3; ++j){
            projected_points[j] = mat4_mul_vec4_project(projection_matrix, transformed_vertices[j]);

            // scaling into the view
            projected_points[j].x *= (window_width / 2.0);
            projected_points[j].y *= (window_height / 2.0);

            // inverting the Y values to account for flipped screen Y coordinate
            projected_points[j].y *= -1;

            // translating the projected_points to the middle of the screen
            projected_points[j].x += (window_width / 2.0);
            projected_points[j].y += (window_height / 2.0);
        }
        
        // calucating the average depth for each face based on the 
        // vertices after transformation
        float avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3.0; 

        float light_intensity_factor = -1 * vec3_dot(vector_normal, light.direction);

        // calculating the triangle color based on the light angle
        uint32_t triangle_color = light_apply_intensity(mesh_face.color, light_intensity_factor);

        triangle_t projected_triangle = {
            .points = {
                { projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w },
                { projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w },
                { projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w },
            },
            .tex_coords = {
                { mesh_face.a_uv.u, mesh_face.a_uv.v },
                { mesh_face.b_uv.u, mesh_face.b_uv.v },
                { mesh_face.c_uv.u, mesh_face.c_uv.v },
            },
            .color = triangle_color,
            .avg_depth = avg_depth 
        };
        array_push(triangles_to_render, projected_triangle);
    }

    // sorting the triangles to render by their average depth
    int num_of_triangles = array_length(triangles_to_render);
    sort_triangles(triangles_to_render, num_of_triangles);
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

        if(render_method == RENDER_TEXTURED || render_method == RENDER_TEXTURED_WIRE){
            // drawing textured triangle
            draw_textured_triangle(
                triangle.points[0].x, 
                triangle.points[0].y,
                triangle.points[0].z,
                triangle.points[0].w,
                triangle.tex_coords[0].u,
                triangle.tex_coords[0].v,
                triangle.points[1].x,
                triangle.points[1].y,
                triangle.points[1].z,
                triangle.points[1].w,
                triangle.tex_coords[1].u,
                triangle.tex_coords[1].v,
                triangle.points[2].x,
                triangle.points[2].y,
                triangle.points[2].z,
                triangle.points[2].w,
                triangle.tex_coords[2].u,
                triangle.tex_coords[2].v,
                mesh_texture
            );
        }

        if(render_method == RENDER_WIRE || render_method == RENDER_WIRE_VERTEX || render_method == RENDER_FILL_TRIANGLE_WIRE || render_method == RENDER_TEXTURED_WIRE){
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
    clear_z_buffer();

    // this function updates the screen with any rendering performed  
    // since the previous call. it swaps the back buffer with the fron buffer,
    // displaying the current rendering result on the screen.
    SDL_RenderPresent(renderer);
}

void free_resources(void){
    free(color_buffer);
    free(z_buffer);
    upng_free(png_texture);
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
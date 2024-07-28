#include "headers/display.h"

// GLOBAL VARIABLES
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

void draw_grid(void){
    for(int y = 0; y < window_height; y += 20){
        for(int x = 0; x < window_width; x += 20){
            color_buffer[(window_width * y) + x] = 0xFF333333;
        }
    }
}

void draw_pixel(int x, int y, uint32_t color){
    if(x < 0 || y < 0 || x >= window_width || y >= window_height) return;

    color_buffer[(window_width * y) + x] = color;
}

void draw_rect(int x, int y, int width, int height, uint32_t color){
    for(int i = 0; i <= height; ++i){
        for(int j = 0; j <= width; ++j){
            int current_x = x + i;
            int current_y = y + j;
            draw_pixel(current_x, current_y, color);
            // color_buffer[(window_width * (i + y)) + (j + x)] = color;
        }
    }
}

void draw_line(int x1, int y1, int x2, int y2, uint32_t color){
    int delta_x = (x2 - x1);
    int delta_y = (y2 - y1);

    int longest_side_length = (abs(delta_x) >= abs(delta_y)) ? abs(delta_x) : abs(delta_y);

    float x_inc = delta_x / (float)longest_side_length;
    float y_inc = delta_y / (float)longest_side_length;

    float current_x = x1;  
    float current_y = y1;
    for(int i = 0; i <= longest_side_length; ++i){
        draw_pixel(round(current_x), round(current_y), color);
        current_x += x_inc;
        current_y += y_inc;
    }     
}

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color){
    draw_line(x0, y0, x1, y1, color);
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x0, y0, color);
}

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
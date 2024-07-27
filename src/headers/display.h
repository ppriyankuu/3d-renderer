#ifndef DISPLAY_H // if not defined (DISPLAY_H)
#define DISPLAY_H // then define (DISPLAY_H); pretty much like singleton pattern

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define FPS 30
#define FRAME_TARGET_TIME (1000 / FPS) // how long should 1 frame take (in mili-secs)

// GLOBAL VARIABLES
// 'extern' is used to declare a variable or function
// that is defined in some other file (external file) 
extern SDL_Window* window; 
extern SDL_Renderer* renderer;

extern uint32_t* color_buffer;
extern SDL_Texture* color_buffer_texture;

extern int window_width;
extern int window_height;

// FUNCTION SIGNATURES
bool initialize_window(void);

void draw_pixel(int x, int y, uint32_t color);
void draw_grid(void);
void draw_rect(int x, int y, int width, int height, uint32_t color);

void render_color_buffer(void);
void clear_color_buffer(uint32_t color);

void destroy_window(void);

#endif
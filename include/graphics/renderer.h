#ifndef RENDERER_H
#define RENDERER_H

#include "core/engine.h"
#include "graphics/camera.h"

typedef struct Renderer {
    SDL_Window* window;
    SDL_Renderer* sdl_renderer;
    Camera* camera;
    Color clear_color;
} Renderer;

Renderer* renderer_create(const char* title, int width, int height);
void renderer_destroy(Renderer* renderer);
void renderer_set_camera(Renderer* renderer, Camera* camera);
void renderer_set_clear_color(Renderer* renderer, Color color);
void renderer_begin_frame(Renderer* renderer);
void renderer_end_frame(Renderer* renderer);
SDL_Renderer* renderer_get_sdl_renderer(Renderer* renderer);

#endif



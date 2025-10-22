#include "graphics/renderer.h"

Renderer* renderer_create(const char* title, int width, int height) {
    Renderer* renderer = (Renderer*)calloc(1, sizeof(Renderer));
    if (!renderer) return NULL;

    renderer->window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_RESIZABLE
    );

    if (!renderer->window) {
        fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
        free(renderer);
        return NULL;
    }

    renderer->sdl_renderer = SDL_CreateRenderer(
        renderer->window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!renderer->sdl_renderer) {
        fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(renderer->window);
        free(renderer);
        return NULL;
    }

    renderer->clear_color.r = 100;
    renderer->clear_color.g = 150;
    renderer->clear_color.b = 255;
    renderer->clear_color.a = 255;
    renderer->camera = NULL;

    return renderer;
}

void renderer_destroy(Renderer* renderer) {
    if (!renderer) return;

    if (renderer->sdl_renderer) {
        SDL_DestroyRenderer(renderer->sdl_renderer);
    }
    if (renderer->window) {
        SDL_DestroyWindow(renderer->window);
    }

    free(renderer);
}

void renderer_set_camera(Renderer* renderer, Camera* camera) {
    if (renderer) {
        renderer->camera = camera;
    }
}

void renderer_set_clear_color(Renderer* renderer, Color color) {
    if (renderer) {
        renderer->clear_color = color;
    }
}

void renderer_begin_frame(Renderer* renderer) {
    if (!renderer) return;
    SDL_SetRenderDrawColor(
        renderer->sdl_renderer,
        renderer->clear_color.r,
        renderer->clear_color.g,
        renderer->clear_color.b,
        renderer->clear_color.a
    );
    SDL_RenderClear(renderer->sdl_renderer);
}

void renderer_end_frame(Renderer* renderer) {
    if (!renderer) return;
    SDL_RenderPresent(renderer->sdl_renderer);
}

SDL_Renderer* renderer_get_sdl_renderer(Renderer* renderer) {
    return renderer ? renderer->sdl_renderer : NULL;
}



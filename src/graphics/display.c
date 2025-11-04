#include "display.h"
#include <SDL2/SDL.h>
#include <engine/input.h>
#include <stdio.h>
#include <stdlib.h>

struct Display {
  SDL_Window *window;
  // Used only for displaying, doesn't really render anything
  SDL_Renderer *renderer;
  SDL_Texture *texture;
  int width;
  int height;
  uint32_t last_frame_time;
  float fps;
};

Display *display_create(int width, int height, float scale, const char *title) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
    fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
    return NULL;
  }

  Display *d = calloc(1, sizeof(Display));
  if (!d) {
    SDL_Quit();
    return NULL;
  }

  d->width = width;
  d->height = height;
  d->last_frame_time = SDL_GetTicks();
  d->fps = 0.0f;

  d->window = SDL_CreateWindow(title,
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      width * scale,
      height * scale,
      SDL_WINDOW_SHOWN);
  if (!d->window) {
    fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
    free(d);
    SDL_Quit();
    return NULL;
  }

  d->renderer = SDL_CreateRenderer(d->window, -1, SDL_RENDERER_ACCELERATED);
  if (!d->renderer) {
    fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
    SDL_DestroyWindow(d->window);
    free(d);
    SDL_Quit();
    return NULL;
  }

  d->texture = SDL_CreateTexture(d->renderer,
      SDL_PIXELFORMAT_ARGB8888,
      SDL_TEXTUREACCESS_STREAMING,
      width,
      height);
  if (!d->texture) {
    fprintf(stderr, "SDL_CreateTexture Error: %s\n", SDL_GetError());
    SDL_DestroyRenderer(d->renderer);
    SDL_DestroyWindow(d->window);
    free(d);
    SDL_Quit();
    return NULL;
  }

  return d;
}

void display_free(Display *d) {
  if (!d) return;

  if (d->texture) SDL_DestroyTexture(d->texture);
  if (d->renderer) SDL_DestroyRenderer(d->renderer);
  if (d->window) SDL_DestroyWindow(d->window);

  free(d);
  SDL_Quit();
}

// Poll events and send them to input entity
bool display_poll_events(Input *input) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT: input->quit = true; return false;

    case SDL_KEYDOWN:
      if (event.key.repeat) break; // ignore key repeat evenets
      switch (event.key.keysym.scancode) {
      case SDL_SCANCODE_W: input->w = true; break;
      case SDL_SCANCODE_A: input->a = true; break;
      case SDL_SCANCODE_S: input->s = true; break;
      case SDL_SCANCODE_D: input->d = true; break;
      case SDL_SCANCODE_ESCAPE: input->quit = true; break;
      default: break;
      }
      break;

    case SDL_KEYUP:
      switch (event.key.keysym.scancode) {
      case SDL_SCANCODE_W: input->w = false; break;
      case SDL_SCANCODE_A: input->a = false; break;
      case SDL_SCANCODE_S: input->s = false; break;
      case SDL_SCANCODE_D: input->d = false; break;
      case SDL_SCANCODE_ESCAPE: input->quit = false; break;
      default: break;
      }
      break;
    }
  }
  return !input->quit;
}

// Display the given frame buffer
void display_present(Display *d, const uint32_t *pixels) {
  if (!d || !pixels) return;

  // Update FPS
  uint32_t current_time = SDL_GetTicks();
  uint32_t elapsed = current_time - d->last_frame_time;
  if (elapsed > 0) { d->fps = 1000.0f / (float)elapsed; }
  d->last_frame_time = current_time;

  // pixels (RAM) -> texture (VRAM)
  SDL_UpdateTexture(d->texture, NULL, pixels, d->width * sizeof(uint32_t));
  // Clear Backbuffer
  SDL_RenderClear(d->renderer);
  // Copy texture -> Backbuffer
  SDL_RenderCopy(d->renderer, d->texture, NULL, NULL);
  // Swap: Backbuffer -> Frontbuffer (screen)
  SDL_RenderPresent(d->renderer);
}

float display_get_fps(Display *d) {
  return d ? d->fps : 0.0f;
}

uint64_t display_get_ticks() {
  return SDL_GetTicks64();
}

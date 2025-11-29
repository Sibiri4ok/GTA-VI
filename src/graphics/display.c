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
  uint64_t last_frame_time;
  uint64_t delta_time;
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
  d->last_frame_time = SDL_GetTicks64();
  d->delta_time = 0.0f;

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

  d->texture =
      SDL_CreateTexture(d->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
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

void display_present(Display *d, const uint32_t *pixels) {
  if (!d || !pixels) return;

  // Update FPS
  uint64_t current_time = SDL_GetTicks64();
  uint64_t elapsed = current_time - d->last_frame_time;
  if (elapsed > 0) { d->delta_time = elapsed; }
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

uint64_t display_get_delta_time(Display *d) {
  return d ? d->delta_time : 0;
}

uint64_t display_get_last_frame_time(Display *d) {
  return d ? d->last_frame_time : 0;
}

uint64_t display_get_ticks() {
  return SDL_GetTicks64();
}

bool display_poll_events(Input *input) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT: input->quit = true; return false;

    case SDL_KEYDOWN:
      if (event.key.repeat) break; // ignore key repeat events
      switch (event.key.keysym.scancode) {
      case SDL_SCANCODE_UP: input->up = true; break;
      case SDL_SCANCODE_DOWN: input->down = true; break;
      case SDL_SCANCODE_LEFT: input->left = true; break;
      case SDL_SCANCODE_RIGHT: input->right = true; break;
      case SDL_SCANCODE_Q: input->q = true; break;
      case SDL_SCANCODE_W: input->w = true; break;
      case SDL_SCANCODE_E: input->e = true; break;
      case SDL_SCANCODE_R: input->r = true; break;
      case SDL_SCANCODE_T: input->t = true; break;
      case SDL_SCANCODE_Y: input->y = true; break;
      case SDL_SCANCODE_U: input->u = true; break;
      case SDL_SCANCODE_I: input->i = true; break;
      case SDL_SCANCODE_O: input->o = true; break;
      case SDL_SCANCODE_P: input->p = true; break;
      case SDL_SCANCODE_A: input->a = true; break;
      case SDL_SCANCODE_S: input->s = true; break;
      case SDL_SCANCODE_D: input->d = true; break;
      case SDL_SCANCODE_F: input->f = true; break;
      case SDL_SCANCODE_G: input->g = true; break;
      case SDL_SCANCODE_H: input->h = true; break;
      case SDL_SCANCODE_J: input->j = true; break;
      case SDL_SCANCODE_K: input->k = true; break;
      case SDL_SCANCODE_L: input->l = true; break;
      case SDL_SCANCODE_Z: input->z = true; break;
      case SDL_SCANCODE_X: input->x = true; break;
      case SDL_SCANCODE_C: input->c = true; break;
      case SDL_SCANCODE_V: input->v = true; break;
      case SDL_SCANCODE_B: input->b = true; break;
      case SDL_SCANCODE_N: input->n = true; break;
      case SDL_SCANCODE_M: input->m = true; break;
      case SDL_SCANCODE_SPACE: input->space = true; break;
      case SDL_SCANCODE_ESCAPE: input->quit = true; break;
      case SDL_SCANCODE_LCTRL: input->lctrl = true; break;
      case SDL_SCANCODE_LSHIFT: input->lshift = true; break;
      case SDL_SCANCODE_RETURN: input->enter = true; break;
      default: break;
      }
      break;

    case SDL_KEYUP:
      switch (event.key.keysym.scancode) {
      case SDL_SCANCODE_UP: input->up = false; break;
      case SDL_SCANCODE_DOWN: input->down = false; break;
      case SDL_SCANCODE_LEFT: input->left = false; break;
      case SDL_SCANCODE_RIGHT: input->right = false; break;
      case SDL_SCANCODE_Q: input->q = false; break;
      case SDL_SCANCODE_W: input->w = false; break;
      case SDL_SCANCODE_E: input->e = false; break;
      case SDL_SCANCODE_R: input->r = false; break;
      case SDL_SCANCODE_T: input->t = false; break;
      case SDL_SCANCODE_Y: input->y = false; break;
      case SDL_SCANCODE_U: input->u = false; break;
      case SDL_SCANCODE_I: input->i = false; break;
      case SDL_SCANCODE_O: input->o = false; break;
      case SDL_SCANCODE_P: input->p = false; break;
      case SDL_SCANCODE_A: input->a = false; break;
      case SDL_SCANCODE_S: input->s = false; break;
      case SDL_SCANCODE_D: input->d = false; break;
      case SDL_SCANCODE_F: input->f = false; break;
      case SDL_SCANCODE_G: input->g = false; break;
      case SDL_SCANCODE_H: input->h = false; break;
      case SDL_SCANCODE_J: input->j = false; break;
      case SDL_SCANCODE_K: input->k = false; break;
      case SDL_SCANCODE_L: input->l = false; break;
      case SDL_SCANCODE_Z: input->z = false; break;
      case SDL_SCANCODE_X: input->x = false; break;
      case SDL_SCANCODE_C: input->c = false; break;
      case SDL_SCANCODE_V: input->v = false; break;
      case SDL_SCANCODE_B: input->b = false; break;
      case SDL_SCANCODE_N: input->n = false; break;
      case SDL_SCANCODE_M: input->m = false; break;
      case SDL_SCANCODE_SPACE: input->space = false; break;
      case SDL_SCANCODE_ESCAPE: input->quit = false; break;
      case SDL_SCANCODE_RETURN: input->enter = false; break;
      case SDL_SCANCODE_LCTRL: input->lctrl = false; break;
      case SDL_SCANCODE_LSHIFT: input->lshift = false; break;
      default: break;
      }
      break;
    }
  }
  return !input->quit;
}

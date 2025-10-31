#include "app/app.h"
#include "core/engine.h"

App *app_create(const char *title, int width, int height) {
  App *app = (App *)calloc(1, sizeof(App));
  if (!app) return NULL;

  app->state = APP_RUNNING;
  app->running = true;
  app->last_frame_time = 0;
  app->delta_time = 0.0f;
  app->fps = 0.0f;

  app->renderer = renderer_create(title, width, height);
  if (!app->renderer) {
    free(app);
    return NULL;
  }

  app->camera = camera_create((float)width, (float)height);
  if (!app->camera) {
    renderer_destroy(app->renderer);
    free(app);
    return NULL;
  }

  renderer_set_camera(app->renderer, app->camera);

  app->map = map_create(20, 20, ISO_TILE_WIDTH);
  if (!app->map) {
    camera_destroy(app->camera);
    renderer_destroy(app->renderer);
    free(app);
    return NULL;
  }

  map_create_tile_textures(app->map, renderer_get_sdl_renderer(app->renderer));

  Vector2 center_tile = {(float)(app->map->width / 2), (float)(app->map->height / 2)};
  app->player_position = iso_tile_to_world((int)center_tile.x, (int)center_tile.y);
  app->player_velocity.x = 0.0f;
  app->player_velocity.y = 0.0f;
  app->player_texture =
      IMG_LoadTexture(renderer_get_sdl_renderer(app->renderer), "assets/mario.png");
  app->player_texture_down =
      IMG_LoadTexture(renderer_get_sdl_renderer(app->renderer), "assets/mario_back.png");
  app->player_texture_forward =
      IMG_LoadTexture(renderer_get_sdl_renderer(app->renderer), "assets/mario_forward.png");

  if (!app->player_texture) {
    fprintf(stderr, "Failed to load player texture: %s\n", IMG_GetError());
  }
  if (!app->player_texture_down) {
    fprintf(stderr, "Failed to load player down texture: %s\n", IMG_GetError());
  }
  if (!app->player_texture_forward) {
    fprintf(stderr, "Failed to load player forward texture: %s\n", IMG_GetError());
  }

  app->camera->position.x = app->player_position.x;
  app->camera->position.y = app->player_position.y;
  camera_set_target(app->camera, app->player_position);
  camera_set_following(app->camera, true);
  camera_set_world_bounds(app->camera, app->map->world_size);

  return app;
}

void app_destroy(App *app) {
  if (!app) return;

  if (app->player_texture) { SDL_DestroyTexture(app->player_texture); }
  if (app->player_texture_down) { SDL_DestroyTexture(app->player_texture_down); }
  if (app->player_texture_forward) { SDL_DestroyTexture(app->player_texture_forward); }

  if (app->map) { map_destroy(app->map); }

  if (app->camera) { camera_destroy(app->camera); }

  if (app->renderer) { renderer_destroy(app->renderer); }

  free(app);
}

bool app_init(App *app) {
  if (!app) return false;
  if (!engine_init()) return false;
  return true;
}

void app_handle_events(App *app) {
  if (!app) return;

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT: app->running = false; break;

    case SDL_KEYDOWN:
      switch (event.key.keysym.sym) {
      case SDLK_ESCAPE: app->running = false; break;

      case SDLK_UP:
      case SDLK_w: app->player_velocity.y = -1.0f; break;

      case SDLK_DOWN:
      case SDLK_s: app->player_velocity.y = 1.0f; break;

      case SDLK_LEFT:
      case SDLK_a: app->player_velocity.x = -1.0f; break;

      case SDLK_RIGHT:
      case SDLK_d: app->player_velocity.x = 1.0f; break;

      default: break;
      }
      break;

    case SDL_KEYUP:
      switch (event.key.keysym.sym) {
      case SDLK_UP:
      case SDLK_w:
        if (app->player_velocity.y < 0) app->player_velocity.y = 0.0f;
        break;

      case SDLK_DOWN:
      case SDLK_s:
        if (app->player_velocity.y > 0) app->player_velocity.y = 0.0f;
        break;

      case SDLK_LEFT:
      case SDLK_a:
        if (app->player_velocity.x < 0) app->player_velocity.x = 0.0f;
        break;

      case SDLK_RIGHT:
      case SDLK_d:
        if (app->player_velocity.x > 0) app->player_velocity.x = 0.0f;
        break;

      default: break;
      }
      break;

    default: break;
    }
  }
}

void app_update(App *app) {
  if (!app || app->state != APP_RUNNING) return;

  uint32_t current_time = SDL_GetTicks();
  app->delta_time = (current_time - app->last_frame_time) / 1000.0f;
  app->last_frame_time = current_time;

  if (app->delta_time > 0.1f) { app->delta_time = 0.1f; }

  static float last_valid_x = 0;
  static float last_valid_y = 0;
  static bool initialized = false;

  if (!initialized) {
    last_valid_x = app->player_position.x;
    last_valid_y = app->player_position.y;
    initialized = true;
  }

  float speed = 120.0f;
  app->player_position.x += app->player_velocity.x * speed * app->delta_time;
  app->player_position.y += app->player_velocity.y * speed * app->delta_time;

  Vector2 tile_pos = iso_world_to_tile(app->player_position);
  const float eps = 0.001f;
  int tile_x = (int)tile_pos.x;
  int tile_y = (int)tile_pos.y;
  int tile_x_right = (int)ceilf(tile_pos.x - eps);
  int tile_y_top = (int)ceilf(tile_pos.y - eps);

  if (tile_x < 0 || tile_y < 0 || tile_x_right >= app->map->width ||
      tile_y_top >= app->map->height) {
    app->player_position.x = last_valid_x;
    app->player_position.y = last_valid_y;
  } else {
    last_valid_x = app->player_position.x;
    last_valid_y = app->player_position.y;
  }

  camera_set_target(app->camera, app->player_position);
  camera_update(app->camera, app->delta_time);
  engine_update(app->delta_time);
  app->fps = engine_get_fps();
}

void app_render(App *app) {
  if (!app) return;

  renderer_begin_frame(app->renderer);

  if (app->map) { map_render(app->map, renderer_get_sdl_renderer(app->renderer), app->camera); }

  {
    Vector2 screen_pos = camera_world_to_screen(app->camera, app->player_position);
    SDL_Renderer *renderer = renderer_get_sdl_renderer(app->renderer);

    if (app->player_texture) {
      SDL_Texture *current_texture = app->player_texture;
      float scale = 1.0f;
      if (app->player_velocity.y > 0 && app->player_texture_down) {
        current_texture = app->player_texture_down;
        scale = 0.85f;
      } else if (app->player_velocity.y < 0 && app->player_texture_forward) {
        current_texture = app->player_texture_forward;
        scale = 0.85f;
      }

      int base_width = 128;
      int base_height = 128;
      int sprite_width = (int)(base_width * scale);
      int sprite_height = (int)(base_height * scale);
      int half_width = sprite_width / 2;
      int half_height = sprite_height / 2;

      SDL_Rect dst_rect = {(int)screen_pos.x - half_width,
          (int)screen_pos.y - half_height,
          sprite_width,
          sprite_height};

      SDL_Point center = {half_width, half_height};
      SDL_RendererFlip flip = SDL_FLIP_NONE;
      if (app->player_velocity.x < 0) { flip = SDL_FLIP_HORIZONTAL; }

      SDL_RenderCopyEx(renderer, current_texture, NULL, &dst_rect, 0, &center, flip);
    } else {
      SDL_Rect player_rect = {(int)screen_pos.x - 10, (int)screen_pos.y - 10, 20, 20};

      SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
      SDL_RenderFillRect(renderer, &player_rect);

      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      SDL_RenderDrawRect(renderer, &player_rect);
    }
  }

  renderer_end_frame(app->renderer);
}

void app_run(App *app) {
  if (!app) return;

  app->last_frame_time = SDL_GetTicks();

  while (app->running) {
    app_handle_events(app);
    app_update(app);
    app_render(app);
    SDL_Delay(1000 / TARGET_FPS);
  }
}

void app_quit(App *app) {
  if (app) {
    app->running = false;
    app->state = APP_QUITTING;
  }
}

float app_get_delta_time(const App *app) {
  return app ? app->delta_time : 0.0f;
}

float app_get_fps(const App *app) {
  return app ? app->fps : 0.0f;
}

bool app_is_running(const App *app) {
  return app ? app->running : false;
}

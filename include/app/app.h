#ifndef APP_H
#define APP_H

#include "core/engine.h"
#include "graphics/renderer.h"
#include "graphics/camera.h"
#include "game/map.h"

typedef enum {
    APP_RUNNING,
    APP_PAUSED,
    APP_QUITTING
} AppState;

typedef struct App {
    AppState state;
    bool running;
    uint32_t last_frame_time;
    float delta_time;
    float fps;
    Renderer* renderer;
    Camera* camera;
    Map* map;
    SDL_Texture* player_texture;
    SDL_Texture* player_texture_down;
    SDL_Texture* player_texture_forward;
    Vector2 player_position;
    Vector2 player_velocity;
} App;

App* app_create(const char* title, int width, int height);
void app_destroy(App* app);
bool app_init(App* app);
void app_run(App* app);
void app_quit(App* app);
void app_update(App* app);
void app_render(App* app);
void app_handle_events(App* app);
float app_get_delta_time(const App* app);
float app_get_fps(const App* app);
bool app_is_running(const App* app);

#endif



/**
 * @file app.c
 * @brief Реализация главного приложения и игрового цикла
 */

#include "app.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

App* app_create(const char* title, int width, int height) {
    if (!title) {
        return NULL;
    }
    
    App* app = (App*)calloc(1, sizeof(App));
    if (!app) {
        return NULL;
    }
    
    app->state = APP_RUNNING;
    app->running = true;
    app->last_frame_time = 0;
    app->delta_time = 0.0f;
    app->fps = 0.0f;
    
    // Создаем рендерер
    app->renderer = renderer_create(title, width, height);
    if (!app->renderer) {
        free(app);
        return NULL;
    }
    
    // Создаем камеру
    app->camera = camera_create(CAMERA_WIDTH, CAMERA_HEIGHT);
    if (!app->camera) {
        renderer_destroy(app->renderer);
        free(app);
        return NULL;
    }
    
    // Инициализируем игровые объекты
    app->player = NULL;
    app->player_position.x = 100.0f;
    app->player_position.y = 100.0f;
    app->player_velocity.x = 0.0f;
    app->player_velocity.y = 0.0f;
    
    return app;
}

void app_destroy(App* app) {
    if (!app) {
        return;
    }
    
    if (app->player) {
        sprite_destroy(app->player);
    }
    
    if (app->map) {
        map_destroy(app->map);
    }
    
    if (app->camera) {
        camera_destroy(app->camera);
    }
    
    if (app->renderer) {
        renderer_destroy(app->renderer);
    }
    
    free(app);
}

bool app_init(App* app) {
    if (!app) {
        return false;
    }
    
    // Создаем карту
    app->map = map_create(50, 50, 32); // 50x50 тайлов по 32 пикселя
    if (!app->map) {
        printf("Ошибка создания карты\n");
        return false;
    }
    
    // Загружаем карту из PNG (если файл существует)
    if (!map_load_from_png(app->map, renderer_get_sdl_renderer(app->renderer), "assets/map.png")) {
        printf("Не удалось загрузить карту из PNG, создаем простую карту\n");
        // Создаем простую карту программно
        for (int y = 0; y < app->map->height; y++) {
            for (int x = 0; x < app->map->width; x++) {
                TileType type = TILE_GROUND;
                if (x == 0 || x == app->map->width - 1 || y == 0 || y == app->map->height - 1) {
                    type = TILE_WALL; // Границы - стены
                } else if ((x + y) % 10 == 0) {
                    type = TILE_SHADOW; // Некоторые тайлы - тени
                }
                map_set_tile(app->map, x, y, type);
            }
        }
        map_create_tile_textures(app->map, renderer_get_sdl_renderer(app->renderer));
    }
    
    // Создаем персонажа
    app->player = sprite_create(renderer_get_sdl_renderer(app->renderer), "assets/player.png");
    if (!app->player) {
        printf("Не удалось загрузить спрайт игрока, создаем простой квадрат\n");
        // Создаем простой спрайт программно
        SDL_Surface* surface = SDL_CreateRGBSurface(0, 32, 32, 32, 0, 0, 0, 0);
        SDL_FillRect(surface, NULL, SDL_MapRGBA(surface->format, 255, 0, 0, 255)); // Красный квадрат
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_get_sdl_renderer(app->renderer), surface);
        SDL_FreeSurface(surface);
        
        app->player = (Sprite*)calloc(1, sizeof(Sprite));
        app->player->texture = texture;
        app->player->size.x = 32.0f;
        app->player->size.y = 32.0f;
        app->player->type = SPRITE_DYNAMIC;
        app->player->visible = true;
    }
    
    // Настраиваем камеру
    camera_set_world_bounds(app->camera, app->map->world_size);
    camera_set_target(app->camera, app->player_position);
    camera_set_follow_speed(app->camera, 12.0f); // Увеличиваем скорость следования
    
    // Настраиваем рендерер
    renderer_set_camera(app->renderer, app->camera);
    Color clear_color = {30, 30, 30, 255}; // Темно-серый фон
    renderer_set_clear_color(app->renderer, clear_color);
    
    // Добавляем спрайты в рендерер
    renderer_add_sprite(app->renderer, app->player);
    
    return true;
}

void app_run(App* app) {
    if (!app) {
        return;
    }
    
    uint32_t current_time = SDL_GetTicks();
    app->last_frame_time = current_time;
    
    while (app->running) {
        current_time = SDL_GetTicks();
        app->delta_time = (current_time - app->last_frame_time) / 1000.0f;
        app->last_frame_time = current_time;
        
        // Ограничиваем delta_time для стабильности
        if (app->delta_time > 0.1f) {
            app->delta_time = 0.1f;
        }
        
        // Обновляем FPS
        if (app->delta_time > 0) {
            app->fps = 1.0f / app->delta_time;
        }
        
        app_handle_events(app);
        app_update(app);
        app_render(app);
        
        // Ограничиваем FPS до минимум 12 FPS
        uint32_t frame_time = SDL_GetTicks() - current_time;
        if (frame_time < 83) { // 1000ms / 12fps = 83ms
            SDL_Delay(83 - frame_time);
        }
    }
}

void app_quit(App* app) {
    if (app) {
        app->running = false;
        app->state = APP_QUITTING;
    }
}

void app_update(App* app) {
    if (!app || app->state != APP_RUNNING) {
        return;
    }
    
    // Обновляем позицию игрока
    app->player_position.x += app->player_velocity.x * app->delta_time;
    app->player_position.y += app->player_velocity.y * app->delta_time;
    
    // Ограничиваем позицию игрока границами карты
    if (app->player_position.x < 0) app->player_position.x = 0;
    if (app->player_position.y < 0) app->player_position.y = 0;
    if (app->player_position.x > app->map->world_size.x - 32) {
        app->player_position.x = app->map->world_size.x - 32;
    }
    if (app->player_position.y > app->map->world_size.y - 32) {
        app->player_position.y = app->map->world_size.y - 32;
    }
    
    // Обновляем позицию спрайта игрока
    sprite_set_position(app->player, app->player_position.x, app->player_position.y);
    
    // Проверяем состояние освещения игрока
    Vector2 tile_pos = map_world_to_tile(app->map, app->player_position);
    TileType tile_type = map_get_tile(app->map, (int)tile_pos.x, (int)tile_pos.y);
    LightState light_state = (tile_type == TILE_SHADOW) ? LIGHT_SHADOW : LIGHT_BRIGHT;
    sprite_set_light_state(app->player, light_state);
    
    // Обновляем камеру
    camera_set_target(app->camera, app->player_position);
    camera_update(app->camera, app->delta_time);
}

void app_render(App* app) {
    if (!app) {
        return;
    }
    
    renderer_begin_frame(app->renderer);
    
    // Рендерим карту в задний буфер
    map_render(app->map, renderer_get_sdl_renderer(app->renderer), app->camera);
    
    // Рендерим спрайты в тот же буфер
    renderer_render_sprites(app->renderer);
    
    renderer_end_frame(app->renderer);
}

void app_handle_events(App* app) {
    if (!app) {
        return;
    }
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                app_quit(app);
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDL_QUIT:
                    case SDLK_ESCAPE:
                        app_quit(app);
                        break;
                        
                    case SDLK_LEFT:
                    case SDLK_a:
                        app->player_velocity.x = -150.0f;
                        break;
                        
                    case SDLK_RIGHT:
                    case SDLK_d:
                        app->player_velocity.x = 150.0f;
                        break;
                        
                    case SDLK_UP:
                    case SDLK_w:
                        app->player_velocity.y = -150.0f;
                        break;
                        
                    case SDLK_DOWN:
                    case SDLK_s:
                        app->player_velocity.y = 150.0f;
                        break;
                }
                break;
                
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                    case SDLK_a:
                    case SDLK_RIGHT:
                    case SDLK_d:
                        app->player_velocity.x = 0.0f;
                        break;
                        
                    case SDLK_UP:
                    case SDLK_w:
                    case SDLK_DOWN:
                    case SDLK_s:
                        app->player_velocity.y = 0.0f;
                        break;
                }
                break;
        }
    }
}

float app_get_delta_time(const App* app) {
    return app ? app->delta_time : 0.0f;
}

float app_get_fps(const App* app) {
    return app ? app->fps : 0.0f;
}

bool app_is_running(const App* app) {
    return app ? app->running : false;
}

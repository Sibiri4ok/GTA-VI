
#include "app/app.h"
#include <stdio.h>

void create_demo_scene(App* app) {
    if (!app || !app->map) return;

    // Дороги
    for (int y = 0; y < app->map->height; y++) {
        map_set_tile(app->map, 5, y, TILE_ROAD_NS);
        map_set_tile(app->map, 10, y, TILE_ROAD_NS);
    }
    
    for (int x = 0; x < app->map->width; x++) {
        map_set_tile(app->map, x, 5, TILE_ROAD_EW);
        map_set_tile(app->map, x, 10, TILE_ROAD_EW);
    }

    // Перекрестки
    map_set_tile(app->map, 5, 5, TILE_ROAD_CROSS);
    map_set_tile(app->map, 5, 10, TILE_ROAD_CROSS);
    map_set_tile(app->map, 10, 5, TILE_ROAD_CROSS);
    map_set_tile(app->map, 10, 10, TILE_ROAD_CROSS);

    // Здания (жилые)
    map_set_tile(app->map, 2, 2, TILE_BUILDING_RESIDENTIAL);
    map_set_tile(app->map, 3, 2, TILE_BUILDING_RESIDENTIAL);
    map_set_tile(app->map, 2, 3, TILE_BUILDING_RESIDENTIAL);
    map_set_tile(app->map, 3, 3, TILE_BUILDING_RESIDENTIAL);

    map_set_tile(app->map, 7, 2, TILE_BUILDING_RESIDENTIAL);
    map_set_tile(app->map, 8, 2, TILE_BUILDING_RESIDENTIAL);

    // Здания (коммерческие)
    map_set_tile(app->map, 12, 2, TILE_BUILDING_COMMERCIAL);
    map_set_tile(app->map, 13, 2, TILE_BUILDING_COMMERCIAL);
    map_set_tile(app->map, 12, 3, TILE_BUILDING_COMMERCIAL);
    map_set_tile(app->map, 13, 3, TILE_BUILDING_COMMERCIAL);

    // Промышленные объекты
    map_set_tile(app->map, 15, 15, TILE_BUILDING_INDUSTRIAL);
    map_set_tile(app->map, 16, 15, TILE_BUILDING_INDUSTRIAL);
    map_set_tile(app->map, 15, 16, TILE_BUILDING_INDUSTRIAL);
    map_set_tile(app->map, 16, 16, TILE_BUILDING_INDUSTRIAL);

    // Общественные здания
    map_set_tile(app->map, 7, 13, TILE_BUILDING_PUBLIC);
    map_set_tile(app->map, 8, 13, TILE_BUILDING_PUBLIC);
    map_set_tile(app->map, 7, 14, TILE_BUILDING_PUBLIC);
    map_set_tile(app->map, 8, 14, TILE_BUILDING_PUBLIC);

    // Водоем
    map_set_tile(app->map, 17, 3, TILE_WATER);
    map_set_tile(app->map, 18, 3, TILE_WATER);
    map_set_tile(app->map, 17, 4, TILE_WATER);
    map_set_tile(app->map, 18, 4, TILE_WATER);
}

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    printf("=== GTA 6 - Isometric Edition ===\n");
    printf("Version: %s\n", ENGINE_VERSION);
    printf("Loading game...\n\n");

    App* app = app_create("GTA 6", 800, 600);
    if (!app) {
        fprintf(stderr, "Failed to create application\n");
        return 1;
    }

    if (!app_init(app)) {
        fprintf(stderr, "Failed to initialize application\n");
        app_destroy(app);
        return 1;
    }

    printf("Loading city...\n");
    create_demo_scene(app);

    printf("Starting game...\n\n");
    printf("Controls:\n");
    printf("  WASD or Arrow Keys - Move\n");
    printf("  ESC - Exit\n\n");

    camera_set_world_bounds(app->camera, app->map->world_size);
    
    app_run(app);

    printf("Cleaning up...\n");
    app_destroy(app);
    
    engine_cleanup();

    printf("Exiting...\n");
    return 0;
}



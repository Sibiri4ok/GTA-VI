/**
 * @file main.c
 * @brief Демо-приложение для 2D игрового движка
 * 
 * Это простое демо-приложение демонстрирует возможности движка:
 * - Загрузку и отображение карты
 * - Управление персонажем
 * - Следование камеры за персонажем
 * - Систему теней и освещения
 * - Двойную буферизацию
 */

#include "engine.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    printf("=== 2D Игровой Движок v%s ===\n", ENGINE_VERSION);
    printf("Демо-приложение запущено\n");
    printf("Управление: WASD или стрелки для движения\n");
    printf("ESC или закрытие окна для выхода\n");
    printf("FPS: минимум 12, целевой: 60\n\n");
    
    // Создаем приложение
    App* app = engine_create_app("2D Game Engine Demo", 800, 600);
    if (!app) {
        printf("Ошибка создания приложения\n");
        return 1;
    }
    
    printf("Приложение создано успешно\n");
    printf("Размер окна: 800x600\n");
    printf("Размер камеры: %dx%d\n", CAMERA_WIDTH, CAMERA_HEIGHT);
    printf("Размер карты: 50x50 тайлов\n");
    printf("Размер тайла: 32 пикселя\n");
    printf("\nЗапуск игрового цикла...\n");
    
    // Запускаем игровой цикл
    engine_run_app();
    
    printf("\nИгровой цикл завершен\n");
    printf("Средний FPS: %.1f\n", engine_get_fps());
    
    // Очищаем ресурсы
    engine_cleanup();
    
    printf("Движок завершил работу\n");
    return 0;
}

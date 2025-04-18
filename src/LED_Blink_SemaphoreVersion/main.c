#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <semphr.h>
#include "pico/stdlib.h"

#define LED_PIN_GREEN 22
#define LED_PIN_YELLOW 19

static SemaphoreHandle_t xSemaphore_LedGreen = NULL;
static SemaphoreHandle_t xSemaphore_LedYellow = NULL;

void led_task_Green(void *pvParameters)
{   
    gpio_init(LED_PIN_GREEN);
    gpio_set_dir(LED_PIN_GREEN, GPIO_OUT);
    while (true) {
        xSemaphoreTake(xSemaphore_LedGreen, portMAX_DELAY);
        gpio_put(LED_PIN_GREEN, 1);
        vTaskDelay(pdMS_TO_TICKS(200));
        gpio_put(LED_PIN_GREEN, 0);
        xSemaphoreGive(xSemaphore_LedYellow);
    }
}

void led_task_Yellow(void *pvParameters)  
{
    gpio_init(LED_PIN_YELLOW);
    gpio_set_dir(LED_PIN_YELLOW, GPIO_OUT);
    while (true) {
        xSemaphoreTake(xSemaphore_LedYellow, portMAX_DELAY);
        gpio_put(LED_PIN_YELLOW, 1);
        vTaskDelay(pdMS_TO_TICKS(200));
        gpio_put(LED_PIN_YELLOW, 0);
        xSemaphoreGive(xSemaphore_LedGreen);
    }
}

int main()
{
    stdio_init_all();

    xSemaphore_LedGreen = xSemaphoreCreateBinary();
    xSemaphore_LedYellow = xSemaphoreCreateBinary();

    xTaskCreate(led_task_Green, "LED_Task_Green", 256, NULL, 1, NULL);
    xTaskCreate(led_task_Yellow, "LED_Task_Yellow", 256, NULL, 1, NULL);

    xSemaphoreGive(xSemaphore_LedGreen);
    vTaskStartScheduler();

    while(1){};
}
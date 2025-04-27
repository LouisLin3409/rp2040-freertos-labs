#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <queue.h>
#include <timers.h>
#include "pico/stdlib.h"

#define LED_PIN_GREEN 22
#define BUTTON_PIN 19

#define BLINK_FAST 0
#define BLINK_MEDIUM 1
#define BLINK_SLOW 2

static QueueHandle_t xQueue = NULL;
static TimerHandle_t xDebounceTimer = NULL;

void init_button()
{
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
}

void init_led(uint gpio_led)
{
    gpio_init(gpio_led);
    gpio_set_dir(gpio_led, GPIO_OUT);
}

void button_callback(uint gpio, uint32_t events)
{
    // Declare a flag to determine if a highier priority task was worken
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* Start the debounce timer from within the ISR
     * This will delay the actual button processing to avoid bouncing effects 
     */
    xTimerStartFromISR(xDebounceTimer, &xHigherPriorityTaskWoken);

    /* If starting the timer caused a higher priority task to become ready,
     * request a context switch to ensure it executes immediately after the ISR
     */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void debounce_timer_callback(TimerHandle_t xTimer){
    static uint32_t frequence_level = 0;

    if(gpio_get(BUTTON_PIN) == 0){
        if(xQueueSend(xQueue, &frequence_level, 0)){
            printf("[%s]Send level%d to Queue\n\n", __FUNCTION__, frequence_level);
            frequence_level++;
            frequence_level = frequence_level % 3;
        }
    }
}

void led_task_Green(void *pvParameters)
{   
    uint32_t receive_frequence_level = 0;
    static uint32_t time = 0;
    init_led(LED_PIN_GREEN);
    while (true) {
        if(xQueueReceive(xQueue, &receive_frequence_level, 0))
        {
            switch (receive_frequence_level)
            {
                case BLINK_FAST:
                    time = 100;
                    break;
                case BLINK_MEDIUM:
                    time = 500;
                    break;
                case BLINK_SLOW:
                    time = 1000;
                    break;
            }
        }
        gpio_put(LED_PIN_GREEN, 1);
        vTaskDelay(pdMS_TO_TICKS(time));
        gpio_put(LED_PIN_GREEN, 0);
        vTaskDelay(pdMS_TO_TICKS(time));
    }
}


int main()
{
    stdio_init_all();
    init_button();
    xQueue = xQueueCreate(1, sizeof(uint));
    xDebounceTimer = xTimerCreate("DebounceTimer", pdMS_TO_TICKS(30), pdFALSE, NULL, debounce_timer_callback);
    
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &button_callback);

    xTaskCreate(led_task_Green, "LED_Task_Green", 256, NULL, 1, NULL);

    vTaskStartScheduler();
    

    while(1){};
}
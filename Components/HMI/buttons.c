
#include "freeRtos.h"
#include "task.h"
#include "gpio.h"
#include "queue.h"
#include "buttons.h"

#include "MS1_config.h"

TaskHandle_t TaskHandle_btn;
QueueHandle_t QueueHandle_btn;

ENUM_BTN_LIST_t pressed_button = E_BTN_NONE;

static void handler_btn(void* parameters);

/** ************************************************************* *
 * @brief       
 * 
 * @param       parameters 
 * ************************************************************* **/
static void handler_btn(void* parameters)
{
    ENUM_BTN_LIST_t btn;

    while(1)
    {
        if(xQueueReceive(QueueHandle_btn, &btn, portMAX_DELAY))
        {
            pressed_button = btn;
        }
    }
}

void bouttons_init(ENUM_BTN_LIST_t* button)
{
    BaseType_t status;
    
    button = &pressed_button;

    QueueHandle_btn  = xQueueCreate (1, sizeof(ENUM_BTN_LIST_t));

    status = xTaskCreate(handler_btn, "task_btn", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_APP_BTN, &TaskHandle_btn);
    configASSERT(status == pdPASS);
}

void callback_buttons(ENUM_BTN_LIST_t btn)
{
    xQueueSendFromISR(QueueHandle_btn, &btn, (TickType_t)0);
}

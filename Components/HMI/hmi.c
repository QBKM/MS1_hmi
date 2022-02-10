/** ************************************************************* *
 * @file        API_leds.c
 * @brief       
 * 
 * @date        2021-12-08
 * @author      Quentin Bakrim (quentin.bakrim@hotmail.fr)
 * 
 * Mines Space
 * 
 * ************************************************************* **/


/* ------------------------------------------------------------- --
   include
-- ------------------------------------------------------------- */
#include "leds.h"
#include "oled.h"
#include "freeRtos.h"
#include "task.h"
#include "gpio.h"
#include "queue.h"

#include "MS1_config.h"

/* ------------------------------------------------------------- --
   defines
-- ------------------------------------------------------------- */

/* ------------------------------------------------------------- --
   handles
-- ------------------------------------------------------------- */
TaskHandle_t TaskHandle_oled;
TaskHandle_t TaskHandle_btn;
QueueHandle_t QueueHandle_btn;


/* ------------------------------------------------------------- --
   variables
-- ------------------------------------------------------------- */

/* ------------------------------------------------------------- --
   prototypes
-- ------------------------------------------------------------- */
static void handler_oled(void* parameters);
static void handler_btn(void* parameters);

/* ============================================================= ==
   tasks functions
== ============================================================= */
/** ************************************************************* *
 * @brief       This task manage the payload system with the 
 *              opening or closing features. The task need to 
 *              receive command from queue to operate.
 *              Please check at the ENUM_CMD_ID_t enum to send
 *              commands
 * 
 * @param       parameters 
 * ************************************************************* **/
static void handler_oled(void* parameters)
{
 
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       parameters 
 * ************************************************************* **/
static void handler_btn(void* parameters)
{
    ENUM_BTN_LIST_t btn;

    if(xQueueReceive(QueueHandle_btn, &btn, portMAX_DELAY))
    {
        switch (btn)
        {
        case E_LIST_BTN1:
            /* TODO */
            break;

        case E_LIST_BTN2:
            /* TODO */
            break;

        case E_LIST_BTN3:
            /* TODO */
            break;

        case E_LIST_BTN4:
            /* TODO */
            break;                                    
        
        default:
            break;
        }
    }
}

/* ============================================================= ==
   public functions
== ============================================================= */
/** ************************************************************* *
 * @brief       init and start the payload task
 * 
 * ************************************************************* **/
void API_HMI_START(void)
{
    BaseType_t status;

  
    /* create the task */
    status = xTaskCreate(handler_oled, "task_oled", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_APP_OLED, &TaskHandle_oled);
    configASSERT(status == pdPASS);

    status = xTaskCreate(handler_btn, "task_btn", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_APP_BTN, &TaskHandle_btn);
    configASSERT(status == pdPASS);
}

/** ************************************************************* *
 * @brief       send a command to the payload task
 * 
 * @param       cmd 
 * ************************************************************* **/
void API_HMI_CALLBACK(ENUM_HMI_BTN_t btn)
{
    xQueueSend(QueueHandle_btn, &btn, (TickType_t)0);
}

/* ------------------------------------------------------------- --
   end of file
-- ------------------------------------------------------------- */

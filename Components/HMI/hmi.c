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

#include "freeRtos.h"
#include "task.h"
#include "gpio.h"
#include "queue.h"

#include "oled_gfx.h"

#include "hmi.h"

#include "string.h"

#include "MS1_config.h"

/* ------------------------------------------------------------- --
   defines
-- ------------------------------------------------------------- */
#define OLED_DEFAULT_PERIOD_TASK 100

#define OLED_MENU_LIST_0        
#define OLED_MENU_LIST_1        OLED_GUI_MONITORING
#define OLED_MENU_LIST_2        OLED_GUI_STATUS
#define OLED_MENU_LIST_3        OLED_GUI_DATA
#define OLED_MENU_LIST_4        
#define OLED_MENU_LIST_5

#define OLED_MENU_LINE_0		0
#define OLED_MENU_LINE_1		16
#define OLED_MENU_LINE_2		24
#define OLED_MENU_LINE_3		32
#define OLED_MENU_LINE_4		40
#define OLED_MENU_LINE_5		48




#define OLED_BTN_NONE       E_BTN_NONE
#define OLED_BTN_UP         E_BTN_1
#define OLED_BTN_DOWN       E_BTN_3
#define OLED_BTN_OK         E_BTN_2
#define OLED_BTN_RETURN     E_BTN_4


/* ------------------------------------------------------------- --
   types
-- ------------------------------------------------------------- */

typedef enum
{
    E_HMI_OLED_LINE_0,
    E_HMI_OLED_LINE_1,
    E_HMI_OLED_LINE_2,
    E_HMI_OLED_LINE_3,
    E_HMI_OLED_LINE_4,
    E_HMI_OLED_LINE_5,
    E_HMI_OLED_LINE_6,
    E_HMI_OLED_LINE_7,
    E_HMI_OLED_LINE_8,
    E_HMI_OLED_LINE_9
}ENUM_HMI_OLED_POINTER_t;

typedef enum
{
    E_HMI_OLED_MENU_OLD, 
    E_HMI_OLED_MENU_NEW
}ENUM_HMI_OLED_MENU_FLAG_t;

typedef struct
{
    ENUM_HMI_OLED_MENU_FLAG_t menu_flag;
    ENUM_HMI_OLED_POINTER_t line_pointer;
    ENUM_HMI_OLED_POINTER_t min_pointer;
    ENUM_HMI_OLED_POINTER_t max_pointer;
    void (*OLED_GUI_MENU)(void);
}STRUCT_HMI_OLED_t;

/* ------------------------------------------------------------- --
   handles
-- ------------------------------------------------------------- */
TaskHandle_t TaskHandle_oled;

/* ------------------------------------------------------------- --
   variables
-- ------------------------------------------------------------- */
ENUM_BTN_LIST_t button = OLED_BTN_NONE;
STRUCT_UART_STORAGE_t parsed_data = {0};
STRUCT_HMI_OLED_t oled = {0};

/* ------------------------------------------------------------- --
   prototypes
-- ------------------------------------------------------------- */
static void handler_oled(void* parameters);

/* menu GUI */
static void OLED_GUI_START(void);
static void OLED_GUI_MAIN(void);
static void OLED_GUI_MONITORING(void);
static void OLED_GUI_STATUS(void);
static void OLED_GUI_DATA(void);
static void OLED_GUI_SLEEP(void);
static void OLED_GUI_RESET(void);

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
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    while(1)
    {
        if(oled.OLED_GUI_MENU == OLED_GUI_MAIN)
        {
            switch (button)
            {
                case OLED_BTN_UP:      oled.line_pointer = (oled.line_pointer >= oled.max_pointer) ? oled.max_pointer : oled.line_pointer +1; break;
                case OLED_BTN_DOWN:    oled.line_pointer = (oled.line_pointer <= oled.min_pointer) ? oled.min_pointer : oled.line_pointer -1; break;
                case OLED_BTN_OK:
                    switch (oled.line_pointer)
                    {
                        case E_HMI_OLED_LINE_0: break;
                        case E_HMI_OLED_LINE_1: oled.OLED_GUI_MENU = OLED_GUI_MONITORING;    break;
                        case E_HMI_OLED_LINE_2: oled.OLED_GUI_MENU = OLED_GUI_STATUS;        break;
                        case E_HMI_OLED_LINE_3: oled.OLED_GUI_MENU = OLED_GUI_DATA;          break;
                        case E_HMI_OLED_LINE_4: break;
                        case E_HMI_OLED_LINE_5: break;
                        case E_HMI_OLED_LINE_6: break;
                        case E_HMI_OLED_LINE_7: break;
                        case E_HMI_OLED_LINE_8: break;
                        case E_HMI_OLED_LINE_9: break;
                        default:                break;
                    }
                    break;

                case OLED_BTN_RETURN:
                default: break;
            }
            button = E_BTN_NONE;
        }
        else if(button == OLED_BTN_RETURN)
        {
            oled.OLED_GUI_MENU = OLED_GUI_MAIN;
            oled.menu_flag = E_HMI_OLED_MENU_NEW;
            button = E_BTN_NONE;
        }

        /* refresh the page */
        oled.OLED_GUI_MENU(); 

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(OLED_DEFAULT_PERIOD_TASK));
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * ************************************************************* **/
static void OLED_GUI_START(void)
{
    if(oled.menu_flag == E_HMI_OLED_MENU_NEW)
    {
        //seeting for the menu
        oled.line_pointer = E_HMI_OLED_LINE_0;
        oled.min_pointer  = E_HMI_OLED_LINE_0;
        oled.max_pointer  = E_HMI_OLED_LINE_0;
        oled.menu_flag    = E_HMI_OLED_MENU_OLD;

        Clear_Screen();
        Set_Color(WHITE);
        print_String(0, 0, (const uint8_t*)"Hello World !", FONT_5X8);
        print_String(0, 15, (const uint8_t*)"Waiting for SEQ ready...", FONT_5X8);
    }

    /* print the image */
    /* wait for seq */
    if(!strcmp((char*)parsed_data.APP_PHASE, "wait"))
    {
        oled.OLED_GUI_MENU = OLED_GUI_MAIN;
        oled.menu_flag = E_HMI_OLED_MENU_NEW;
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * ************************************************************* **/
static void OLED_GUI_MAIN(void)
{
	static ENUM_HMI_OLED_POINTER_t last_pointer = E_HMI_OLED_LINE_1;

    if(oled.menu_flag == E_HMI_OLED_MENU_NEW)
    {
        //seeting for the menu
        oled.line_pointer = E_HMI_OLED_LINE_1;
        oled.min_pointer  = E_HMI_OLED_LINE_1;
        oled.max_pointer  = E_HMI_OLED_LINE_5;
        oled.menu_flag    = E_HMI_OLED_MENU_OLD;

        Clear_Screen();
        Set_Color(WHITE);
        print_String(0, 0, (const uint8_t*)"===== MAIN MENU =====", FONT_5X8);
        print_String(10, 15, (const uint8_t*)"Monitoring", FONT_5X8);
        print_String(10, 23, (const uint8_t*)"Status", FONT_5X8);
        print_String(10, 31, (const uint8_t*)"Data", FONT_5X8);
        print_String(10, 39, (const uint8_t*)"Sleep", FONT_5X8);
        print_String(10, 47, (const uint8_t*)"Reset all", FONT_5X8);
        print_String(0, OLED_MENU_LINE_1, (const uint8_t*)">", FONT_5X8);
    }

    if(last_pointer != oled.line_pointer)
    {
    	last_pointer = oled.line_pointer;
    	Set_Color(BLACK);
    	Fill_Rect(0,OLED_MENU_LINE_1 , 5, OLED_MENU_LINE_5);
    	Set_Color(WHITE);

		switch(oled.line_pointer)
		{
		case E_HMI_OLED_LINE_1 : print_String(0, OLED_MENU_LINE_1, (const uint8_t*)">", FONT_5X8); break;
		case E_HMI_OLED_LINE_2 : print_String(0, OLED_MENU_LINE_2, (const uint8_t*)">", FONT_5X8); break;
		case E_HMI_OLED_LINE_3 : print_String(0, OLED_MENU_LINE_3, (const uint8_t*)">", FONT_5X8); break;
		case E_HMI_OLED_LINE_4 : print_String(0, OLED_MENU_LINE_4, (const uint8_t*)">", FONT_5X8); break;
		case E_HMI_OLED_LINE_5 : print_String(0, OLED_MENU_LINE_5, (const uint8_t*)">", FONT_5X8); break;
		default: break;
		}
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * ************************************************************* **/
static void OLED_GUI_MONITORING(void)
{
    if(oled.menu_flag == E_HMI_OLED_MENU_NEW)
    {
        //seeting for the menu
        oled.line_pointer = E_HMI_OLED_LINE_1;
        oled.min_pointer  = E_HMI_OLED_LINE_1;
        oled.max_pointer  = E_HMI_OLED_LINE_5;
        oled.menu_flag    = E_HMI_OLED_MENU_OLD;
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * ************************************************************* **/
static void OLED_GUI_STATUS(void)
{
    if(oled.menu_flag == E_HMI_OLED_MENU_NEW)
    {
        //seeting for the menu
        oled.line_pointer = E_HMI_OLED_LINE_1;
        oled.min_pointer  = E_HMI_OLED_LINE_1;
        oled.max_pointer  = E_HMI_OLED_LINE_5;
        oled.menu_flag    = E_HMI_OLED_MENU_OLD;
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * ************************************************************* **/
static void OLED_GUI_DATA(void)
{
    if(oled.menu_flag == E_HMI_OLED_MENU_NEW)
    {
        //seeting for the menu
        oled.line_pointer = E_HMI_OLED_LINE_1;
        oled.min_pointer  = E_HMI_OLED_LINE_1;
        oled.max_pointer  = E_HMI_OLED_LINE_5;
        oled.menu_flag    = E_HMI_OLED_MENU_OLD;
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * ************************************************************* **/
static void OLED_GUI_SLEEP(void)
{
    if(oled.menu_flag == E_HMI_OLED_MENU_NEW)
    {
        //seeting for the menu
        oled.line_pointer = E_HMI_OLED_LINE_0;
        oled.min_pointer  = E_HMI_OLED_LINE_0;
        oled.max_pointer  = E_HMI_OLED_LINE_0;
        oled.menu_flag    = E_HMI_OLED_MENU_OLD;

        print_String(30, 30, (const uint8_t*)"POWER SAVING MODE", FONT_5X8);
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * ************************************************************* **/
static void OLED_GUI_RESET(void)
{
    /* restart the SEQ software */
    HAL_GPIO_WritePin(SEQ_RST_GPIO_Port, SEQ_RST_Pin, GPIO_PIN_SET);
    
    /* restart the HMI software */
	SCB->AIRCR = 0x05fa0004;
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

    leds_init();
    leds_send_cmd(E_LIST_LED5, E_CMD_LEDS_GREEN);

    uart_init(&parsed_data);

    bouttons_init(&button);

    /* oled */ 
    Device_Init();
    Clear_Screen();
    oled.OLED_GUI_MENU = OLED_GUI_START;
    oled.menu_flag = E_HMI_OLED_MENU_NEW;

    /* create the task */
    status = xTaskCreate(handler_oled, "task_oled", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_APP_OLED, &TaskHandle_oled);
    configASSERT(status == pdPASS);
}

/* ------------------------------------------------------------- --
   end of file
-- ------------------------------------------------------------- */

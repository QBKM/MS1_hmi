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
#include "hmi.h"
#include "list_uart.h"
#include "freeRtos.h"
#include "task.h"
#include "gpio.h"
#include "queue.h"
#include "string.h"
#include "stdlib.h"
#include "oled_gfx.h"

#include "MS1_config.h"

/* ------------------------------------------------------------- --
   defines
-- ------------------------------------------------------------- */
#define ID_SIZE         2
#define PAYLOAD_SIZE    12

#define ID_OFFSET       1
#define PAYLOAD_OFFSET  4

#define OLED_DEFAULT_PERIOD_TASK 100

#define OLED_MENU_LIST_0        
#define OLED_MENU_LIST_1        OLED_GUI_MONITORING
#define OLED_MENU_LIST_2        OLED_GUI_STATUS
#define OLED_MENU_LIST_3        OLED_GUI_DATA
#define OLED_MENU_LIST_4        
#define OLED_MENU_LIST_5        

/* ------------------------------------------------------------- --
   types
-- ------------------------------------------------------------- */
typedef enum 
{
    E_HMI_OLED_BTN_NONE,
    E_HMI_OLED_BTN_UP,
    E_HMI_OLED_BTN_DOWN, 
    E_HMI_OLED_BTN_OK,
    E_HMI_OLED_BTN_RETURN
}ENUM_HMI_OLED_BTN_t;

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

typedef struct
{
    uint8_t APP_PHASE[PAYLOAD_SIZE];
    uint8_t APP_AEROC[PAYLOAD_SIZE];
    uint8_t APP_WINDOW[PAYLOAD_SIZE];
    uint8_t APP_RECOV_APOGEE[PAYLOAD_SIZE];

    uint8_t SENS_IMU_AX[PAYLOAD_SIZE];
    uint8_t SENS_IMU_AY[PAYLOAD_SIZE];
    uint8_t SENS_IMU_AZ[PAYLOAD_SIZE];
    uint8_t SENS_IMU_GX[PAYLOAD_SIZE];
    uint8_t SENS_IMU_GY[PAYLOAD_SIZE];
    uint8_t SENS_IMU_GZ[PAYLOAD_SIZE];
    uint8_t SENS_IMU_TEMP[PAYLOAD_SIZE];
    uint8_t SENS_IMU_ERROR[PAYLOAD_SIZE];
    uint8_t SENS_BARO_PRESS[PAYLOAD_SIZE];
    uint8_t SENS_BARO_TEMP[PAYLOAD_SIZE];
    uint8_t SENS_BARO_ERROR[PAYLOAD_SIZE];

    uint8_t MNTR_BAT_SEQ[PAYLOAD_SIZE];
    uint8_t MNTR_BAT_MOTOR1[PAYLOAD_SIZE];
    uint8_t MNTR_BAT_MOTOR2[PAYLOAD_SIZE];

    uint8_t RECOV_LAST_CMD[PAYLOAD_SIZE];
    uint8_t RECOV_STATUS[PAYLOAD_SIZE];

    uint8_t PAYLOAD_LAST_CMD[PAYLOAD_SIZE];
    uint8_t PAYLOAD_STATUS[PAYLOAD_SIZE];
}STRUCT_HMI_STORAGE_t;

/* ------------------------------------------------------------- --
   handles
-- ------------------------------------------------------------- */
TaskHandle_t TaskHandle_oled;
TaskHandle_t TaskHandle_btn;
TaskHandle_t TaskHandle_uart;
QueueHandle_t QueueHandle_btn;
QueueHandle_t QueueHandle_uart;


/* ------------------------------------------------------------- --
   variables
-- ------------------------------------------------------------- */
STRUCT_HMI_STORAGE_t hmi_storage = {0};
STRUCT_HMI_OLED_t oled = {0};
ENUM_HMI_OLED_BTN_t button = E_HMI_OLED_BTN_NONE;

/* ------------------------------------------------------------- --
   prototypes
-- ------------------------------------------------------------- */
static void handler_oled(void* parameters);
static void handler_btn(void* parameters);
static void handler_uart(void* parameters);

/* menu GUI */
static void OLED_GUI_START(void);
static void OLED_GUI_MAIN(void);
static void OLED_GUI_MONITORING(void);
static void OLED_GUI_STATUS(void);
static void OLED_GUI_DATA(void);

/* UART id parsing fucntions */
static void id_parser_app_phase(uint8_t* data);
static void id_parser_app_window(uint8_t* data);
static void id_parser_app_aeroc(uint8_t* data);

static void id_parser_sens_imu_ax(uint8_t* data);
static void id_parser_sens_imu_ay(uint8_t* data);
static void id_parser_sens_imu_az(uint8_t* data);
static void id_parser_sens_imu_gx(uint8_t* data);
static void id_parser_sens_imu_gy(uint8_t* data);
static void id_parser_sens_imu_gz(uint8_t* data);
static void id_parser_sens_imu_error(uint8_t* data);
static void id_parser_sens_imu_temp(uint8_t* data);
static void id_parser_sens_baro_press(uint8_t* data);
static void id_parser_sens_baro_temp(uint8_t* data);
static void id_parser_sens_baro_error(uint8_t* data);

static void id_parser_mntr_bat_seq(uint8_t* data);
static void id_parser_mntr_bat_motor1(uint8_t* data);
static void id_parser_mntr_bat_motor2(uint8_t* data);

static void id_parser_recov_last_cmd(uint8_t* data);
static void id_parser_recov_status(uint8_t* data);

static void id_parser_payload_last_cmd(uint8_t* data);
static void id_parser_payload_status(uint8_t* data);

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
                case E_HMI_OLED_BTN_UP:      oled.line_pointer = (oled.line_pointer >= oled.max_pointer) ? oled.max_pointer : oled.line_pointer +1; break;
                case E_HMI_OLED_BTN_DOWN:    oled.line_pointer = (oled.line_pointer <= oled.min_pointer) ? oled.min_pointer : oled.line_pointer -1; break;
                case E_HMI_OLED_BTN_OK:
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
                    }break;

                case E_HMI_OLED_BTN_RETURN:
                default: break;
            }
        }
        else if(button == E_HMI_OLED_BTN_RETURN)
        {
            oled.OLED_GUI_MENU = OLED_GUI_MAIN;
        }

        /* refresh the page */
        oled.OLED_GUI_MENU();

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(OLED_DEFAULT_PERIOD_TASK));
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       parameters 
 * ************************************************************* **/
static void handler_btn(void* parameters)
{
    ENUM_HMI_BTN_t btn;

    while(1)
    {
        if(xQueueReceive(QueueHandle_btn, &btn, portMAX_DELAY))
        {
            switch (btn)
            {
                case E_HMI_BTN1: button = E_HMI_OLED_BTN_UP;        break;
                case E_HMI_BTN2: button = E_HMI_OLED_BTN_DOWN;      break;
                case E_HMI_BTN3: button = E_HMI_OLED_BTN_RETURN;    break;
                case E_HMI_BTN4: button = E_HMI_OLED_BTN_OK;        break;
                default:                                            break;
            }
        }
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       parameters 
 * ************************************************************* **/
static void handler_uart(void* parameters)
{
    uint8_t frame[MAX_RX_BUFFER_SIZE] = {0};
    uint8_t buffID[ID_SIZE+1] = {0};
    uint8_t id = 0;

    while(1)
    {
        if(xQueueReceive(QueueHandle_uart, &frame, portMAX_DELAY))
        {
            if(frame[0] == '[' && frame[3] == ']')
            {
                // parsing 
	            memcpy(buffID, (char*)(frame + ID_OFFSET), ID_SIZE);
	            buffID[2] = '\0';
	            id = atoi((char*)buffID);

                /* process the payload with the associated ID */
                switch (id)
                {
                    /* none */
                    case HMI_ID_NONE:                                                    break;

                    /* application data */
                    case HMI_ID_APP_PHASE:          id_parser_app_phase(frame);          break; 
                    case HMI_ID_APP_WINDOW:         id_parser_app_window(frame);         break; 
                    case HMI_ID_APP_AEROC:          id_parser_app_aeroc(frame);          break;   
                    
                    /* sensors data */
                    case HMI_ID_SENS_IMU_AX:        id_parser_sens_imu_ax(frame);        break; 
                    case HMI_ID_SENS_IMU_AY:        id_parser_sens_imu_ay(frame);        break;   
                    case HMI_ID_SENS_IMU_AZ:        id_parser_sens_imu_az(frame);        break;    
                    case HMI_ID_SENS_IMU_GX:        id_parser_sens_imu_gx(frame);        break; 
                    case HMI_ID_SENS_IMU_GY:        id_parser_sens_imu_gy(frame);        break;   
                    case HMI_ID_SENS_IMU_GZ:        id_parser_sens_imu_gz(frame);        break;
                    case HMI_ID_SENS_IMU_TEMP:      id_parser_sens_imu_temp(frame);      break;
                    case HMI_ID_SENS_IMU_ERROR:     id_parser_sens_imu_error(frame);     break;
                    case HMI_ID_SENS_BARO_PRESS:    id_parser_sens_baro_press(frame);    break;
                    case HMI_ID_SENS_BARO_TEMP:     id_parser_sens_baro_temp(frame);     break;
                    case HMI_ID_SENS_BARO_ERROR:    id_parser_sens_baro_error(frame);    break;
                    
                    /* monitoring data */
                    case HMI_ID_MNTR_BAT_SEQ:       id_parser_mntr_bat_seq(frame);       break;
                    case HMI_ID_MNTR_BAT_MOTOR1:    id_parser_mntr_bat_motor1(frame);    break;
                    case HMI_ID_MNTR_BAT_MOTOR2:    id_parser_mntr_bat_motor2(frame);    break;
                    
                    /* recovery data */
                    case HMI_ID_RECOV_LAST_CMD:     id_parser_recov_last_cmd(frame);     break;
                    case HMI_ID_RECOV_STATUS:       id_parser_recov_status(frame);       break;
                    
                    /* payload data */
                    case HMI_ID_PAYLOAD_LAST_CMD:   id_parser_payload_last_cmd(frame);   break; 
                    case HMI_ID_PAYLOAD_STATUS:     id_parser_payload_status(frame);     break;

                    default:                                                            break;

                }
            }
        }
    }
}

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
    }

    /* print the image */
    /* wait for seq */
    //if(seq == ok)
    //oled.OLED_GUI_MENU = OLED_GUI_MAIN;

}
static void OLED_GUI_MAIN(void)
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
 * @param       data 
 * ************************************************************* **/
static void id_parser_app_phase(uint8_t* data)
{
    memcpy(hmi_storage.APP_PHASE, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));

    /* phase wait */
    if(strcmp("wait", (char*)(data + PAYLOAD_OFFSET)))
    {
        API_LEDS_SEND_CMD(E_LIST_LED4, E_CMD_LEDS_GREEN);
    }
    else
    /* phase ascend */
    if(strcmp("ascend", (char*)(data + PAYLOAD_OFFSET)))
    {
        API_LEDS_SEND_CMD(E_LIST_LED4, E_CMD_LEDS_RED);
    }
    else
    /* phase descend */
    if(strcmp("descend", (char*)(data + PAYLOAD_OFFSET)))
    {
        API_LEDS_SEND_CMD(E_LIST_LED4, E_CMD_LEDS_NONE);
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_app_window(uint8_t* data)
{
    /* window in */
    if(strcmp("in", (char*)(data + PAYLOAD_OFFSET)))
    {
        memcpy(hmi_storage.APP_WINDOW, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
        API_LEDS_SEND_CMD(E_LIST_LED4, E_CMD_LEDS_BLUE);
    }
    else
    /* window out */
    if(strcmp("out", (char*)(data + PAYLOAD_OFFSET)))
    {
        memcpy(hmi_storage.APP_WINDOW, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
        API_LEDS_SEND_CMD(E_LIST_LED4, E_CMD_LEDS_NONE);
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_app_aeroc(uint8_t* data)
{
    memcpy(hmi_storage.APP_AEROC, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
    API_LEDS_SEND_CMD(E_LIST_LED5, E_CMD_LEDS_RED);
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_sens_imu_ax(uint8_t* data)
{
    memcpy(hmi_storage.SENS_IMU_AX, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_sens_imu_ay(uint8_t* data)
{
    memcpy(hmi_storage.SENS_IMU_AY, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_sens_imu_az(uint8_t* data)
{
    memcpy(hmi_storage.SENS_IMU_AZ, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_sens_imu_gx(uint8_t* data)
{
    memcpy(hmi_storage.SENS_IMU_GX, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_sens_imu_gy(uint8_t* data)
{
    memcpy(hmi_storage.SENS_IMU_GY, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_sens_imu_gz(uint8_t* data)
{
    memcpy(hmi_storage.SENS_IMU_GZ, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_sens_imu_temp(uint8_t* data)
{
    memcpy(hmi_storage.SENS_IMU_TEMP, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_sens_imu_error(uint8_t* data)
{
    memcpy(hmi_storage.SENS_IMU_ERROR, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_sens_baro_press(uint8_t* data)
{
    memcpy(hmi_storage.SENS_BARO_PRESS, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_sens_baro_temp(uint8_t* data)
{
    memcpy(hmi_storage.SENS_BARO_TEMP, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_sens_baro_error(uint8_t* data)
{
    memcpy(hmi_storage.SENS_BARO_ERROR, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_mntr_bat_seq(uint8_t* data)
{
    memcpy(hmi_storage.MNTR_BAT_SEQ, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
    
    /* battery ok */
    if(strcmp("ok", (char*)(data + PAYLOAD_OFFSET)))
    {
        API_LEDS_SEND_CMD(E_LIST_LED1, E_CMD_LEDS_GREEN);
    }
    else
    /* battery ko */
    if(strcmp("ko", (char*)(data + PAYLOAD_OFFSET)))
    {
        API_LEDS_SEND_CMD(E_LIST_LED1, E_CMD_LEDS_RED);
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_mntr_bat_motor1(uint8_t* data)
{
    memcpy(hmi_storage.MNTR_BAT_MOTOR1, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
    
    /* battery ok */
    if(strcmp("ok", (char*)(data + PAYLOAD_OFFSET)))
    {
        API_LEDS_SEND_CMD(E_LIST_LED2, E_CMD_LEDS_GREEN);
    }
    else
    /* battery ko */
    if(strcmp("ko", (char*)(data + PAYLOAD_OFFSET)))
    {
        API_LEDS_SEND_CMD(E_LIST_LED2, E_CMD_LEDS_RED);
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_mntr_bat_motor2(uint8_t* data)
{
    memcpy(hmi_storage.MNTR_BAT_MOTOR2, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));

    /* battery ok */
    if(strcmp("ok", (char*)(data + PAYLOAD_OFFSET)))
    {
        API_LEDS_SEND_CMD(E_LIST_LED3, E_CMD_LEDS_GREEN);
    }
    else
    /* battery ko */
    if(strcmp("ko", (char*)(data + PAYLOAD_OFFSET)))
    {
        API_LEDS_SEND_CMD(E_LIST_LED3, E_CMD_LEDS_RED);
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_recov_last_cmd(uint8_t* data)
{
    memcpy(hmi_storage.RECOV_LAST_CMD, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_recov_status(uint8_t* data)
{
    memcpy(hmi_storage.RECOV_STATUS, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_payload_last_cmd(uint8_t* data)
{
    memcpy(hmi_storage.PAYLOAD_LAST_CMD, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_payload_status(uint8_t* data)
{
    memcpy(hmi_storage.PAYLOAD_STATUS, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
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

    /* AEROC led */
    API_LEDS_SEND_CMD(E_LIST_LED5, E_CMD_LEDS_GREEN);

    /* oled */ 
    Device_Init();
    Clear_Screen();
    oled.OLED_GUI_MENU = OLED_GUI_START;

    QueueHandle_btn  = xQueueCreate (1, sizeof(ENUM_HMI_BTN_t));
    QueueHandle_uart = xQueueCreate (32, sizeof(uint8_t*));

    /* create the task */
    status = xTaskCreate(handler_oled, "task_oled", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_APP_OLED, &TaskHandle_oled);
    configASSERT(status == pdPASS);

    status = xTaskCreate(handler_btn, "task_btn", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_APP_BTN, &TaskHandle_btn);
    configASSERT(status == pdPASS);

    status = xTaskCreate(handler_uart, "task_uart", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_APP_BTN, &TaskHandle_btn);
    configASSERT(status == pdPASS);
}

/** ************************************************************* *
 * @brief       send a command to the payload task
 * 
 * @param       cmd 
 * ************************************************************* **/
void API_HMI_BTN_CALLBACK(ENUM_HMI_BTN_t btn)
{
    xQueueSend(QueueHandle_btn, &btn, (TickType_t)0);
}

/** ************************************************************* *
 * @brief       send a command to the payload task
 * 
 * @param       cmd 
 * ************************************************************* **/
void API_HMI_UART_CALLBACK(uint8_t* data)
{
    xQueueSend(QueueHandle_uart, &data, (TickType_t)0);
}

/* ------------------------------------------------------------- --
   end of file
-- ------------------------------------------------------------- */

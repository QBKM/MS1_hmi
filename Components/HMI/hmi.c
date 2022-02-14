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

#define  OLED_DEFAULT_PERIOD_TASK 100

#define APP_PHASE_WAIT          "wait"
#define APP_PHASE_ASCEND        "ascend"
#define APP_PHASE_DESCEND       "descend"
#define APP_AEROC_RELEASED      "released"
#define APP_WINDOW_IN           "in"
#define APP_WINDOW_OUT          "out"
#define APP_RECOV_APOGEE_OK     "done"

#define MNTR_BAT_SEQ_OK         "ok"
#define MNTR_BAT_SEQ_KO         "ko"
#define MNTR_BAT_MOTOR1_OK      "ok"
#define MNTR_BAT_MOTOR1_KO      "ko"
#define MNTR_BAT_MOTOR2_OK      "ok"
#define MNTR_BAT_MOTOR2_KO      "ko"

#define RECOV_LAST_CMD_NONE     "none"
#define RECOV_LAST_CMD_STOP     "stop"
#define RECOV_LAST_CMD_OPEN     "open"
#define RECOV_LAST_CMD_CLOSE    "close"
#define RECOV_STATUS_NONE       "none"
#define RECOV_STATUS_STOPPED    "stopped"
#define RECOV_STATUS_RUNNING    "running"
#define RECOV_STATUS_OPENED     "opened"
#define RECOV_STATUS_CLOSED     "closed"

#define PAYLOAD_LAST_CMD_NONE   "none"
#define PAYLOAD_LAST_CMD_STOP   "stop"
#define PAYLOAD_LAST_CMD_OPEN   "open"
#define PAYLOAD_LAST_CMD_CLOSE  "close"
#define PAYLOAD_STATUS_NONE     "none"
#define PAYLOAD_STATUS_STOPPED  "stopped"
#define PAYLOAD_STATUS_RUNNING  "running"
#define PAYLOAD_STATUS_OPENED   "opened"
#define PAYLOAD_STATUS_CLOSED   "closed"

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
    uint8_t SENS_BARO_PRESS[PAYLOAD_SIZE];
    uint8_t SENS_BARO_TEMP[PAYLOAD_SIZE];

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
static void id_parser_sens_imu_temp(uint8_t* data);
static void id_parser_sens_baro_press(uint8_t* data);
static void id_parser_sens_baro_temp(uint8_t* data);
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
            case E_HMI_OLED_BTN_UP:      oled.line_pointer = (oled.line_pointer >= oled.max_pointer) ? oled.max_pointer : oled.line_pointer++; break;
            case E_HMI_OLED_BTN_DOWN:    oled.line_pointer = (oled.line_pointer <= oled.min_pointer) ? oled.min_pointer : oled.line_pointer--; break;
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
    uint8_t data[MAX_RX_BUFFER_SIZE] = {0};
    uint8_t buffID[ID_SIZE+1] = {0};
    uint8_t id = {0};

    while(1)
    {
        if(xQueueReceive(QueueHandle_uart, &data, portMAX_DELAY))
        {
            if(data[0] == '[' && data[3] == ']')
            {
                // ID parsing (from string to int)
	            memcpy(buffID, (char*)(data + ID_OFFSET), ID_SIZE);
	            buffID[2] = '\0';
	            id = atoi((char*)buffID);

                /* process the payload with the associated ID */
                switch (id)
                {
                    /* none */
                    case HMI_ID_NONE:                                                   break;

                    /* application data */
                    case HMI_ID_APP_PHASE:          id_parser_app_phase(data);          break; 
                    case HMI_ID_APP_WINDOW:         id_parser_app_window(data);         break; 
                    case HMI_ID_APP_AEROC:          id_parser_app_aeroc(data);          break;   
                    
                    /* sensors data */
                    case HMI_ID_SENS_IMU_AX:        id_parser_sens_imu_ax(data);        break; 
                    case HMI_ID_SENS_IMU_AY:        id_parser_sens_imu_ay(data);        break;   
                    case HMI_ID_SENS_IMU_AZ:        id_parser_sens_imu_az(data);        break;    
                    case HMI_ID_SENS_IMU_GX:        id_parser_sens_imu_gx(data);        break; 
                    case HMI_ID_SENS_IMU_GY:        id_parser_sens_imu_gy(data);        break;   
                    case HMI_ID_SENS_IMU_GZ:        id_parser_sens_imu_gz(data);        break;
                    case HMI_ID_SENS_IMU_TEMP:      id_parser_sens_imu_temp(data);      break;
                    case HMI_ID_SENS_BARO_PRESS:    id_parser_sens_baro_press(data);    break;
                    case HMI_ID_SENS_BARO_TEMP:     id_parser_sens_baro_temp(data);     break;
                    
                    /* monitoring data */
                    case HMI_ID_MNTR_BAT_SEQ:       id_parser_mntr_bat_seq(data);       break;
                    case HMI_ID_MNTR_BAT_MOTOR1:    id_parser_mntr_bat_motor1(data);    break;
                    case HMI_ID_MNTR_BAT_MOTOR2:    id_parser_mntr_bat_motor2(data);    break;
                    
                    /* recovery data */
                    case HMI_ID_RECOV_LAST_CMD:     id_parser_recov_last_cmd(data);     break;
                    case HMI_ID_RECOV_STATUS:       id_parser_recov_status(data);       break;
                    
                    /* payload data */
                    case HMI_ID_PAYLOAD_LAST_CMD:   id_parser_payload_last_cmd(data);   break; 
                    case HMI_ID_PAYLOAD_STATUS:     id_parser_payload_status(data);     break;

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
    /* phase wait */
    if(strcmp("w", (char*)(data + PAYLOAD_OFFSET)))
    {
        memcpy(hmi_storage.APP_PHASE, APP_PHASE_WAIT, sizeof(APP_PHASE_WAIT));
        API_LEDS_SEND_CMD(E_LIST_LED4, E_CMD_LEDS_GREEN);
    }
    else
    /* phase ascend */
    if(strcmp("a", (char*)(data + PAYLOAD_OFFSET)))
    {
        memcpy(hmi_storage.APP_PHASE, APP_PHASE_ASCEND, sizeof(APP_PHASE_ASCEND));
        API_LEDS_SEND_CMD(E_LIST_LED4, E_CMD_LEDS_RED);
    }
    else
    /* phase descend */
    if(strcmp("d", (char*)(data + PAYLOAD_OFFSET)))
    {
        memcpy(hmi_storage.APP_PHASE, APP_PHASE_DESCEND, sizeof(APP_PHASE_DESCEND));
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
    if(strcmp("i", (char*)(data + PAYLOAD_OFFSET)))
    {
        memcpy(hmi_storage.APP_WINDOW, APP_WINDOW_IN, sizeof(APP_WINDOW_IN));
        API_LEDS_SEND_CMD(E_LIST_LED4, E_CMD_LEDS_BLUE);
    }
    else
    /* window out */
    if(strcmp("o", (char*)(data + PAYLOAD_OFFSET)))
    {
        memcpy(hmi_storage.APP_WINDOW, APP_WINDOW_OUT, sizeof(APP_WINDOW_OUT));
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
    memcpy(hmi_storage.APP_AEROC, APP_AEROC_RELEASED, sizeof(APP_AEROC_RELEASED));
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
    memcpy(hmi_storage.SENS_IMU_TEMP, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_mntr_bat_seq(uint8_t* data)
{
    /* battery ok */
    if(strcmp("o", (char*)(data + PAYLOAD_OFFSET)))
    {
        memcpy(hmi_storage.MNTR_BAT_SEQ, MNTR_BAT_SEQ_OK, sizeof(MNTR_BAT_SEQ_OK));
        API_LEDS_SEND_CMD(E_LIST_LED1, E_CMD_LEDS_GREEN);
    }
    else
    /* battery ko */
    if(strcmp("k", (char*)(data + PAYLOAD_OFFSET)))
    {
        memcpy(hmi_storage.MNTR_BAT_SEQ, MNTR_BAT_SEQ_KO, sizeof(MNTR_BAT_SEQ_KO));
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
    /* battery ok */
    if(strcmp("o", (char*)(data + PAYLOAD_OFFSET)))
    {
        memcpy(hmi_storage.MNTR_BAT_MOTOR1, MNTR_BAT_MOTOR1_OK, sizeof(MNTR_BAT_MOTOR1_OK));
        API_LEDS_SEND_CMD(E_LIST_LED1, E_CMD_LEDS_GREEN);
    }
    else
    /* battery ko */
    if(strcmp("k", (char*)(data + PAYLOAD_OFFSET)))
    {
        memcpy(hmi_storage.MNTR_BAT_MOTOR1, MNTR_BAT_MOTOR1_KO, sizeof(MNTR_BAT_MOTOR1_KO));
        API_LEDS_SEND_CMD(E_LIST_LED1, E_CMD_LEDS_RED);
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_mntr_bat_motor2(uint8_t* data)
{
    /* battery ok */
    if(strcmp("o", (char*)(data + PAYLOAD_OFFSET)))
    {
        memcpy(hmi_storage.MNTR_BAT_MOTOR2, MNTR_BAT_MOTOR2_OK, sizeof(MNTR_BAT_MOTOR2_OK));
        API_LEDS_SEND_CMD(E_LIST_LED1, E_CMD_LEDS_GREEN);
    }
    else
    /* battery ko */
    if(strcmp("k", (char*)(data + PAYLOAD_OFFSET)))
    {
        memcpy(hmi_storage.MNTR_BAT_MOTOR1, MNTR_BAT_MOTOR2_KO, sizeof(MNTR_BAT_MOTOR2_KO));
        API_LEDS_SEND_CMD(E_LIST_LED1, E_CMD_LEDS_RED);
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_recov_last_cmd(uint8_t* data)
{
    /* no command */
    if(strcmp("n", (char*)(data + PAYLOAD_OFFSET)))
    {
        memcpy(hmi_storage.RECOV_LAST_CMD, RECOV_LAST_CMD_NONE, sizeof(RECOV_LAST_CMD_NONE));
    }
    else
    /* stop command */
    if(strcmp("s", (char*)(data + PAYLOAD_OFFSET)))
    {
        memcpy(hmi_storage.RECOV_LAST_CMD, RECOV_LAST_CMD_STOP, sizeof(RECOV_LAST_CMD_STOP));
    }
    else
    /* open command */
    if(strcmp("o", (char*)(data + PAYLOAD_OFFSET)))
    {
        memcpy(hmi_storage.RECOV_LAST_CMD, RECOV_LAST_CMD_OPEN, sizeof(RECOV_LAST_CMD_OPEN));
    }
    else
    /* close command */
    if(strcmp("c", (char*)(data + PAYLOAD_OFFSET)))
    {
        memcpy(hmi_storage.RECOV_LAST_CMD, RECOV_LAST_CMD_CLOSE, sizeof(RECOV_LAST_CMD_CLOSE));
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_recov_status(uint8_t* data)
{
    /* no command */
    if(strcmp("n", (char*)(data + PAYLOAD_OFFSET)))
    {
        memcpy(hmi_storage.RECOV_STATUS, RECOV_STATUS_NONE, sizeof(RECOV_STATUS_NONE));
    }
    else
    /* stop command */
    if(strcmp("s", (char*)(data + PAYLOAD_OFFSET)))
    {
        memcpy(hmi_storage.RECOV_STATUS, RECOV_STATUS_STOPPED, sizeof(RECOV_STATUS_STOPPED));
    }
    else
    /* open command */
    if(strcmp("r", (char*)(data + PAYLOAD_OFFSET)))
    {
        memcpy(hmi_storage.RECOV_STATUS, RECOV_STATUS_RUNNING, sizeof(RECOV_STATUS_RUNNING));
    }
    /* open command */
    if(strcmp("o", (char*)(data + PAYLOAD_OFFSET)))
    {
        memcpy(hmi_storage.RECOV_STATUS, RECOV_STATUS_OPENED, sizeof(RECOV_STATUS_OPENED));
    }                        
    else
    /* close command */
    if(strcmp("c", (char*)(data + PAYLOAD_OFFSET)))
    {
        memcpy(hmi_storage.RECOV_STATUS, RECOV_STATUS_CLOSED, sizeof(RECOV_STATUS_CLOSED));
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_payload_last_cmd(uint8_t* data)
{
    /* no command */
    if(strcmp("n", (char*)(data + PAYLOAD_OFFSET)))
    {
        memcpy(hmi_storage.PAYLOAD_LAST_CMD, PAYLOAD_LAST_CMD_NONE, sizeof(PAYLOAD_LAST_CMD_NONE));
    }
    else
    /* stop command */
    if(strcmp("s", (char*)(data + PAYLOAD_OFFSET)))
    {
        memcpy(hmi_storage.PAYLOAD_LAST_CMD, PAYLOAD_LAST_CMD_STOP, sizeof(PAYLOAD_LAST_CMD_STOP));
    }
    else
    /* open command */
    if(strcmp("o", (char*)(data + PAYLOAD_OFFSET)))
    {
        memcpy(hmi_storage.PAYLOAD_LAST_CMD, PAYLOAD_LAST_CMD_OPEN, sizeof(PAYLOAD_LAST_CMD_OPEN));
    }
    else
    /* close command */
    if(strcmp("c", (char*)(data + PAYLOAD_OFFSET)))
    {
        memcpy(hmi_storage.PAYLOAD_LAST_CMD, PAYLOAD_LAST_CMD_CLOSE, sizeof(PAYLOAD_LAST_CMD_CLOSE));
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_payload_status(uint8_t* data)
{
    /* no command */
    if(strcmp("n", (char*)(data + PAYLOAD_OFFSET)))
    {
        memcpy(hmi_storage.PAYLOAD_STATUS, PAYLOAD_STATUS_NONE, sizeof(PAYLOAD_STATUS_NONE));
    }
    else
    /* stop command */
    if(strcmp("s", (char*)(data + PAYLOAD_OFFSET)))
    {
        memcpy(hmi_storage.PAYLOAD_STATUS, PAYLOAD_STATUS_STOPPED, sizeof(PAYLOAD_STATUS_STOPPED));
    }
    else
    /* open command */
    if(strcmp("r", (char*)(data + PAYLOAD_OFFSET)))
    {
        memcpy(hmi_storage.PAYLOAD_STATUS, PAYLOAD_STATUS_RUNNING, sizeof(PAYLOAD_STATUS_RUNNING));
    }
    /* open command */
    if(strcmp("o", (char*)(data + PAYLOAD_OFFSET)))
    {
        memcpy(hmi_storage.PAYLOAD_STATUS, PAYLOAD_STATUS_OPENED, sizeof(PAYLOAD_STATUS_OPENED));
    }                        
    else
    /* close command */
    if(strcmp("c", (char*)(data + PAYLOAD_OFFSET)))
    {
        memcpy(hmi_storage.PAYLOAD_STATUS, PAYLOAD_STATUS_CLOSED, sizeof(RECOV_STATUS_CLOSED));
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

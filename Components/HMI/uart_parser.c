
#include "freertos.h"

#include "task.h"
#include "queue.h"

#include "uart_parser.h"
#include "MS1_config.h"
#include "stdint.h"
#include "leds.h"

#include "string.h"
#include "stdlib.h"

STRUCT_UART_STORAGE_t uart_storage = {0};

QueueHandle_t QueueHandle_uart;
TaskHandle_t TaskHandle_uart;

static void handler_uart(void* parameters);

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

/** ************************************************************* *
 * @brief       
 * 
 * @param       parameters 
 * ************************************************************* **/
void handler_uart(void* parameters)
{
    uint8_t frame[FRAME_SIZE] = {0};
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

                    default:                                                             break;

                }
            }
        }
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * ************************************************************* **/
void uart_init(STRUCT_UART_STORAGE_t* parsed_data)
{
    BaseType_t status;

    parsed_data = &uart_storage;

    QueueHandle_uart = xQueueCreate (32, sizeof(uint8_t*));
    status = xTaskCreate(handler_uart, "task_uart", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_APP_UART, &TaskHandle_uart);
    configASSERT(status == pdPASS);
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       cmd 
 * ************************************************************* **/
void uart_parser_callback(uint8_t* data)
{
    xQueueSend(QueueHandle_uart, &data, (TickType_t)0);
}


/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_app_phase(uint8_t* data)
{
    memcpy(uart_storage.APP_PHASE, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));

    /* phase wait */
    if(!strcmp("wait", (char*)(data + PAYLOAD_OFFSET)))
    {
        leds_send_cmd(E_LIST_LED4, E_CMD_LEDS_GREEN);
    }
    else
    /* phase ascend */
    if(!strcmp("ascend", (char*)(data + PAYLOAD_OFFSET)))
    {
        leds_send_cmd(E_LIST_LED4, E_CMD_LEDS_RED);
    }
    else
    /* phase descend */
    if(!strcmp("descend", (char*)(data + PAYLOAD_OFFSET)))
    {
        leds_send_cmd(E_LIST_LED4, E_CMD_LEDS_NONE);
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
    if(!strcmp("in", (char*)(data + PAYLOAD_OFFSET)))
    {
        memcpy(uart_storage.APP_WINDOW, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
        leds_send_cmd(E_LIST_LED4, E_CMD_LEDS_BLUE);
    }
    else
    /* window out */
    if(!strcmp("out", (char*)(data + PAYLOAD_OFFSET)))
    {
        memcpy(uart_storage.APP_WINDOW, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
        leds_send_cmd(E_LIST_LED4, E_CMD_LEDS_NONE);
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_app_aeroc(uint8_t* data)
{
    memcpy(uart_storage.APP_AEROC, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
    leds_send_cmd(E_LIST_LED5, E_CMD_LEDS_RED);
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_sens_imu_ax(uint8_t* data)
{
    memcpy(uart_storage.SENS_IMU_AX, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_sens_imu_ay(uint8_t* data)
{
    memcpy(uart_storage.SENS_IMU_AY, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_sens_imu_az(uint8_t* data)
{
    memcpy(uart_storage.SENS_IMU_AZ, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_sens_imu_gx(uint8_t* data)
{
    memcpy(uart_storage.SENS_IMU_GX, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_sens_imu_gy(uint8_t* data)
{
    memcpy(uart_storage.SENS_IMU_GY, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_sens_imu_gz(uint8_t* data)
{
    memcpy(uart_storage.SENS_IMU_GZ, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_sens_imu_temp(uint8_t* data)
{
    memcpy(uart_storage.SENS_IMU_TEMP, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_sens_imu_error(uint8_t* data)
{
    memcpy(uart_storage.SENS_IMU_ERROR, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_sens_baro_press(uint8_t* data)
{
    memcpy(uart_storage.SENS_BARO_PRESS, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_sens_baro_temp(uint8_t* data)
{
    memcpy(uart_storage.SENS_BARO_TEMP, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_sens_baro_error(uint8_t* data)
{
    memcpy(uart_storage.SENS_BARO_ERROR, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_mntr_bat_seq(uint8_t* data)
{
    memcpy(uart_storage.MNTR_BAT_SEQ, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
    
    /* battery ok */
    if(!strcmp("ok", (char*)(data + PAYLOAD_OFFSET)))
    {
        leds_send_cmd(E_LIST_LED1, E_CMD_LEDS_GREEN);
    }
    else
    /* battery ko */
    if(!strcmp("ko", (char*)(data + PAYLOAD_OFFSET)))
    {
        leds_send_cmd(E_LIST_LED1, E_CMD_LEDS_RED);
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_mntr_bat_motor1(uint8_t* data)
{
    memcpy(uart_storage.MNTR_BAT_MOTOR1, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
    
    /* battery ok */
    if(!strcmp("ok", (char*)(data + PAYLOAD_OFFSET)))
    {
        leds_send_cmd(E_LIST_LED2, E_CMD_LEDS_GREEN);
    }
    else
    /* battery ko */
    if(!strcmp("ko", (char*)(data + PAYLOAD_OFFSET)))
    {
        leds_send_cmd(E_LIST_LED2, E_CMD_LEDS_RED);
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_mntr_bat_motor2(uint8_t* data)
{
    memcpy(uart_storage.MNTR_BAT_MOTOR2, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));

    /* battery ok */
    if(!strcmp("ok", (char*)(data + PAYLOAD_OFFSET)))
    {
        leds_send_cmd(E_LIST_LED3, E_CMD_LEDS_GREEN);
    }
    else
    /* battery ko */
    if(!strcmp("ko", (char*)(data + PAYLOAD_OFFSET)))
    {
        leds_send_cmd(E_LIST_LED3, E_CMD_LEDS_RED);
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_recov_last_cmd(uint8_t* data)
{
    memcpy(uart_storage.RECOV_LAST_CMD, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_recov_status(uint8_t* data)
{
    memcpy(uart_storage.RECOV_STATUS, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_payload_last_cmd(uint8_t* data)
{
    memcpy(uart_storage.PAYLOAD_LAST_CMD, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static void id_parser_payload_status(uint8_t* data)
{
    memcpy(uart_storage.PAYLOAD_STATUS, (char*)(data + PAYLOAD_OFFSET), sizeof(PAYLOAD_SIZE));
}

STRUCT_UART_STORAGE_t* uart_storage_attach(void)
{
    return &uart_storage;
}

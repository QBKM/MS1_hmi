
#include "freertos.h"

#include "task.h"
#include "queue.h"

#include "uart_parser.h"
#include "MS1_config.h"
#include "stdint.h"
#include "leds.h"

#include "string.h"
#include "stdlib.h"

#include "TinyFrame.h"

STRUCT_UART_STORAGE_t uart_storage = {0};

TinyFrame *TinyFrame_RX;

QueueHandle_t QueueHandle_uart;
TaskHandle_t TaskHandle_uart;

static void handler_uart(void* parameters);

/* UART id parsing fucntions */
TF_Result id_parser_app_phase(TinyFrame *tf, TF_Msg *msg);
static TF_Result id_parser_app_window(TinyFrame *tf, TF_Msg *msg);
static TF_Result id_parser_app_aeroc(TinyFrame *tf, TF_Msg *msg);
static TF_Result id_parser_sens_imu_ax(TinyFrame *tf, TF_Msg *msg);
static TF_Result id_parser_sens_imu_ay(TinyFrame *tf, TF_Msg *msg);
static TF_Result id_parser_sens_imu_az(TinyFrame *tf, TF_Msg *msg);
static TF_Result id_parser_sens_imu_gx(TinyFrame *tf, TF_Msg *msg);
static TF_Result id_parser_sens_imu_gy(TinyFrame *tf, TF_Msg *msg);
static TF_Result id_parser_sens_imu_gz(TinyFrame *tf, TF_Msg *msg);
static TF_Result id_parser_sens_imu_error(TinyFrame *tf, TF_Msg *msg);
static TF_Result id_parser_sens_imu_temp(TinyFrame *tf, TF_Msg *msg);
static TF_Result id_parser_sens_baro_press(TinyFrame *tf, TF_Msg *msg);
static TF_Result id_parser_sens_baro_temp(TinyFrame *tf, TF_Msg *msg);
static TF_Result id_parser_sens_baro_error(TinyFrame *tf, TF_Msg *msg);
static TF_Result id_parser_mntr_bat_seq(TinyFrame *tf, TF_Msg *msg);
static TF_Result id_parser_mntr_bat_motor1(TinyFrame *tf, TF_Msg *msg);
static TF_Result id_parser_mntr_bat_motor2(TinyFrame *tf, TF_Msg *msg);
static TF_Result id_parser_recov_last_cmd(TinyFrame *tf, TF_Msg *msg);
static TF_Result id_parser_recov_status(TinyFrame *tf, TF_Msg *msg);
static TF_Result id_parser_payload_last_cmd(TinyFrame *tf, TF_Msg *msg);
static TF_Result id_parser_payload_status(TinyFrame *tf, TF_Msg *msg);

/** ************************************************************* *
 * @brief       
 * 
 * @param       parameters 
 * ************************************************************* **/
void handler_uart(void* parameters)
{
    uint8_t frame[FRAME_SIZE] = {0};

    while(1)
    {
        if(xQueueReceive(QueueHandle_uart, frame, portMAX_DELAY) == pdTRUE)
        {
            TF_Accept(TinyFrame_RX, frame, 128);
        }
    }
}

/** ************************************************************* *
 * @brief       This function should be defined in the 
 *              application code. It implements the lowest 
 *              layer - sending bytes to UART (or other)
 * 
 * @param       tf 
 * @param       buff 
 * @param       len 
 * ************************************************************* **/
void TF_WriteImpl(TinyFrame *tf, const uint8_t *buff, uint32_t len)
{
    // only used for TX
}

/** ************************************************************* *
 * @brief       
 * 
 * ************************************************************* **/
void uart_init(void)
{
    BaseType_t status;

    TinyFrame_RX = TF_Init(TF_SLAVE); // 1 = master, 0 = slave

    /* subscribe for all message id */
    TF_AddTypeListener(TinyFrame_RX, HMI_ID_APP_PHASE, id_parser_app_phase);
    TF_AddTypeListener(TinyFrame_RX, HMI_ID_APP_WINDOW, id_parser_app_window);
    TF_AddTypeListener(TinyFrame_RX, HMI_ID_APP_AEROC, id_parser_app_aeroc);
    TF_AddTypeListener(TinyFrame_RX, HMI_ID_SENS_IMU_AX, id_parser_sens_imu_ax);
    TF_AddTypeListener(TinyFrame_RX, HMI_ID_SENS_IMU_AY, id_parser_sens_imu_ay);
    TF_AddTypeListener(TinyFrame_RX, HMI_ID_SENS_IMU_AZ, id_parser_sens_imu_az);
    TF_AddTypeListener(TinyFrame_RX, HMI_ID_SENS_IMU_GX, id_parser_sens_imu_gx);
    TF_AddTypeListener(TinyFrame_RX, HMI_ID_SENS_IMU_GY, id_parser_sens_imu_gy);
    TF_AddTypeListener(TinyFrame_RX, HMI_ID_SENS_IMU_GZ, id_parser_sens_imu_gz);
    TF_AddTypeListener(TinyFrame_RX, HMI_ID_SENS_IMU_ERROR, id_parser_sens_imu_error);
    TF_AddTypeListener(TinyFrame_RX, HMI_ID_SENS_IMU_TEMP, id_parser_sens_imu_temp);
    TF_AddTypeListener(TinyFrame_RX, HMI_ID_SENS_BARO_PRESS, id_parser_sens_baro_press);
    TF_AddTypeListener(TinyFrame_RX, HMI_ID_SENS_BARO_TEMP, id_parser_sens_baro_temp);
    TF_AddTypeListener(TinyFrame_RX, HMI_ID_SENS_BARO_ERROR, id_parser_sens_baro_error);
    TF_AddTypeListener(TinyFrame_RX, HMI_ID_MNTR_BAT_SEQ, id_parser_mntr_bat_seq);
    TF_AddTypeListener(TinyFrame_RX, HMI_ID_MNTR_BAT_MOTOR1, id_parser_mntr_bat_motor1);
    TF_AddTypeListener(TinyFrame_RX, HMI_ID_MNTR_BAT_MOTOR2, id_parser_mntr_bat_motor2);
    TF_AddTypeListener(TinyFrame_RX, HMI_ID_RECOV_LAST_CMD, id_parser_recov_last_cmd);
    TF_AddTypeListener(TinyFrame_RX, HMI_ID_RECOV_STATUS, id_parser_recov_status);
    TF_AddTypeListener(TinyFrame_RX, HMI_ID_PAYLOAD_LAST_CMD, id_parser_payload_last_cmd);
    TF_AddTypeListener(TinyFrame_RX, HMI_ID_PAYLOAD_STATUS, id_parser_payload_status);

    QueueHandle_uart = xQueueCreate (16, FRAME_SIZE * sizeof(uint8_t));
    status = xTaskCreate(handler_uart, "task_uart", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_APP_UART, &TaskHandle_uart);
    configASSERT(status == pdPASS);
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       cmd 
 * ************************************************************* **/
void uart_parser_callback(uint8_t* frame)
{
    xQueueSendFromISR(QueueHandle_uart, frame, pdFALSE);
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
TF_Result id_parser_app_phase(TinyFrame *tf, TF_Msg *msg)
{
    memcpy(uart_storage.APP.PHASE, (char*)(tf->data), sizeof(PAYLOAD_SIZE));

    /* phase wait */
    if(!strcmp("wait", (char*)(tf->data)))
    {
        leds_send_cmd(E_LIST_LED4, E_CMD_LEDS_GREEN);
    }
    else
    /* phase ascend */
    if(!strcmp("ascend", (char*)(tf->data)))
    {
        leds_send_cmd(E_LIST_LED4, E_CMD_LEDS_RED);
    }
    else
    /* phase descend */
    if(!strcmp("descend", (char*)(tf->data)))
    {
        leds_send_cmd(E_LIST_LED4, E_CMD_LEDS_NONE);
    }

    return TF_STAY;
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static TF_Result id_parser_app_window(TinyFrame *tf, TF_Msg *msg)
{
    memcpy(uart_storage.APP.WINDOW, (char*)(tf->data), sizeof(PAYLOAD_SIZE));

    /* window in */
    if(!strcmp("in", (char*)(tf->data)))
    {
        leds_send_cmd(E_LIST_LED4, E_CMD_LEDS_BLUE);
    }
    else
    /* window out */
    if(!strcmp("out", (char*)(tf->data)))
    {
        leds_send_cmd(E_LIST_LED4, E_CMD_LEDS_NONE);
    }

    return TF_STAY;
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static TF_Result id_parser_app_aeroc(TinyFrame *tf, TF_Msg *msg)
{
    memcpy(uart_storage.APP.AEROC, (char*)(tf->data), sizeof(PAYLOAD_SIZE));
    leds_send_cmd(E_LIST_LED5, E_CMD_LEDS_RED);
    return TF_STAY;
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static TF_Result id_parser_sens_imu_ax(TinyFrame *tf, TF_Msg *msg)
{
    memcpy(uart_storage.SENSOR.IMU_AX, (char*)(tf->data), sizeof(PAYLOAD_SIZE));
    return TF_STAY;
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static TF_Result id_parser_sens_imu_ay(TinyFrame *tf, TF_Msg *msg)
{
    memcpy(uart_storage.SENSOR.IMU_AY, (char*)(tf->data), sizeof(PAYLOAD_SIZE));
    return TF_STAY;
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static TF_Result id_parser_sens_imu_az(TinyFrame *tf, TF_Msg *msg)
{
    memcpy(uart_storage.SENSOR.IMU_AZ, (char*)(tf->data), sizeof(PAYLOAD_SIZE));
    return TF_STAY;
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static TF_Result id_parser_sens_imu_gx(TinyFrame *tf, TF_Msg *msg)
{
    memcpy(uart_storage.SENSOR.IMU_GX, (char*)(tf->data), sizeof(PAYLOAD_SIZE));
    return TF_STAY;
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static TF_Result id_parser_sens_imu_gy(TinyFrame *tf, TF_Msg *msg)
{
    memcpy(uart_storage.SENSOR.IMU_GY, (char*)(tf->data), sizeof(PAYLOAD_SIZE));
    return TF_STAY;
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static TF_Result id_parser_sens_imu_gz(TinyFrame *tf, TF_Msg *msg)
{
    memcpy(uart_storage.SENSOR.IMU_GZ, (char*)(tf->data), sizeof(PAYLOAD_SIZE));
    return TF_STAY;
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static TF_Result id_parser_sens_imu_temp(TinyFrame *tf, TF_Msg *msg)
{
    memcpy(uart_storage.SENSOR.IMU_TEMP, (char*)(tf->data), sizeof(PAYLOAD_SIZE));
    return TF_STAY;
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static TF_Result id_parser_sens_imu_error(TinyFrame *tf, TF_Msg *msg)
{
    memcpy(uart_storage.ERROR.IMU, (char*)(tf->data), sizeof(PAYLOAD_SIZE));
    return TF_STAY;
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static TF_Result id_parser_sens_baro_press(TinyFrame *tf, TF_Msg *msg)
{
    memcpy(uart_storage.SENSOR.BARO_PRESS, (char*)(tf->data), sizeof(PAYLOAD_SIZE));
    return TF_STAY;
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static TF_Result id_parser_sens_baro_temp(TinyFrame *tf, TF_Msg *msg)
{
    memcpy(uart_storage.SENSOR.BARO_TEMP, (char*)(tf->data), sizeof(PAYLOAD_SIZE));
    return TF_STAY;
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static TF_Result id_parser_sens_baro_error(TinyFrame *tf, TF_Msg *msg)
{
    memcpy(uart_storage.ERROR.BARO, (char*)(tf->data), sizeof(PAYLOAD_SIZE));
    return TF_STAY;
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static TF_Result id_parser_mntr_bat_seq(TinyFrame *tf, TF_Msg *msg)
{
    memcpy(uart_storage.MNTR.BAT_SEQ, (char*)(tf->data), sizeof(PAYLOAD_SIZE));
    
    /* battery ok */
    if(!strcmp("ok", (char*)(tf->data)))
    {
        leds_send_cmd(E_LIST_LED1, E_CMD_LEDS_GREEN);
    }
    else
    /* battery ko */
    if(!strcmp("ko", (char*)(tf->data)))
    {
        leds_send_cmd(E_LIST_LED1, E_CMD_LEDS_RED);
    }

    return TF_STAY;
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static TF_Result id_parser_mntr_bat_motor1(TinyFrame *tf, TF_Msg *msg)
{
    memcpy(uart_storage.MNTR.BAT_MOTOR1, (char*)(tf->data), sizeof(PAYLOAD_SIZE));
    
    /* battery ok */
    if(!strcmp("ok", (char*)(tf->data)))
    {
        leds_send_cmd(E_LIST_LED2, E_CMD_LEDS_GREEN);
    }
    else
    /* battery ko */
    if(!strcmp("ko", (char*)(tf->data)))
    {
        leds_send_cmd(E_LIST_LED2, E_CMD_LEDS_RED);
    }

    return TF_STAY;
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static TF_Result id_parser_mntr_bat_motor2(TinyFrame *tf, TF_Msg *msg)
{
    memcpy(uart_storage.MNTR.BAT_MOTOR2, (char*)(tf->data), sizeof(PAYLOAD_SIZE));

    /* battery ok */
    if(!strcmp("ok", (char*)(tf->data)))
    {
        leds_send_cmd(E_LIST_LED3, E_CMD_LEDS_GREEN);
    }
    else
    /* battery ko */
    if(!strcmp("ko", (char*)(tf->data)))
    {
        leds_send_cmd(E_LIST_LED3, E_CMD_LEDS_RED);
    }

    return TF_STAY;
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static TF_Result id_parser_recov_last_cmd(TinyFrame *tf, TF_Msg *msg)
{
    memcpy(uart_storage.RECOVERY.LAST_CMD, (char*)(tf->data), sizeof(PAYLOAD_SIZE));
    return TF_STAY;
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static TF_Result id_parser_recov_status(TinyFrame *tf, TF_Msg *msg)
{
    memcpy(uart_storage.RECOVERY.STATUS, (char*)(tf->data), sizeof(PAYLOAD_SIZE));
    return TF_STAY;
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static TF_Result id_parser_payload_last_cmd(TinyFrame *tf, TF_Msg *msg)
{
    memcpy(uart_storage.PAYLOAD.LAST_CMD, (char*)(tf->data), sizeof(PAYLOAD_SIZE));
    return TF_STAY;
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * ************************************************************* **/
static TF_Result id_parser_payload_status(TinyFrame *tf, TF_Msg *msg)
{
    memcpy(uart_storage.PAYLOAD.STATUS, (char*)(tf->data), sizeof(PAYLOAD_SIZE));
    return TF_STAY;
}

/** ************************************************************* *
 * @brief       
 * 
 * @return      STRUCT_UART_STORAGE_t* 
 * ************************************************************* **/
STRUCT_UART_STORAGE_t* uart_storage_attach(void)
{
   return &uart_storage;
}

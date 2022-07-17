/** ************************************************************* *
 * @file        uart_parser.h
 * @brief       
 * 
 * @date        2021-10-16
 * @author      Quentin Bakrim (quentin.bakrim@hotmail.fr)
 * 
 * Mines Space
 * 
 * ************************************************************* **/

#ifndef HMI_INC_UART_PARSER_H_
#define HMI_INC_UART_PARSER_H_

#include "uart_list.h"
#include "freertos.h"

#define FRAME_SIZE      25
#define PAYLOAD_SIZE    16

typedef struct
{
    uint8_t PHASE[PAYLOAD_SIZE];
    uint8_t AEROC[PAYLOAD_SIZE];
    uint8_t WINDOW[PAYLOAD_SIZE];
    uint8_t APOGEE[PAYLOAD_SIZE];
}STRUCT_UART_APP_t;

typedef struct
{
    uint8_t IMU_X_KALMAN[PAYLOAD_SIZE];
    uint8_t IMU_Y_KALMAN[PAYLOAD_SIZE];
    uint8_t BARO_PRESS[PAYLOAD_SIZE];
}STRUCT_UART_SENSOR_t;

typedef struct
{
    uint8_t BAT_SEQ[PAYLOAD_SIZE];
    uint8_t BAT_MOTOR1[PAYLOAD_SIZE];
    uint8_t BAT_MOTOR2[PAYLOAD_SIZE];
}STRUCT_UART_MNTR_t;

typedef struct
{
    uint8_t LAST_CMD[PAYLOAD_SIZE];
    uint8_t STATUS[PAYLOAD_SIZE];
}STRUCT_UART_RECOVERY_t;

typedef struct
{
    uint8_t LAST_CMD[PAYLOAD_SIZE];
    uint8_t STATUS[PAYLOAD_SIZE];
}STRUCT_UART_PAYLOAD_t;


typedef struct
{
    STRUCT_UART_APP_t       APP;
    STRUCT_UART_SENSOR_t    SENSOR;
    STRUCT_UART_MNTR_t      MNTR;
    STRUCT_UART_RECOVERY_t  RECOVERY;
    STRUCT_UART_PAYLOAD_t   PAYLOAD;
}STRUCT_UART_STORAGE_t;


void uart_init(void);
void uart_parser_callback(uint8_t* frame);
STRUCT_UART_STORAGE_t* uart_storage_attach(void);

#endif

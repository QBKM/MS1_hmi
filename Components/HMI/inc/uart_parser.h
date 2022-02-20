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

#define  FRAME_SIZE      16
#define ID_SIZE         2
#define PAYLOAD_SIZE    (FRAME_SIZE - ID_SIZE)

#define ID_OFFSET       1
#define PAYLOAD_OFFSET  4

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
}STRUCT_UART_STORAGE_t;

void uart_init(STRUCT_UART_STORAGE_t* parsed_data);
void uart_parser_callback(uint8_t* data);
STRUCT_UART_STORAGE_t* uart_storage_attach(void);

#endif

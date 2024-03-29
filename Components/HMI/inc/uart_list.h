/** ************************************************************* *
 * @file        API_HMI.h
 * @brief       
 * 
 * @date        2021-10-16
 * @author      Quentin Bakrim (quentin.bakrim@hotmail.fr)
 * 
 * Mines Space
 * 
 * ************************************************************* **/

#ifndef HMI_INC_API_HMI_H_
#define HMI_INC_API_HMI_H_

/* ------------------------------------------------------------- --
   includes
-- ------------------------------------------------------------- */
#include "stdint.h"

/* ------------------------------------------------------------- --
   defines
-- ------------------------------------------------------------- */ 
/* default value */
#define HMI_ID_NONE                 (uint8_t)0x00

/* application IDs */
#define HMI_ID_APP_PHASE            (uint8_t)0x10
#define HMI_ID_APP_AEROC            (uint8_t)0x11
#define HMI_ID_APP_WINDOW           (uint8_t)0x12
#define HMI_ID_APP_RECOV_APOGEE     (uint8_t)0x13

/* sensor IDs */
#define HMI_ID_SENS_IMU_AX          (uint8_t)0x20
#define HMI_ID_SENS_IMU_AY          (uint8_t)0x21
#define HMI_ID_SENS_IMU_AZ          (uint8_t)0x22
#define HMI_ID_SENS_IMU_GX          (uint8_t)0x23
#define HMI_ID_SENS_IMU_GY          (uint8_t)0x24
#define HMI_ID_SENS_IMU_GZ          (uint8_t)0x25
#define HMI_ID_SENS_IMU_X_KALMAN    (uint8_t)0x26
#define HMI_ID_SENS_IMU_Y_KALMAN    (uint8_t)0x27
#define HMI_ID_SENS_IMU_TEMP        (uint8_t)0x28
#define HMI_ID_SENS_IMU_ERROR       (uint8_t)0x29
#define HMI_ID_SENS_BARO_PRESS      (uint8_t)0x2A
#define HMI_ID_SENS_BARO_TEMP       (uint8_t)0x2B
#define HMI_ID_SENS_BARO_ERROR      (uint8_t)0x2C

/* monitoring IDs */
#define HMI_ID_MNTR_BAT_SEQ         (uint8_t)0x30
#define HMI_ID_MNTR_BAT_MOTOR1      (uint8_t)0x31
#define HMI_ID_MNTR_BAT_MOTOR2      (uint8_t)0x32

/* recovery IDs */
#define HMI_ID_RECOV_LAST_CMD       (uint8_t)0x40
#define HMI_ID_RECOV_STATUS         (uint8_t)0x41

/* payload IDs */
#define HMI_ID_PAYLOAD_LAST_CMD     (uint8_t)0x50
#define HMI_ID_PAYLOAD_STATUS       (uint8_t)0x51

/* ------------------------------------------------------------- --
   end of file
-- ------------------------------------------------------------- */

#endif /* HMI_INC_API_HMI_H_ */

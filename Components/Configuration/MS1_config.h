/** ************************************************************* *
 * @file        MS1_config.h
 * @brief       
 * 
 * @date        2021-11-29
 * @author      Quentin Bakrim (quentin.bakrim@hotmail.fr)
 * 
 * Mines Space
 * 
 * ************************************************************* **/

#ifndef _MS1_CONFIG_H_
#define _MS1_CONFIG_H_

/* ------------------------------------------------------------- --
   includes
-- ------------------------------------------------------------- */
#include "stdint.h"

/* ------------------------------------------------------------- --
   defines
-- ------------------------------------------------------------- */
/* TASK PRIORITIES */
#define TASK_PRIORITY_APP_LEDS   (uint32_t)4     /* Application */
#define TASK_PRIORITY_APP_OLED   (uint32_t)5     /* Application */
#define TASK_PRIORITY_APP_UART   (uint32_t)6     /* Application */


#endif /* _MS1_CONFIG_H_ */
/* ------------------------------------------------------------- --
   end of file
-- ------------------------------------------------------------- */
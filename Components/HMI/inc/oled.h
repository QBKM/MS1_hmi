/** ************************************************************* *
 * @file        API_oled.h
 * @brief       
 * 
 * @date        2021-12-08
 * @author      Quentin Bakrim (quentin.bakrim@hotmail.fr)
 * 
 * Mines Space
 * 
 * ************************************************************* **/
#ifndef OLED_INC_API_OLED_H_
#define OLED_INC_API_OLED_H_

/* ------------------------------------------------------------- --
   includes
-- ------------------------------------------------------------- */
#include "stdint.h"
#include "stdbool.h"

/* ------------------------------------------------------------- --
   types
-- ------------------------------------------------------------- */
/* List of commands available for this API. 
 * -> Stop command      : will stop the motors 
 * -> Open/Close command: will run the motors until reach the end */
typedef enum
{
    E_CMD_OLED_NONE,
    E_CMD_OLED_RED,
    E_CMD_OLED_GREEN,
    E_CMD_OLED_BLUE
}ENUM_OLED_CMD_t;

typedef enum
{
    E_LIST_BTN1,
    E_LIST_BTN2,  
    E_LIST_BTN3,
    E_LIST_BTN4
}ENUM_BTN_LIST_t;

/* ------------------------------------------------------------- --
   function propotypes
-- ------------------------------------------------------------- */
void API_OLED_START(void);
void API_OLED_CALLBACK(ENUM_BTN_LIST_t btn);

/* ------------------------------------------------------------- --
   end of file
-- ------------------------------------------------------------- */

#endif /* OLED_INC_API_OLED_H_ */

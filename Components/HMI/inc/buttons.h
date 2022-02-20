
#ifndef _INC_BUTTONS_H_
#define _INC_BUTTONS_H_


typedef enum
{
    E_BTN_NONE,
    E_BTN_1,
    E_BTN_2,  
    E_BTN_3,
    E_BTN_4,
}ENUM_BTN_LIST_t;

void bouttons_init(ENUM_BTN_LIST_t* button);
void callback_buttons(ENUM_BTN_LIST_t btn);

#endif

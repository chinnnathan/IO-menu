#ifndef INC_MENU_API_H_
#define INC_MENU_API_H_

/*
*   menu_api.h will be used to register actions with the menu.
*/

#ifdef __cplusplus
 extern "C" {
#endif
#include <stdlib.h>
#include <stdint.h>

void menu_init(); 

void menu_next();
void menu_prev();
uint16_t menu_entry(uint8_t* pbuff, size_t len);

void blink_led();

#ifdef __cplusplus
}
#endif

#endif /* PROGRAM_INC_OLED_H_ */
#ifndef INC_MENU_API_H_
#define INC_MENU_API_H_

#ifdef __cplusplus
 extern "C" {
#endif
#include <stdlib.h>
#include <stdint.h>
#include "main.h"

void menu_init(I2C_HandleTypeDef* hi2c);

void menu_next();
void menu_prev();
uint16_t menu_entry(uint8_t* pbuff, size_t len);

void blink_led_pattern_1();
void blink_led_pattern_2();
void blink_led_pattern_3();
void blink_led_pattern_4();


#ifdef __cplusplus
}
#endif

#endif /* PROGRAM_INC_OLED_H_ */
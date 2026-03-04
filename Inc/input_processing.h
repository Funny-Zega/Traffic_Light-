/*
 * input_reading.h
 *
 *  Created on: Oct 12, 2025
 *      Author: ACER
 */

#ifndef INC_INPUT_READING_H_
#define INC_INPUT_READING_H_
#include "main.h"
#define NO_OF_BUTTONS 3
#define DURATION_FOR_AUTO_INCREASING 100
#define BUTTON_IS_PRESSED GPIO_PIN_RESET
#define BUTTON_IS_RELEASED GPIO_PIN_SET

GPIO_PinState buttonBuffer[NO_OF_BUTTONS];
GPIO_PinState debounceButtonBuffer1[NO_OF_BUTTONS];
GPIO_PinState debounceButtonBuffer2[NO_OF_BUTTONS];
uint8_t flagForButtonPress1s[NO_OF_BUTTONS];
uint16_t counterForButtonPress1s[NO_OF_BUTTONS];
void button_reading ( void );
unsigned char is_button_pressed ( unsigned char index );
unsigned char is_button_pressed_1s ( unsigned char index );


#endif /* INC_INPUT_READING_H_ */

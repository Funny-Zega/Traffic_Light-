#include "main.h"
#include "input_reading.h"

void button_reading(void) {
    for (uint8_t i = 0; i < NO_OF_BUTTONS; i++) {
        GPIO_TypeDef* GPIOx;
        uint16_t GPIO_Pin;
        switch (i) {
            case 0: GPIOx = GPIOB; GPIO_Pin = Button1_Pin; break;
            case 1: GPIOx = GPIOB; GPIO_Pin = Button2_Pin; break;
            case 2: GPIOx = GPIOB; GPIO_Pin = Button3_Pin; break;
        }
        debounceButtonBuffer2[i] = debounceButtonBuffer1[i];
        debounceButtonBuffer1[i] = HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);
        if (debounceButtonBuffer1[i] == debounceButtonBuffer2[i]) {
            buttonBuffer[i] = debounceButtonBuffer1[i];
        }
        if (buttonBuffer[i] == BUTTON_IS_PRESSED) {
            if (counterForButtonPress1s[i] < DURATION_FOR_AUTO_INCREASING) {
                counterForButtonPress1s[i]++;
            } else {
                flagForButtonPress1s[i] = 1;
            }
        } else {
            counterForButtonPress1s[i] = 0;
            flagForButtonPress1s[i] = 0;
        }
    }
}

unsigned char is_button_pressed(uint8_t index) {
    if (index >= NO_OF_BUTTONS) return 0;
    return (buttonBuffer[index] == BUTTON_IS_PRESSED);
}

unsigned char is_button_pressed_1s(unsigned char index) {
    if (index >= NO_OF_BUTTONS) return 0xff;
    return (flagForButtonPress1s[index] == 1);
}

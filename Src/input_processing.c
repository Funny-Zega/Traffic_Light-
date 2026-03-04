#include "input_processing.h"

static int temp_duration = 0;
static int state = 0;

enum { INIT, RED_GREEN, GREEN_RED };

void turn_off_all_traffic_leds(void) {
    HAL_GPIO_WritePin(GPIOA, LED_RED_Pin | LED_AMBER_Pin | LED_GREEN_Pin |
                           LED_REDVer_Pin | LED_AMBERVer_Pin | LED_GREENVer_Pin, SET);
}

void set_blinking_led(int mode) {
    static int led_state = 0;
    led_state = !led_state;

    switch (mode) {
        case 2: // Red LED blinking
            HAL_GPIO_WritePin(GPIOA, LED_RED_Pin | LED_REDVer_Pin, led_state ? RESET : SET);
            HAL_GPIO_WritePin(GPIOA, LED_AMBER_Pin | LED_GREEN_Pin |
                                  LED_AMBERVer_Pin | LED_GREENVer_Pin, SET);
            break;
        case 3: // Amber LED blinking
            HAL_GPIO_WritePin(GPIOA, LED_AMBER_Pin | LED_AMBERVer_Pin, led_state ? RESET : SET);
            HAL_GPIO_WritePin(GPIOA, LED_RED_Pin | LED_GREEN_Pin |
                                  LED_REDVer_Pin | LED_GREENVer_Pin, SET);
            break;
        case 4: // Green LED blinking
            HAL_GPIO_WritePin(GPIOA, LED_GREEN_Pin | LED_GREENVer_Pin, led_state ? RESET : SET);
            HAL_GPIO_WritePin(GPIOA, LED_RED_Pin | LED_AMBER_Pin |
                                  LED_REDVer_Pin | LED_AMBERVer_Pin, SET);
            break;
        default:
            turn_off_all_traffic_leds();
            break;
    }
}

void fsm_for_input_processing(void) {
    static uint8_t last_button_state[NO_OF_BUTTONS] = {BUTTON_IS_RELEASED, BUTTON_IS_RELEASED, BUTTON_IS_RELEASED};

    button_reading();

    // Button 1: Mode selection
    if (is_button_pressed(0) && last_button_state[0] == BUTTON_IS_RELEASED) {
        mode = (mode % 4) + 1;
        if (mode != 1) {
            turn_off_all_traffic_leds();
            setTimer7SEG(250);
            // Khởi tạo temp_duration dựa trên chế độ
            switch (mode) {
                case 2:
                    temp_duration = red_duration;
                    break;
                case 3:
                    temp_duration = amber_duration;
                    break;
                case 4:
                    temp_duration = green_duration;
                    break;
                case 1:
                    temp_duration = 15; // Giá trị mặc định nếu có lỗi
                    break;
                default:
                    temp_duration = 15; // Giá trị mặc định nếu có lỗi
                    break;
            }
            updateClockBuffer(temp_duration, mode);
        } else {
            state = INIT;
            countdown = red_duration; // Khởi tạo countdown với red_duration
            setTimerLED(1000);
            RedLightPhase(countdown); // Bắt đầu pha đỏ
            updateClockBuffer(countdown, mode); // Cập nhật LED 7 đoạn
        }
    }

    // Button 2: Increase temp_duration
    if (mode >= 2 && is_button_pressed(1) && last_button_state[1] == BUTTON_IS_RELEASED) {
        temp_duration = (temp_duration >= 99) ? 1 : temp_duration + 1;
        updateClockBuffer(temp_duration, mode);
    }

    // Button 3: Set the chosen value
    if (mode >= 2 && is_button_pressed(2) && last_button_state[2] == BUTTON_IS_RELEASED) {
        int blink_interval = (temp_duration * 50 > 1000) ? 1000 : temp_duration * 50;
        setTimer7SEG(blink_interval);
        if (mode == 2) {
            red_duration = temp_duration; // Lưu cho đèn đỏ
        } else if (mode == 3) {
            amber_duration = temp_duration; // Lưu cho đèn vàng
        } else if (mode == 4) {
            green_duration = temp_duration; // Lưu cho đèn xanh
        }
    }

    for (int i = 0; i < NO_OF_BUTTONS; i++) {
        last_button_state[i] = buttonBuffer[i];
    }
}

void RedLightPhase(int countdown) {
    HAL_GPIO_WritePin(GPIOA, LED_RED_Pin, RESET);
    HAL_GPIO_WritePin(GPIOA, LED_AMBER_Pin, SET);
    HAL_GPIO_WritePin(GPIOA, LED_GREEN_Pin, SET);
    HAL_GPIO_WritePin(GPIOA, LED_REDVer_Pin, SET);
    if (countdown <= amber_duration && countdown >= 0) {
        HAL_GPIO_WritePin(GPIOA, LED_AMBERVer_Pin, RESET);
        HAL_GPIO_WritePin(GPIOA, LED_GREENVer_Pin, SET);
    } else {
        HAL_GPIO_WritePin(GPIOA, LED_AMBERVer_Pin, SET);
        HAL_GPIO_WritePin(GPIOA, LED_GREENVer_Pin, RESET);
    }
    updateClockBuffer(countdown, mode);
}

void GreenLightPhase(int countdown) {
    HAL_GPIO_WritePin(GPIOA, LED_RED_Pin, SET);
    HAL_GPIO_WritePin(GPIOA, LED_AMBERVer_Pin, SET);
    HAL_GPIO_WritePin(GPIOA, LED_GREEN_Pin, SET);
    HAL_GPIO_WritePin(GPIOA, LED_REDVer_Pin, RESET);
    if (countdown <= amber_duration) {
        HAL_GPIO_WritePin(GPIOA, LED_AMBER_Pin, RESET);
        HAL_GPIO_WritePin(GPIOA, LED_GREEN_Pin, SET);
    } else {
        HAL_GPIO_WritePin(GPIOA, LED_AMBER_Pin, SET);
        HAL_GPIO_WritePin(GPIOA, LED_GREEN_Pin, RESET);
    }
    updateClockBuffer(countdown, mode);
}

void fsm_mode_normal_run(void) {
    if (timer_flag[0] == 1) {
        setTimerLED(1000); // Chu kỳ 1 giây cho đèn giao thông

        countdown--;
        updateClockBuffer(countdown, mode);

        switch (state) {
            case INIT:
                state = RED_GREEN;
                countdown = red_duration;
                RedLightPhase(countdown);
                break;

            case RED_GREEN:
                RedLightPhase(countdown);
                if (countdown <= 0) {
                    state = GREEN_RED;
                    countdown = green_duration;
                    GreenLightPhase(countdown);
                }
                break;

            case GREEN_RED:
                GreenLightPhase(countdown);
                if (countdown <= 0) {
                    state = RED_GREEN;
                    countdown = red_duration;
                    RedLightPhase(countdown);
                }
                break;

            default:
                turn_off_all_traffic_leds();
                break;
        }
    }

    if (timer7SEG_flag == 1) {
        update7SEG(index_led);
        index_led = (index_led + 1) % MAX_LED;
        setTimer7SEG(250);
    }
}

void fsm_mode_modify_run(void) {
    if (timer7SEG_flag == 1) {
        set_blinking_led(mode);
        updateClockBuffer(temp_duration, mode);
        update7SEG(index_led);
        index_led = (index_led + 1) % MAX_LED;
        setTimer7SEG(250);
    }
}



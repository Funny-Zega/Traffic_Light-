// input_processing.h
#ifndef INC_INPUT_PROCESSING_H_
#define INC_INPUT_PROCESSING_H_

#include "main.h"
#include "input_reading.h"

void turn_off_all_traffic_leds(void);
void set_blinking_led(int mode);
void fsm_for_input_processing(void);
void RedLightPhase(int countdown);
void GreenLightPhase(int countdown);
void fsm_mode_normal_run(void);
void fsm_mode_modify_run(void);

#endif /* INC_INPUT_PROCESSING_H_ */

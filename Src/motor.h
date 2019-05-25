
#include "stm32f3xx_hal.h"

#define FORWARD 0
#define BACKWARD 1


void motor_init(void);
void motorLeft_start(_Bool, uint16_t);
void motorRight_start(_Bool, uint16_t);
void motor_turnLeft(uint8_t);
void motor_turnRight(uint8_t);
void motor_forward(); 
void motor_turn90Left();


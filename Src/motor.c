
#include "motor.h"
#include "tim.h"

#define MAX_PWM 9999  
#define MIN_PWM 500 //pobawić się
#define CORNERING_STEP MAX_PWM/4

void motor_turnLeft(uint8_t angle) {
  uint16_t speedMRight = MAX_PWM;
  uint16_t speedMLeft = (MIN_PWM + MAX_PWM) - CORNERING_STEP*angle;
  motorLeft_start(FORWARD, speedMLeft);
  motorRight_start(FORWARD, speedMRight);
}

void motor_turnRight(uint8_t angle) {
  uint16_t speedMLeft = MAX_PWM;
  uint16_t speedMRight = (MIN_PWM + MAX_PWM) - CORNERING_STEP*angle;
  motorLeft_start(FORWARD, speedMLeft);
  motorRight_start(FORWARD, speedMRight);
}

void motor_forward() {
  motorLeft_start(FORWARD, MAX_PWM);
  motorRight_start(FORWARD, MAX_PWM);
}

void motor_init() {
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
  M_STBY_GPIO_Port->BSRR = M_STBY_Pin;
}

void motorRight_start(_Bool forward, uint16_t speed) {
  TIM4->CCR1 = speed;
  if (forward) {
    M1_IN2_GPIO_Port->BSRR = M1_IN2_Pin;
    M1_IN1_GPIO_Port->BRR = M1_IN1_Pin;
  } else {
    M1_IN2_GPIO_Port->BRR = M1_IN2_Pin;
    M1_IN1_GPIO_Port->BSRR = M1_IN1_Pin;
  }
}
void motorLeft_start(_Bool forward, uint16_t speed) {
  TIM4->CCR2 = speed;
  if (!forward) {
    M2_IN2_GPIO_Port->BSRR = M2_IN2_Pin;
    M2_IN1_GPIO_Port->BRR = M2_IN1_Pin;
  } else {
    M2_IN2_GPIO_Port->BRR = M2_IN2_Pin;
    M2_IN1_GPIO_Port->BSRR = M2_IN1_Pin;
  }
}
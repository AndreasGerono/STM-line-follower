
#include "lineSensor.h"
#include "tim.h"

#define CHARGE_TIME 10 // ??
#define NR_OF_SENSORS 8
#define BLACK_TRESHOLD 1500 //zmienic??

#define TIMER TIM6
#define TIMER_HANDLER &htim6

static _Bool startFlag;
static _Bool nextFlag;
static _Bool finishFlag;
static uint8_t currentSensor;
static uint16_t sensorValues[NR_OF_SENSORS];


static uint16_t sensorPin[] = {
    SENSOR0_Pin, SENSOR1_Pin, SENSOR2_Pin, SENSOR3_Pin,
    SENSOR4_Pin, SENSOR5_Pin, SENSOR6_Pin, SENSOR7_Pin};

static GPIO_TypeDef *sensorGpio[] = {
    SENSOR0_GPIO_Port, SENSOR1_GPIO_Port, SENSOR2_GPIO_Port, SENSOR3_GPIO_Port,
    SENSOR4_GPIO_Port, SENSOR5_GPIO_Port, SENSOR6_GPIO_Port, SENSOR7_GPIO_Port};

_Bool sensor_get(uint8_t sensor) {
  if (sensorValues[sensor] >= BLACK_TRESHOLD)
    return 1;
  else
    return 0;
}

static void outputMode(uint8_t sensor) {
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Pin = sensorPin[sensor];
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(sensorGpio[sensor], &GPIO_InitStruct);
}

static void inputMode(uint8_t sensor) {
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Pin = sensorPin[sensor];
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(sensorGpio[sensor], &GPIO_InitStruct);
}

static void startDischarging() {
  startFlag = RESET;
  nextFlag = SET;
  outputMode(currentSensor);
  sensorGpio[currentSensor]->BSRR = sensorPin[currentSensor];
  TIMER->CNT = RESET;
  HAL_TIM_Base_Start(TIMER_HANDLER);
}

static void stopDischarging() {
  nextFlag = RESET;
  finishFlag = SET;
  inputMode(currentSensor);
  TIMER->CNT = RESET;
}

static void finishMeasure() {
  sensorValues[currentSensor] = TIMER->CNT;
  HAL_TIM_Base_Stop(TIMER_HANDLER);
  startFlag = SET;
  finishFlag = RESET;
 // printf("%u. %u\n", currentSensor,sensorValues[currentSensor]);
  currentSensor++;
}

static _Bool readSensorPin(uint8_t sensor) {
  return HAL_GPIO_ReadPin(sensorGpio[sensor], sensorPin[sensor]); //uwaga!!
}

void sensor_start() {
  startFlag = SET;
  currentSensor = 0;
  LED_ON_GPIO_Port->BSRR = LED_ON_Pin;
}

void sensor_stop() {
  HAL_TIM_Base_Stop(TIMER_HANDLER);
  LED_ON_GPIO_Port->BRR = LED_ON_Pin;
  TIMER->CNT = 0;
  startFlag = RESET;
  nextFlag = RESET;
  finishFlag = RESET;
}

void sensor_measure() {
  currentSensor %= NR_OF_SENSORS;
  if (startFlag)
    startDischarging();
  else if (nextFlag && (TIMER->CNT >= CHARGE_TIME))
    stopDischarging();
  else if ((finishFlag && !readSensorPin(currentSensor)) || (TIMER->CNT >= BLACK_TRESHOLD)) //uwaga!!!
    finishMeasure();
}
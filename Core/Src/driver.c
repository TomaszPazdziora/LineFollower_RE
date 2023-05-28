#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "adc.h"
#include "driver.h"
#define ARR_TIM4 79
#define FORWARD 1
#define BACKWARD 2

bool isWhite[SENSOR_NUMBER];
int readings[SENSOR_NUMBER];

int leftMotorSpeed = 0;
int rightMotorSpeed = 0;

void clearReadings();
void measure();

void moveLeftMotor(int speed);
void moveRightMotor(int speed);


/*--------- PD VARIABLES ----------*/

#define SET_SPEED 70
#define MAX_SPEED ARR_VAL
#define MIN_SPEED 0

//bool isRunning = true;

const float KP = 1;
const float KD = 0.2;
float PD = 0;

// int weights[SENSOR_NUMBER] = {-20, -15, -10, -8, -5, -3, -1, 1, 3, 5, 8, 10, 15, 20};
int weights[SENSOR_NUMBER] = {-30, 30};
int speedDelta = 0;

float error = 0;
float prevError = 0;
int sum = 0;
int sensorsDetected = 0;

void countPDvalues();


/* ---- TASK IMPLEMENTATION ---- */

extern void ExStartDriveTask(void const * argument) {
  /* USER CODE BEGIN StartDriveTask */
  /* Infinite loop */

	// set left motor dir
	HAL_GPIO_WritePin(DRIVR1_GPIO_Port, DRIVR1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(DRIVL1_GPIO_Port, DRIVL1_Pin, GPIO_PIN_SET);

	// set right motor dir
	HAL_GPIO_WritePin(DRIVR2_GPIO_Port, DRIVR2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(DRIVL2_GPIO_Port, DRIVL2_Pin, GPIO_PIN_SET);

  for(;;)
  {
	if (isRunning) {
       measure(); 
	   countPDvalues();
	   TIM4->CCR1 = leftMotorSpeed;
	   TIM4->CCR2 = rightMotorSpeed;
	   osDelay(100);
    }
	else {
		TIM4->CCR1 = 0;
	   	TIM4->CCR2 = 0;
	}
  }
  /* USER CODE END StartDriveTask */
}



/* ---- SENSOR FUNCTIONS ---- */

void measure() {
	HAL_ADC_Start(&hadc1);

	for(int i = 0; i < SENSOR_NUMBER; i++) {
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		readings[i] = HAL_ADC_GetValue(&hadc1);

		if(readings[i] < TRESHOLD) isWhite[i] = true;
		else isWhite[i] = false;
	}
}

void clearReadings() {
	for(int i = 0; i < SENSOR_NUMBER; i++) {
		readings[i] = CLEARED_READING;
		isWhite[i] = false;
	}
}



/* ---- MOTOR FUNCTIONS ---- */


// 79 = 100 % pwm duty
// 0 = 0 % pwm duty
// duty = CRR / ARR
// CRRx where 'x' means channel

void moveLeftMotor(int speed) {
    TIM4->CCR1 = speed;

	HAL_GPIO_WritePin(DRIVR1_GPIO_Port, DRIVR1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(DRIVL1_GPIO_Port, DRIVL1_Pin, GPIO_PIN_SET);
	// HAL_GPIO_WritePin(DRIVR1_GPIO_Port, DRIVR1_Pin, GPIO_PIN_SET);
	// HAL_GPIO_WritePin(DRIVL1_GPIO_Port, DRIVL1_Pin, GPIO_PIN_RESET);
}

void moveRightMotor(int speed) {
    TIM4->CCR2 = speed;
	
	HAL_GPIO_WritePin(DRIVR2_GPIO_Port, DRIVR2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(DRIVL2_GPIO_Port, DRIVL2_Pin, GPIO_PIN_SET);
	// HAL_GPIO_WritePin(DRIVR2_GPIO_Port, DRIVR2_Pin, GPIO_PIN_SET);
	// HAL_GPIO_WritePin(DRIVL2_GPIO_Port, DRIVL2_Pin, GPIO_PIN_RESET);
}



/* ---- PD algorithm ---- */

void countPDvalues() {
	sum = 0;
	speedDelta = 0;
	sensorsDetected = 0;

    for (int i = 0; i < SENSOR_NUMBER; i++) {
		if (isWhite[i] == false) {
			sum += weights[i];
			sensorsDetected++;
		}
	}

	prevError = error;
	error = sum/sensorsDetected;
	speedDelta = (KP * error) + (KD * (error - prevError));

	leftMotorSpeed = SET_SPEED - speedDelta;
	rightMotorSpeed = SET_SPEED + speedDelta;

	if (leftMotorSpeed > MAX_SPEED) leftMotorSpeed = MAX_SPEED;
	else if (leftMotorSpeed < MIN_SPEED) leftMotorSpeed = MIN_SPEED;
	
	if (rightMotorSpeed > MAX_SPEED) rightMotorSpeed = MAX_SPEED;
	else if (rightMotorSpeed < MIN_SPEED) rightMotorSpeed = MIN_SPEED;
}
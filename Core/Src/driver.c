#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "adc.h"

#define ARR_TIM4 79
#define FORWARD 1
#define BACKWARD 2

bool isWhite[SENSOR_NUMBER];
int readings[SENSOR_NUMBER];

int leftMotorSpeed = 0;
int rightMotorSpeed = 0;

void clearReadings();
void measure();

int percentToDuty(int percent);
void moveLeftMotor(int dir, int speedInPercent);
void moveRightMotor(int dir, int speedInPercent);
void moveMotors();


/*--------- PD VARIABLES ----------*/

#define MAX_SPEED 70
bool isRunning = true;

const float KP = 1;
const float KD = 0.2;
float PD = 0;

// int weights[SENSOR_NUMBER] = {-20, -15, -10, -8, -5, -3, -1, 1, 3, 5, 8, 10, 15, 20};
int weights[SENSOR_NUMBER] = {-30, 30};
int speedDelay = 0;

float error = 0;
float prevError = 0;
int average = 0;
int sum = 0;
int sensorsDetected = 0;

void countPDvalues();


/* ---- TASK IMPLEMENTATION ---- */

extern void ExStartDriveTask(void const * argument) {
  /* USER CODE BEGIN StartDriveTask */
  /* Infinite loop */

 

  for(;;)
  {
    if (isRunning) {
       measure(); 
	   countPDvalues();
	   moveMotors();
	   osDelay(100);
    }
	


	// for(int i = 0; i <= 100; i++) {
	// 	moveLeftMotor(FORWARD, i);
	// 	moveRightMotor(BACKWARD, 100 - i);
	// 	osDelay(100);
	// }

	// for(int i = 0; i <= 100; i++) {
	// 	moveLeftMotor(FORWARD, 100 - i);
	// 	moveRightMotor(BACKWARD, i);
	// 	osDelay(100);
	// }
	
	// osDelay(SENSOR_DELAY);
    // measure();
    // osDelay(SENSOR_DELAY);
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

int percentToDuty(int percent) {
	// 79 = 100 % pwm duty
	// 0 = 0 % pwm duty
	// duty = CRR / ARR
	// CRRx where 'x' means channel
	return (percent * (ARR_TIM4+1)/100 - 1);
}

void moveLeftMotor(int dir, int speedInPercent) {
    TIM4->CCR1 = percentToDuty(speedInPercent);

	if (dir == FORWARD) {
		HAL_GPIO_WritePin(DRIVR1_GPIO_Port, DRIVR1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(DRIVL1_GPIO_Port, DRIVL1_Pin, GPIO_PIN_SET);
	}
	else {
		HAL_GPIO_WritePin(DRIVR1_GPIO_Port, DRIVR1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(DRIVL1_GPIO_Port, DRIVL1_Pin, GPIO_PIN_RESET);
	}
}

void moveRightMotor(int dir, int speedInPercent) {
    TIM4->CCR2 = percentToDuty(speedInPercent);

	if (dir == FORWARD) {
		HAL_GPIO_WritePin(DRIVR2_GPIO_Port, DRIVR2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(DRIVL2_GPIO_Port, DRIVL2_Pin, GPIO_PIN_SET);
	}
	else {
		HAL_GPIO_WritePin(DRIVR2_GPIO_Port, DRIVR2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(DRIVL2_GPIO_Port, DRIVL2_Pin, GPIO_PIN_RESET);
	}
}

void moveMotors() {
	moveLeftMotor(FORWARD, leftMotorSpeed);
	moveRightMotor(FORWARD, rightMotorSpeed);
}



/* ---- PD algorithm ---- */

void countPDvalues() {
	average = 0;
	sum = 0;
	speedDelay = 0;
	sensorsDetected = 0;

    for (int i = 0; i < SENSOR_NUMBER; i++) {
		if (isWhite[i] == false) {
			sum += weights[i];
			sensorsDetected++;
		}
	}

	average = sum/sensorsDetected;

	prevError = error;
	error = average;
	speedDelay = (KP * error) + (KD * (error - prevError));

	if (speedDelay > MAX_SPEED) speedDelay = MAX_SPEED;
	if (speedDelay < -MAX_SPEED) speedDelay = -MAX_SPEED;

	if (speedDelay < 0) {
		rightMotorSpeed = MAX_SPEED;
      	leftMotorSpeed = MAX_SPEED - speedDelay;
	}
	else {
      	rightMotorSpeed = MAX_SPEED - speedDelay;
      	leftMotorSpeed = MAX_SPEED;
    }
}
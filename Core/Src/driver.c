#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "adc.h"
#include "driver.h"
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

volatile bool isRunning = false;
float KP = 20;
float KD = 20;
float PD = 0;

volatile bool followLeft = false;
volatile bool lostLine = false;

/*--------- PD VARIABLES ----------*/

#define SET_SPEED 252
#define MAX_SPEED ARR_VAL
#define MIN_SPEED 0

//bool isRunning = true;


int weights[SENSOR_NUMBER] = {0, -20, -15, -10, -8, -5, -3,  3, 5, 8, 10, 15, 20, 0};
// int weights[SENSOR_NUMBER] = {-30, 30};
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
	HAL_GPIO_WritePin(DRIVR2_GPIO_Port, DRIVR2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(DRIVR1_GPIO_Port, DRIVR1_Pin, GPIO_PIN_SET);

	// set right motor dir
	HAL_GPIO_WritePin(DRIVR2_GPIO_Port, DRIVR2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(DRIVL2_GPIO_Port, DRIVL2_Pin, GPIO_PIN_SET);

  for(;;)
  {
    if (isRunning) {
       measure();

       // on trace
       if (!lostLine) {
        countPDvalues();
        TIM4->CCR1 = leftMotorSpeed;
        TIM4->CCR2 = rightMotorSpeed;

        int leftCnt = 0;
        int rightCnt = 0; 

        for(int i = 1; i < 7; i++) {
            if (!isWhite[i]) leftCnt++;
        }
        for(int i = 7; i < 13; i++) {
            if (!isWhite[i]) rightCnt++;
        }

        if (leftCnt > rightCnt) followLeft = true;
        else followLeft = false;

        osDelay(1);
        
       }
       // if line folower lost line
       else {
        if (followLeft) {
            TIM4->CCR1 = 0;
               TIM4->CCR2 = SET_SPEED;
        }
        else {
            TIM4->CCR1 = SET_SPEED;
               TIM4->CCR2 = 0;
        }
       }
       
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
        HAL_ADC_PollForConversion(&hadc1, 1);
        readings[i] = HAL_ADC_GetValue(&hadc1);
    }

	lostLine = true;
	for(int i = 1; i < SENSOR_NUMBER -1; i++) {
		if(readings[i] < TRESHOLD) isWhite[i] = true;
        else {
            isWhite[i] = false;
            lostLine = false;
        }
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
	if (sensorsDetected != 0)
		error = sum/sensorsDetected;
	speedDelta = (KP * error) + (KD * (error - prevError));

	leftMotorSpeed = SET_SPEED + speedDelta;
	rightMotorSpeed = SET_SPEED - speedDelta;

	if (leftMotorSpeed > MAX_SPEED) leftMotorSpeed = MAX_SPEED;
	else if (leftMotorSpeed < MIN_SPEED) leftMotorSpeed = MIN_SPEED;
	
	if (rightMotorSpeed > MAX_SPEED) rightMotorSpeed = MAX_SPEED;
	else if (rightMotorSpeed < MIN_SPEED) rightMotorSpeed = MIN_SPEED;
}
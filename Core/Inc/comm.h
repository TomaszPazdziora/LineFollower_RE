#ifndef __COMM_H__
#define __COMM_H__
#include "usart.h"

#define MSIZE 4

//extern bool isWhite[SENSOR_NUMBER];
//extern int readings[SENSOR_NUMBER];
extern UART_HandleTypeDef huart6;
//bool test1, test2;
enum mode{turnLeft, turnRight, adc, stop, start};
uint8_t mess[MSIZE]= {0,0,0,0};

uint8_t Rx_data[100];  
uint8_t* rxd = Rx_data;
void sendMess(enum mode m, uint32_t value);  
void ExStartCommTask(void const * argument);

uint8_t frameBegin[] = {'/','$','A'};
int frameSize = 3;

#endif
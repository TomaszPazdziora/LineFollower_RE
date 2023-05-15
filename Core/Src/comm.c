#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "usart.h"
#include <string.h>



#define MSIZE 4

  //extern bool isWhite[SENSOR_NUMBER];
  //extern int readings[SENSOR_NUMBER];
  extern UART_HandleTypeDef huart6;
  //bool test1, test2;
  enum mode{turnLeft, turnRight, adc, stop, start};
  uint8_t mess[MSIZE]= {0,0,0,0};

  void sendMess(enum mode m, uint32_t value);  

  void ExStartCommTask(void const * argument){

  //HAL_UART_Transmit(&huart6, mess, MSIZE, 1000);
  for(;;) {
    //test1 = isWhite[0];
    //test2 = isWhite[1];
    //HAL_UART_Transmit(&huart6, mess, MSIZE, 1000);
    //sendMess(adc, 19);

    //sendMess(stop, 0);
    //HAL_UART_Receive(&huart6,buffer, sizeof(buffer),100);
    HAL_GPIO_TogglePin(LED_CTR_GPIO_Port, LED_CTR_Pin);
    osDelay(500);
  }
  }




// void sendMess(enum mode m, uint32_t value){
//   mess[0]=(uint8_t)m;
//   uint32_t tmp;
//   for(int i = 1; i<MSIZE; i++) {
//     tmp = value >> (8*(i-1));
//     mess[MSIZE-i] = tmp & 0xFF;
//   }
//   HAL_UART_Transmit(&huart6, mess, MSIZE, 1000);
//   osDelay(50);
// }


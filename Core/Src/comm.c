#include "comm.h"

extern bool isRunning;

  void ExStartCommTask(void const * argument){
    //USART6_IRQHandler();
    HAL_UART_Receive_IT(&huart6, &Rx_data, 1);
    HAL_UART_Transmit_IT(&huart6, mess, 4);
    for(;;) {
      while (HAL_UART_GetState(&huart6) == HAL_UART_STATE_BUSY_TX ||
      HAL_UART_GetState(&huart6) == HAL_UART_STATE_BUSY_TX_RX);
      
      //HAL_UART_Transmit(&huart6, mess, MSIZE, 1000);
      //sendMess(adc, 19);
      
      //UART_Start_Receive_IT(&huart6,Rx_data, 4);
      osDelay(500);
      sendMess(stop, 120);
      osDelay(500);
      //HAL_UART_Receive(&huart6,Rx_data, 4,100);
      //HAL_UART_RxCpltCallback(&huart6);
    }
  }


//HAL_UART_IRQHandler(&huart6);


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
int xxxx;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {


  xxxx=Rx_data[0]-0x30; //mod pracy;
  
	switch (xxxx) {
 
	case 0: // Jezeli odebrany zostanie znak 0
		//size = sprintf(Data, "STOP\n\r");
		HAL_GPIO_WritePin(LED_CTR_GPIO_Port, LED_CTR_Pin, GPIO_PIN_RESET);
    isRunning = false;
		break;
 
	case 1: // Jezeli odebrany zostanie znak 1
		//size = sprintf(Data, "START\n\r");
		HAL_GPIO_WritePin(LED_CTR_GPIO_Port, LED_CTR_Pin, GPIO_PIN_SET);
    isRunning = true;
		break;
 
	default: // Jezeli odebrano nieobslugiwany znak
		//size = sprintf(Data, "Odebrano nieznany znak: %c\n\r", Rx_data);
		break;
	}

  HAL_UART_Receive_IT(&huart6, &Rx_data, 1);
}


//HAL_UART_Receive_IT(&huart6, Rx_data, 4);


void sendMess(enum mode m, uint32_t value){
  mess[0]=(uint8_t)m;
  uint32_t tmp;
  for(int i = 1; i<MSIZE; i++) {
    tmp = value >> (8*(i-1));
    mess[MSIZE-i] = tmp & 0xFF;
  }
  HAL_UART_Transmit_IT(&huart6, mess, MSIZE);
}



int findFrameBegin(uint8_t *data, int length) {
    int i;
    for (i = 0; i < length - frameSize + 1; i++) {
        int j;
        for (j = 0; j < frameSize; j++) {
            if (data[i + j] != frameBegin[j]) {
                break;
            }
        }
        if (j == frameSize) {
            return i+3;
        }
    }
    return -1;  // Frame not found
}


#include "comm.h"
#include "driver.h"


void ExStartCommTask(void const * argument){
    HAL_UART_Receive_DMA(&huart6, Rx_data, 9);
    HAL_UART_Transmit_IT(&huart6, mess, 4);
    for(;;) {
      while (HAL_UART_GetState(&huart6) == HAL_UART_STATE_BUSY_TX ||
      HAL_UART_GetState(&huart6) == HAL_UART_STATE_BUSY_TX_RX);
      sendMess(stop, 50);
      osDelay(500);
      sendMess(stop, 120);
      osDelay(500);

    }
  }


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

  //if(Rx_data[0] != '#') return;
	switch (Rx_data[1]) {
	case '0': // stop
		//size = sprintf(Data, "STOP\n\r");
		HAL_GPIO_WritePin(LED_CTR_GPIO_Port, LED_CTR_Pin, GPIO_PIN_RESET);
    isRunning = false;
		break;
 
	case '1': // start
		//size = sprintf(Data, "START\n\r");
		HAL_GPIO_WritePin(LED_CTR_GPIO_Port, LED_CTR_Pin, GPIO_PIN_SET);
    isRunning = true;
		break;
  case '2': // setP
		//size = sprintf(Data, "START\n\r");
		HAL_GPIO_WritePin(LED_CTR_GPIO_Port, LED_CTR_Pin, GPIO_PIN_SET);
    //isRunning = true;
		break;
  case '3': // setI
		//size = sprintf(Data, "START\n\r");
		HAL_GPIO_WritePin(LED_CTR_GPIO_Port, LED_CTR_Pin, GPIO_PIN_SET);
    //for(int n = 2; n < 8; n++) KD += (Rx_data[n]-'0')<<(4*(n-2));
		break;
	default: // Jezeli odebrano nieobslugiwany znak
		sendMess(idkmess, 69);
		break;
	}

 // HAL_UART_Receive_IT(&huart6, Rx_data, 1);
  //HAL_UART_Receive_IT(huart, &Rx_data, 8);
    HAL_UART_Receive_DMA(huart, Rx_data, 9);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart){
  int i = 0;
  int b = i+1;
  __HAL_UART_CLEAR_OREFLAG(huart);
  HAL_UART_Receive_DMA(huart, Rx_data, 9);
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
            if (data[(i + j)%length] != frameBegin[j]) {
                break;
            }
        }
        if (j == frameSize) {
            return (i+3)%length;
        }
    }
    return -1;  // Frame not found
}



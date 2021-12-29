/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"

#include "PwmDriver.h"
#include "GpioDriver.h"
#include "MotorDriver.h"
#include "MotorController.h"

#include "Deque.h"

#include "ComProtocol.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
PwmDriver pwmDriver;
GpioDriver enGpioDriver;
GpioDriver dirGpioDriver;

MotorDriver motorDriver;
MotorController motorController;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t rx_buffer[1];
uint8_t tx_buffer[MAX_PACKET_LENGTH];
Deque rxDequeue;

bool control_flag = 0;

void init_all_driver() {
  PwmDriver_init(&pwmDriver, 64, &htim1, TIM_CHANNEL_1, 64, 65535);
  GpioDriver_init(&enGpioDriver, GPIOB, GPIO_PIN_14);
  GpioDriver_init(&dirGpioDriver, GPIOB, GPIO_PIN_15);

  MotorDriver_init(&motorDriver, &pwmDriver, &enGpioDriver, &dirGpioDriver);
  //100Hz
  MotorController_init(&motorController, &motorDriver, 100, 0.1);

}

int try_parse_protocol_from_queue(RxProtocolPacket *data_packet) {
  uint8_t cur_packet[MAX_PACKET_LENGTH];//将包写入buffer

  //先找个包头
  uint8_t temp_lst_head = 0;
  bool find_head_flag = false;
  for (size_t i = 0; i < MAX_PACKET_LENGTH; i++) {
    uint8_t cur = Dequeue_pop_front(&rxDequeue);
    if (temp_lst_head == HD_NUM && cur == ID_NUM) {
      find_head_flag = true;
      break;
    }
    temp_lst_head = cur;
  }
  //没找到包头
  if (find_head_flag == false) return -1;

  cur_packet[0] = HD_NUM;
  cur_packet[1] = ID_NUM;

  // 长度
  size_t cur_packet_length = cur_packet[2] = Dequeue_pop_front(&rxDequeue);

  //长度错误
  printf("len: %d\n", cur_packet_length);
  if (cur_packet_length > MAX_PACKET_LENGTH) return -2;

  // 剩余的deque长度不够现在的包长，把取出来的再塞回去，之后够了再说
  if (cur_packet_length - 3 > rxDequeue.length) {
    Dequeue_push_front(&rxDequeue, cur_packet[2]);
    Dequeue_push_front(&rxDequeue, cur_packet[1]);
    Dequeue_push_front(&rxDequeue, cur_packet[0]);
    return -3;
  }

  //完整读入整个包
  for (size_t i = 3; i < cur_packet_length; i++) {
    cur_packet[i] = Dequeue_pop_front(&rxDequeue);
  }

  data_packet->length = cur_packet_length;
  int8_t parse_res = parse_packet(cur_packet, cur_packet_length, data_packet);

  //解析错误
  if (parse_res != 0) {
    // 把除了包头的内容推回去，认为当前包头错误
    for (size_t i = cur_packet_length - 1; i >= 2; i--) {
      Dequeue_push_front(&rxDequeue, cur_packet[i]);
    }
    return parse_res - 3;
  }

  return 0;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART3_UART_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

  //开启usart1中断
  HAL_UART_Receive_IT(&huart1, rx_buffer, 1);

  //开启定时器2中断
  HAL_TIM_Base_Start_IT(&htim2);

  init_all_driver();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    // 当buffer内长度足够，只取MAX_PACKET_LENGTH以内长度，试图解包
    if (rxDequeue.length > MAX_PACKET_LENGTH) {
      RxProtocolPacket protocol_packet;
      int parse_res = try_parse_protocol_from_queue(&protocol_packet);
      if (parse_res != 0) {
        printf("%d\n", parse_res);
      } else {
        MotorState motor_state;
        motor_state.mode = protocol_packet.inner_msg.axle_cmd[1].mode;
        motor_state.velocity = (double) protocol_packet.inner_msg.axle_cmd[1].velocity / 60;// 从转每分钟转为转每秒
        motor_state.position = protocol_packet.inner_msg.axle_cmd[1].position;
        motor_state.current = protocol_packet.inner_msg.axle_cmd[1].current;

        MotorController_set_target(&motorController, motor_state);
      }

    }

    // 按周期控制 100Hz
    if (control_flag) {
      control_flag = false;
      MotorController_control(&motorController);
      printf("control\n");

//      ProtocolPacket data_packet;
//
//      for (size_t i = 1; i < 2; i++) {
//        data_packet.inner_msg.axle_cmd[i].mode = (int16_t) (motorController.cur_motor_state.mode);
//        data_packet.inner_msg.axle_cmd[i].current = (int16_t) (motorController.cur_motor_state.current * 100);
//        data_packet.inner_msg.axle_cmd[i].position = (int16_t) (motorController.cur_motor_state.position * 100);
//        data_packet.inner_msg.axle_cmd[i].velocity = (int16_t) (motorController.cur_motor_state.velocity * 100);
//
//      }
//
//      dump_packet(tx_buffer, MAX_PACKET_LENGTH, &data_packet);
//      HAL_UART_Transmit_IT(&huart3, tx_buffer, data_packet.length);
    }
  }


  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
      | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
uint8_t lst = 0;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  if (huart->Instance == USART1) {
    Dequeue_push_back(&rxDequeue, rx_buffer[0]);
    HAL_UART_Receive_IT(&huart1, rx_buffer, 1);
  }
//  if (huart->Instance == USART3) {
//    Dequeue_push_back(&dequeue, rx_buffer[0]);
//    HAL_UART_Receive_IT(&huart3, rx_buffer, 1);
//  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim == (&htim2)) {
    control_flag = true;
  }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

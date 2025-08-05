/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include "oled.h"
#include "esp01s.h"
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#include "stdio.h"

int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}

#include "los_task.h"
//task1

static void TaskEntry1(void) {
//		// 订阅主题
//    if (!MQTT_Subscribe("stm32/clear", 0)) {
//        printf("[MQTT] Subscription failed!\n");
//    } else {
//        printf("[MQTT] Subscribed to stm32/clear\n");
//    }
	while (1)  {
		
           MQTT_ProcessReceivedData();
//		     printf("Current MQTT Payload: %s\n", mqtt_payload_buffer);
        if (strcmp(mqtt_payload_buffer, "0") == 0) {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); // 关洒水
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET); // 关打扫
            OLED_Clear();
            OLED_ShowString(2, 2, "all off", 12, 0);
					  LOS_Msleep(2);
					  printf("Current MQTT Payload: %s\n", mqtt_payload_buffer);
        } else if (strcmp(mqtt_payload_buffer, "1") == 0) {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);   // 开洒水
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET); // 关打扫
            OLED_Clear();
            OLED_ShowString(2, 2, "watering", 12, 0);
					  LOS_Msleep(2);
					  printf("Current MQTT Payload: %s\n", mqtt_payload_buffer);
        } else if (strcmp(mqtt_payload_buffer, "2") == 0) {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); // 关洒水
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);   // 开打扫
            OLED_Clear();
            OLED_ShowString(2, 2, "cleaning", 12, 0);
					  LOS_Msleep(2);
					printf("Current MQTT Payload: %s\n", mqtt_payload_buffer);
        } else if (strcmp(mqtt_payload_buffer, "3") == 0) {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);   // 开洒水
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);   // 开打扫
            OLED_Clear();
            OLED_ShowString(2, 2, "both on", 12, 0);
					  LOS_Msleep(2);
					printf("Current MQTT Payload: %s\n", mqtt_payload_buffer);
        }
	
           } 
}                	

//task2
static void TaskEntry2(void) {
	while (1)  {
		HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
		LOS_Msleep(2);
//		printf("Task 2 is working....\n");
	}
}

//task3_openmv
extern int distance, x_coordinate, y_coordinate;
void sendToESP8266_string(uint8_t *data, uint16_t len);
uint32_t last_publish = 0;
static void TaskEntry3(void) {
     while (1)
    {
//			  uint32_t now = HAL_GetTick();
//			   char openmvJson1[64];
//			   char openmvJson2[64];
//			   char openmvJson3[64];
////			   char test[64];
//      if (now - last_publish >= 5000) 
//			 {
//				 printf("[OpenMV] Distance: %d mm, X: %d, Y: %d\r\n", distance, x_coordinate, y_coordinate);
//         snprintf(openmvJson1, sizeof(openmvJson1),
//					 "{\'04\':%d}",
//                 distance);
//				 snprintf(openmvJson2, sizeof(openmvJson2),
//					 "{\'02\':%d}",
//                 x_coordinate);
//				 	snprintf(openmvJson3, sizeof(openmvJson3),
//					 "{\'03\':%d}",
//                 y_coordinate);
////				 snprintf(test, sizeof(test), "Hello from OpenHarmony! Tick: %lu", now); 
//				    // 打印即将发送的数据
//            printf("[DEBUG] Ready to publish: %s\n", openmvJson1);
//				    printf("[DEBUG] Ready to publish: %s\n", openmvJson2);
//            // 发布 MQTT 数据
//            if (MQTT_Publish("stm32/openmv1", openmvJson1, 0, false)) {
//                printf("[MQTT] Published OpenMV Data: %s\n", openmvJson1);
//            } else {
//                printf("[MQTT] Failed to publish OpenMV Data\n");
//            }
//						
//            if (MQTT_Publish("stm32/openmv2", openmvJson2, 0, false)) {
//                printf("[MQTT] Published OpenMV Data: %s\n", openmvJson2);
//            } else {
//                printf("[MQTT] Failed to publish OpenMV Data\n");
//            }
//						 if (MQTT_Publish("stm32/openmv3", openmvJson3, 0, false)) {
//                printf("[MQTT] Published OpenMV Data: %s\n", openmvJson3);
//            } else {
//                printf("[MQTT] Failed to publish OpenMV Data\n");
//            }
//        last_publish = now;
//       }
//        LOS_Msleep(1000);  // 小睡一会，避免空跑 CPU
    }
}

//task4: MQTT处理任务
static void TaskEntry4(void) {
    printf("[MQTT] Task4 started\n");

    // 初始化 ESP01s 模块
    if (!ESP01S_Init()) {
        printf("[MQTT] ESP01S init failed!\n");
        while (1) LOS_Msleep(100
					);
    }
    printf("[MQTT] ESP01S init OK\n");

    // 订阅主题
    if (!MQTT_Subscribe("stm32/clear", 0)) {
        printf("[MQTT] Subscription failed!\n");
    } else {
        printf("[MQTT] Subscribed to stm32/clear\n");
    }

    // 发布循环
//    uint32_t last_publish = 0;
    while (1) {
//        MQTT_ProcessReceivedData();

//        uint32_t now = HAL_GetTick();
//        if (now - last_publish >= 5000) {
//            char msg[64];
//            snprintf(msg, sizeof(msg), "Hello from OpenHarmony! Tick: %lu", now);
//            MQTT_Publish("stm32/data", msg, 0, false);
//            printf("[MQTT] Published: %s\n", msg);
//            last_publish = now;
        }      

//        LOS_Msleep(200); // 稍作延时
    }

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

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
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
	printf("SYS Init Ok\n");
  LOS_KernelInit();	
	
	OLED_Init();                           //OLED��ʼ
	OLED_Clear();                         //����
	OLED_ShowString(0,0,"INIT OK",16, 0);    //������ʾ8X16�ַ���
	LOS_Msleep(100);
	OLED_Clear(); 
	HAL_UART_Receive_IT(&huart2, &c, 1);
	
//  //start task 1
	unsigned int ret;
	unsigned int taskID1;
	TSK_INIT_PARAM_S task1 = { 0 };
	task1.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskEntry1;
	task1.uwStackSize  = 0x1000;
	task1.pcName       = "TaskEntry1";
	task1.usTaskPrio   = 6;
	ret = LOS_TaskCreate(&taskID1, &task1);
	if (ret != LOS_OK) {
	  printf("Task1 init Fail!\n");
	  while(1) {
		}
	}
  //start task 2
	unsigned int taskID2;
	TSK_INIT_PARAM_S task2 = { 0 };
	task2.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskEntry2;
	task2.uwStackSize  = 0x1000;
	task2.pcName       = "TaskEntry2";
	task2.usTaskPrio   = 5;
	ret = LOS_TaskCreate(&taskID2, &task2);
	if (ret != LOS_OK) {
	  printf("Task2 init Fail!\n");
	  while(1) {
		}
	}
	
	//start task 3
	unsigned int taskID3;
  TSK_INIT_PARAM_S task3 = { 0 };
  task3.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskEntry3;
  task3.uwStackSize  = 0x1000;
  task3.pcName       = "TaskEntry3";
  task3.usTaskPrio   = 7;
  ret = LOS_TaskCreate(&taskID3, &task3);
  if (ret != LOS_OK) {
    printf("Task3 init Fail!\n");
    while(1);
}
	
  //start task 4
  unsigned int taskID4;
  TSK_INIT_PARAM_S task4 = { 0 };
  task4.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskEntry4;
  task4.uwStackSize  = 0x1000;
  task4.pcName       = "TaskEntry4_MQTT";
  task4.usTaskPrio   = 6;
  ret = LOS_TaskCreate(&taskID4, &task4);
  if (ret != LOS_OK) {
    printf("Task4 MQTT init Fail!\n");
    while(1);
  }
	
	//start LOS
	LOS_Start();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
//void sendToESP8266_string(uint8_t *data, uint16_t len)
//{
//    HAL_UART_Transmit(&huart3, data, len, HAL_MAX_DELAY);

//    uint8_t newline = '\n';
//    HAL_UART_Transmit(&huart3, &newline, 1, HAL_MAX_DELAY);
//}
/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM7 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM7)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
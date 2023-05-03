/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint8_t Rxbuf[500];
uint8_t msg[100];
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
uint8_t Data_Rx = 0;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */
void debug_printf(char * fmt, ...);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{

	if (huart->Instance == USART3)
	{
		Data_Rx = 1;
		HAL_UARTEx_ReceiveToIdle_IT(&huart3,Rxbuf,sizeof(Rxbuf));
		
		//HAL_UART_Receive_IT(&huart3,Rxbuf,sizeof(Rxbuf));
	}
}

void gsm_on()
{
	HAL_GPIO_WritePin(GSM_RESET_GPIO_Port, GSM_RESET_Pin, GPIO_PIN_SET);
	HAL_Delay(200);
	HAL_GPIO_WritePin(GSM_RESET_GPIO_Port, GSM_RESET_Pin, GPIO_PIN_RESET);
	HAL_Delay(200);
	
}

uint8_t Send_AT_cmd(char *str123,char *Matched_str)
{
	sprintf((char*)msg,"%s",str123);
	debug_printf("TX : %s",msg);
	
	Data_Rx = 0;
	HAL_UART_Transmit(&huart3,(uint8_t *)msg,strlen((char*)msg),1000);
	
	HAL_Delay(50);
	
	while(Data_Rx == 0)
	{
		HAL_Delay(1);
	}
	
	if(strstr((char *)Rxbuf,Matched_str) && Data_Rx == 1)
	{
		debug_printf("RX1 : %s",Rxbuf);
		memset(Rxbuf,0,sizeof(Rxbuf));
		Data_Rx = 0;
		return 0;
	}
	else if(Data_Rx == 1)
	{
		debug_printf("RX2 : %s",Rxbuf);
		return 1;
	}
			
}

void debug_printf(char * fmt, ...)
{
	char debug_str[500];
	memset(debug_str,NULL,sizeof(debug_str));
	
	va_list args;
	va_start(args,fmt);
	vsprintf(debug_str,fmt,args);
	va_end(args);
	
	HAL_UART_Transmit(&huart2,(uint8_t *)debug_str,strlen(debug_str),1000);
	//HAL_UART_Transmit(&huart2,(uint8_t *)"\r\n",2,10);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

	uint8_t call_cnt = 0;
	uint8_t CNTZ = 0x1a;
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
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
	
	
	debug_printf("GSM Testing Demo...\r\n");
	gsm_on();
	debug_printf("GSM Init...\r\n");
	
	memset(Rxbuf,NULL,sizeof(Rxbuf));
	HAL_UARTEx_ReceiveToIdle_IT(&huart3,Rxbuf,sizeof(Rxbuf));
	//HAL_UART_Receive_IT(&huart3,Rxbuf,sizeof(Rxbuf));
	
	Send_AT_cmd("AT\r\n","OK");  // This command is used to check communication between the module and the MCU.
	HAL_Delay(500);
	
	//ATI
	Send_AT_cmd("ATI\r\n","OK");  // Command To Get Product Information of GSM / GPRS Modem
	HAL_Delay(500);

	Send_AT_cmd("AT+CPIN?\r\n","OK"); // AT commands for SIM presence and status
	HAL_Delay(2000);
	
	Send_AT_cmd("AT+CREG?\r\n","+CREG:"); // Network registration
	HAL_Delay(2000);
	
	Send_AT_cmd("AT+CREG?\r\n","+CREG:"); // Network registration
	HAL_Delay(2000);
	
	Send_AT_cmd("AT+CREG?\r\n","+CREG:");  // Network registration
	HAL_Delay(2000);
	
	Send_AT_cmd("AT+CREG?\r\n","+CREG:");  // Network registration
	HAL_Delay(2000);
	
	Send_AT_cmd("AT+CSQ\r\n","+CSQ:");  //Signal quality
	HAL_Delay(2000);
	
	Send_AT_cmd("AT+CBC\r\n","+CBC:"); //Battery charge level
	HAL_Delay(2000);
	
	Send_AT_cmd("AT+COPS?\r\n","+COPS:");  //sets the modem to choose a network operator automatically
	HAL_Delay(2000);
	
//	Send_AT_cmd("AT+QICSGP=1,\"internet\"\r\n","OK");  // Select CSD or GPRS as the Bearer
//	HAL_Delay(2000);
	
	Send_AT_cmd("AT+CGDCONT=1,\"IP\",\"internet\"\r\n","OK");  //AT+CGDCONT=1,"IP","gprs.oi.com.br"
	HAL_Delay(2000);

	//AT+CGACT=1,1
	Send_AT_cmd("AT+CGACT=1,1\r\n","OK");   //AT+CGDCONT=1,"IP","gprs.oi.com.br"
	HAL_Delay(2000);
	
//	Send_AT_cmd("AT+CGATT=?\r\n","+CGATT:");   //AT+CGDCONT=1,"IP","gprs.oi.com.br"
//	HAL_Delay(2000);

	//AT+CGATT=1
	Send_AT_cmd("AT+CGATT=1\r\n","OK");   //AT+CGDCONT=1,"IP","gprs.oi.com.br"
	HAL_Delay(2000);
	
	Send_AT_cmd("AT+CGPADDR=1\r\n","+CGPADDR:");   
	HAL_Delay(2000);
	
//	Send_AT_cmd("AT+QIACT\r\n","OK");   
//	HAL_Delay(5000);
	
//	//AT+QISTAT=?	
//	Send_AT_cmd("AT+QISTAT=?\r\n","OK");   
//	HAL_Delay(5000);
//	
//	
//	Send_AT_cmd("AT+QISTAT\r\n","+QISTAT:");   
//	HAL_Delay(5000);
	
	memset(msg,0,sizeof(msg));
	sprintf(msg,"AT+QILOCIP\r\n");
	HAL_UART_Transmit(&huart3,(uint8_t *)msg,strlen((char*)msg),1000); 
	HAL_Delay(5000);
	
	//MQTT Commmand
		
  // AT+QMTCFG="recv/mode",0,0,1
	
//	Send_AT_cmd("AT+QMTCFG=\"recv/mode\",0,0,1\r\n","OK");   
//	HAL_Delay(2000);

//	Send_AT_cmd("AT+QMTOPEN=?\r\n","+QMTOPEN:");  
//	HAL_Delay(5000);

	Send_AT_cmd("AT+QMTDISC=0\r\n","OK");  //AT+QMTDISC=0  //Disconnect a client from MQTT server
	HAL_Delay(200);
	
	Send_AT_cmd("AT+QMTOPEN?\r\n","+QMTOPEN:");  
	HAL_Delay(2000);
	
	Send_AT_cmd("AT+QMTOPEN=0,\"65.2.107.224\",8051\r\n","+QMTOPEN:");  
	HAL_Delay(2000);
	
	Send_AT_cmd("AT+QMTOPEN?\r\n","+QMTOPEN:");  
	HAL_Delay(2000);
	
	Send_AT_cmd("AT+QMTCONN=0,\"MqqtClient\",\"ResonentLoomsMonitor\",\"uHjhUqN5zbO5VKc\"\r\n","+QMTCONN:");  
	HAL_Delay(2000);
	
	Send_AT_cmd("AT+QMTCONN?\r\n","+QMTCONN:");  
	HAL_Delay(5000);
	
	//AT+QMTSUB=0,0,"topic/example
	
	//AT+QMTSUB=?
//	Send_AT_cmd("AT+QMTSUB=?\r\n","+QMTSUB:");  
//	HAL_Delay(2000);

	Send_AT_cmd("AT+QMTSUB=0,1,\"TestSub\",0\r\n","+QMTSUB:");  
	HAL_Delay(500);
	
	Send_AT_cmd("AT+QMTPUB=0,0,0,0,\"TestPub\"\r\n",">");  
	HAL_Delay(500);
	
	memset(msg,0,sizeof(msg));
	sprintf(msg,"Hello from STM32_GSM");
	HAL_UART_Transmit(&huart3,(uint8_t *)msg,strlen((char*)msg),1000);
	HAL_UART_Transmit(&huart3,&CNTZ,1,1000);
	
//AT+QMTCLOSE
//	Send_AT_cmd("AT+QMTCLOSE=0\r\n","+QMTCLOSE:");  
//	HAL_Delay(500);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
		if(strstr((char *)Rxbuf,"RING") && Data_Rx == 1)
	  {
			debug_printf("RX : %s",Rxbuf);
		  call_cnt++;
		  if(call_cnt == 3)
		  {
			  Send_AT_cmd("ATA\r\n","OK");
			  
			  HAL_Delay(4000);
			  
			  Send_AT_cmd("ATH\r\n","OK");
			  call_cnt = 0;
		  }
		  Data_Rx = 0;
	  }
		if(strstr((char *)Rxbuf,"SEND OK") && Data_Rx == 1)
	  {
			debug_printf("RX : %s",Rxbuf);
		  Data_Rx = 0;
	  }
		else
		{
			if(Data_Rx == 1)
			{
				debug_printf("RX : %s",Rxbuf);
				memset(Rxbuf,NULL,sizeof(Rxbuf));
				Data_Rx = 0;
			}
		}
	  HAL_Delay(10);
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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

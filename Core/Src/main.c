/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */


#include <stdio.h>
#include <string.h>

#include "car.h"
#include "direction.h"
#include "speed.h"
#include "delay.h"
#include "ledbar.h"
#include "temp.h"

#include "car2.h"
#include "ultrasonic.h"
#include "autodrive.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

#ifdef __GNUC__
/* With GCC small printf (option LD Linker->Libraries->Small printf
 * set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int  __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int  fputc(int ch, FILE *f)
#endif /* __GNUC__*/

/** @brief Retargets the C library printf function to the USART.
 *  @param None
 *  @retval None
 */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART2 and Loop
     until the end of transmission */
  if(ch == '\n')
  {
	  HAL_UART_Transmit(&huart2, (uint8_t*) "\r", 1, 0xFFFF);
  }
   HAL_UART_Transmit(&huart2, (uint8_t*) &ch, 1, 0xFFFF);
   return ch;
}

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

// uart variable
uint8_t rx1_data;
//uint8_t rx2_data;

// temp adc variable
uint32_t adc_value;
//char msg[50];
uint32_t voltage_mV;

uint8_t current_speed = 50;   // 현재 적용 속도
uint8_t base_speed;

volatile uint8_t uart1_flag = 0;
volatile uint8_t uart1_cmd;

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        uart1_cmd = rx1_data;
        uart1_flag = 1;

        HAL_UART_Receive_IT(&huart1, &rx1_data, 1);
    }
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	Ultrasonic_IC_Callback(htim);
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
  MX_TIM2_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM11_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */

//  // 1) speed 모듈에 TIM2 채널 연결
  Speed_Init(&htim2, TIM_CHANNEL_1, &htim2, TIM_CHANNEL_2);
//
//  // 2) PWM Start + 초기 STOP
  Car_Init();

  // UART
  HAL_UART_Receive_IT(&huart1, &rx1_data, 1);

  // temp
  base_speed = current_speed;

  // ultrasonic
  HAL_TIM_Base_Start(&htim11);
  Ultrasonic_Init();
  AutoDrive_Init();

  uint8_t auto_mode = 0;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */



  while (1)
  {

/// ======= ultrasonic ========
//  	Ultrasonic_Task();   // 논블로킹 초음파
//  	AutoDrive_Run();
  	if(auto_mode)
  	    {
  	        Ultrasonic_Task();
  	        AutoDrive_Run();
  	    }

//		 디버그 출력 (필요하면 주기 줄이기)
		static uint32_t prev_print = 0;
		if(HAL_GetTick() - prev_print > 200)
		{
				prev_print = HAL_GetTick();
				printf("L:%d C:%d R:%d | %s\r\n",
								dist_L, dist_C, dist_R, AutoDrive_GetActionString());
		}

/// ======= car test ========
//  	Car_test();

/// ======= temp sensor test =======
  	/// ADC, voltage debugging cord using uart2 ///
//  	read_adc();
//  	adc_value = read_adc();
//
//  	printf("ADC: %lu\r\n", adc_value);
//  	voltage_mV = (adc_value * 3300) / 4095;
//  	printf("Voltage: %lu.%03lu V\r\n",
//  	       voltage_mV / 1000,
//  	       voltage_mV % 1000);
//
//  	int16_t temp = get_temperature();
//  	/// /////////////////////////////////////// ///
//
//  	printf("Temp: %d C\r\n", temp);
//
//  	current_speed = update_speed_by_temp(temp, current_speed, base_speed);
//  	Car_MovePercent(CAR_FRONT, current_speed);
//
//  	printf("Speed: %d %%\r\n", current_speed);
//  	printf("===========\r\n");
//  	HAL_Delay(1000);


  	/// ======= uart1 =======
  	if (uart1_flag)
		{
				uart1_flag = 0;

				switch (uart1_cmd)
				{
						case 'w': auto_mode = 0; Car_Move(CAR_FRONT, SPD_50); break;
						case 's': auto_mode = 0; Car_Move(CAR_BACK, SPD_50); break;
						case 'a': auto_mode = 0; Car_Move(CAR_LEFT, SPD_50); break;
						case 'd': auto_mode = 0; Car_Move(CAR_RIGHT, SPD_50); break;
						case 'q': auto_mode = 1; break;   // 자율 ON

						default: auto_mode = 0; Car_Stop(); break;
				}

				// 디버그 출력은 여기서
				HAL_UART_Transmit(&huart2, (uint8_t*)&uart1_cmd, 1, 10);
		}


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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
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

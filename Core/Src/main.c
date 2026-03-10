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
#include "i2c.h"
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
#include "ultrasonic.h"
#include "autodrive.h"
#include "gas.h"
#include "ina219.h"

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

uint16_t adc_buf[2];   // [0]=Temp(PA6), [1]=Gas(PC2)

// uart
uint8_t rx1_data;

volatile uint8_t uart1_flag = 0;
volatile uint8_t uart1_cmd;

extern I2C_HandleTypeDef hi2c3; // MX에서 설정한 I2C3 핸들
INA219_t battery_monitor;
int32_t v_mv, i_ma;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

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
  MX_I2C3_Init();
  /* USER CODE BEGIN 2 */

  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buf, 2);

//  1) speed 모듈에 TIM2 채널 연결
  Speed_Init(&htim2, TIM_CHANNEL_1, &htim2, TIM_CHANNEL_2);
//
//  2) PWM Start + 초기 STOP
  Car_Init();

  // UART1
  HAL_UART_Receive_IT(&huart1, &rx1_data, 1);

  // ultrasonic
  HAL_TIM_Base_Start(&htim11);
  Ultrasonic_Init();

  AutoDrive_Init();

  if (INA219_Init(&battery_monitor, &hi2c3) != HAL_OK) {
      printf("INA219 Init Failed!\r\n");
  }


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {

  	// 온도, 가스 상태 확인 (반환값 받기)
  	TempLevel_t temp_status = Temp_TaskLevel();
  	GasLevel_t gas_status = Gas_TaskPPM();

  	// 온도, 가스에 따른 감
  	// 온도, 가스에 따른 감
  	if (gas_status != GAS_SAFE || temp_status != TEMP_SAFE) {
  		uint8_t reduction_step = 0;

  		// 1단계 : 둘 다 위
				if (gas_status != GAS_SAFE && temp_status != TEMP_SAFE) {
	        reduction_step = 5;
				}
				// 2단계 둘 중 하나만위
				else                  reduction_step = 1;

				// reduction
				if (current_speed > 20)
				{
					if (current_speed >= 20 + reduction_step)
					{
						current_speed -= reduction_step;
					}
					else
					{
						current_speed = 20;
					}
				}
				else {
					current_speed = 20;
				}

				// 속도가 변할 때마다 즉시 적용
				Drive_Control(uart1_cmd, current_speed);
		}

  	// uart1으로 통
		if (uart1_flag) {
				uart1_flag = 0;
				Drive_Control(uart1_cmd, current_speed);
				HAL_UART_Transmit(&huart2, (uint8_t*)&uart1_cmd, 1, 10);
		}

  	ultrasonic_uart2_debugmsg();

  	// 자율주행모드
  	if(auto_mode)
		{
				Ultrasonic_Task();
				AutoDrive_Run();
		}

  	if (stop_flag)
  		printf("0 \r\n");
  	else
  		printf("%d \r\n", current_speed);


  	v_mv = INA219_ReadBusVoltage_mV(&battery_monitor);
		i_ma = INA219_ReadCurrent_mA(&battery_monitor);

		// 12.5V 150mA인 경우: "Bat: 12.50 V, Cur: 150 mA"
		printf("Bat: %ld.%02ld V, Cur: %ld mA\r\n", v_mv / 1000, (v_mv % 1000) / 10, i_ma);

  	printf("=====================\r\n");

  	HAL_Delay(500);



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

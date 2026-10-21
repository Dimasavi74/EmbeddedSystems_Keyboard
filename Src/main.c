/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "kb.h"
#include "sdk_uart.h"
#include "pca9538.h"
#include "oled.h"
#include "fonts.h"
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
uint32_t num1 = 0;
uint32_t num2 = 0;
int64_t result = 0;
uint8_t operation = 0;
uint8_t step = 0;

char num1_buf[11];  // 10 цифр максимум + '\0'
char num2_buf[11];
char result_buf[21];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void KB_Test( void );
void OLED_KB( uint8_t OLED_Keys[]);
void oled_Reset( void );
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_I2C1_Init();
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */
  oled_Init();

  /* USER CODE END 2 */
 
 

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  KB_Test();
//	  HAL_Delay(500);

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
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
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

static void u32_to_str(uint32_t v, char *buf) {
    char tmp[10];
    int i = 0;
    if (v == 0) { buf[0]='0'; buf[1]='\0'; return; }
    while (v) { tmp[i++] = '0' + (v % 10); v /= 10; }
    int j = 0;
    while (i) buf[j++] = tmp[--i];
    buf[j] = '\0';
}

static void i64_to_str(int64_t v, char *buf) {
    char tmp[20];
    int i = 0;
    int neg = (v < 0);
    uint64_t u = neg ? -(uint64_t)v : (uint64_t)v;
    if (u == 0) { buf[0]='0'; buf[1]='\0'; return; }
    while (u) { tmp[i++] = '0' + (u % 10); u /= 10; }
    int j = 0;
    if (neg) buf[j++] = '-';
    while (i) buf[j++] = tmp[--i];
    buf[j] = '\0';
}

void KB_Test( void ) {
//	UART_Transmit( (uint8_t*)"KB test start\n" );
	uint8_t flag = 0, Row[4] = {ROW4, ROW3, ROW2, ROW1}, Key, OldKey, OLED_Keys[12] = {0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30};
	oled_Reset();
	oled_SetCursor(0, 0);

	u32_to_str(num1, num1_buf);
	u32_to_str(num2, num2_buf);
	i64_to_str(result, result_buf);

	oled_WriteString(num1_buf, Font_7x10, White);
	oled_SetCursor(0, 10);
	switch (operation) {
	    case 0: oled_WriteString("+", Font_7x10, White); break;
	    case 1: oled_WriteString("-", Font_7x10, White); break;
	    case 2: oled_WriteString("*", Font_7x10, White); break;
	}
	oled_SetCursor(0, 20);
	oled_WriteString(num2_buf, Font_7x10, White);
	oled_SetCursor(0, 30);
	oled_WriteString(result_buf, Font_7x10, White);
	oled_UpdateScreen();
	while (flag == 0) {
	for ( int i = 0; i < 4; i++ ) {
//		while( !( R && C && L ) ) {
//			OldKey = Key;
//			Key = Check_Row( Row[i] );
//			if ( Key == 0x01 && Key != OldKey) {
//				UART_Transmit( (uint8_t*)"Right pressed\n" );
//				R = 1;
//				OLED_Keys[2+3*i] = 0x31;
//				OLED_KB(OLED_Keys);
//			} else if ( Key == 0x02 && Key != OldKey) {
//				UART_Transmit( (uint8_t*)"Center pressed\n" );
//				C = 1;
//				OLED_Keys[1+3*i] = 0x31;
//				OLED_KB(OLED_Keys);
//			} else if ( Key == 0x04 && Key != OldKey) {
//				UART_Transmit( (uint8_t*)"Left pressed\n" );
//				L = 1;
//				OLED_Keys[3*i] = 0x31;
//				OLED_KB(OLED_Keys);
//			}
//		}

		OldKey = Key;
		Key = Check_Row( Row[i] );
		if (Key != 0x00 && flag == 0) {
			flag = 1;
		}
		if ( Key == 0x01 && i == 1) {
			UART_Transmit( (uint8_t*)"9 pressed\n" );
			if (step == 0) {
				num1 = (num1 * 10 + 9) % 100000000;
			}
			else if (step == 2) {
				num2 = (num2 * 10 + 9) % 100000000;
			}

		}
		else if ( Key == 0x02 && i == 1) {
			UART_Transmit( (uint8_t*)"8 pressed\n" );
			if (step == 0) {
				num1 = (num1 * 10 + 8) % 100000000;
			}
			else if (step == 2) {
				num2 = (num2 * 10 + 8) % 100000000;
			}
		}
		else if ( Key == 0x04 && i == 1) {
			UART_Transmit( (uint8_t*)"7 pressed\n" );
			if (step == 0) {
				num1 = (num1 * 10 + 7) % 100000000;
			}
			else if (step == 2) {
				num2 = (num2 * 10 + 7) % 100000000;
			}
		}
		else if ( Key == 0x01 && i == 2) {
			UART_Transmit( (uint8_t*)"6 pressed\n" );
			if (step == 0) {
				num1 = (num1 * 10 + 6) % 100000000;
			}
			else if (step == 2) {
				num2 = (num2 * 10 + 6) % 100000000;
			}
		}
		else if ( Key == 0x02 && i == 2) {
			UART_Transmit( (uint8_t*)"5 pressed\n" );
			if (step == 0) {
				num1 = (num1 * 10 + 5) % 100000000;
			}
			else if (step == 2) {
				num2 = (num2 * 10 + 5) % 100000000;
			}
		}
		else if ( Key == 0x04 && i == 2) {
			UART_Transmit( (uint8_t*)"4 pressed\n" );
			if (step == 0) {
				num1 = (num1 * 10 + 4) % 100000000;
			}
			else if (step == 2) {
				num2 = (num2 * 10 + 4) % 100000000;
			}
		}
		else if ( Key == 0x01 && i == 3) {
			UART_Transmit( (uint8_t*)"3 pressed\n" );
			if (step == 0) {
				num1 = (num1 * 10 + 3) % 100000000;
			}
			else if (step == 2) {
				num2 = (num2 * 10 + 3) % 100000000;
			}
		}
		else if ( Key == 0x02 && i == 3) {
			UART_Transmit( (uint8_t*)"2 pressed\n" );
			if (step == 0) {
				num1 = (num1 * 10 + 2) % 100000000;
			}
			else if (step == 2) {
				num2 = (num2 * 10 + 2) % 100000000;
			}
		}
		else if ( Key == 0x04 && i == 3) {
			UART_Transmit( (uint8_t*)"1 pressed\n" );
			if (step == 0) {
				num1 = (num1 * 10 + 1) % 100000000;
			}
			else if (step == 2) {
				num2 = (num2 * 10 + 1) % 100000000;
			}
		}
		else if ( Key == 0x02 && i == 0) {
			UART_Transmit( (uint8_t*)"0 pressed\n" );
			if (step == 0) {
				num1 = (num1 * 10 + 0) % 100000000;;
			}
			else if (step == 2) {
				num2 = (num2 * 10 + 0) % 100000000;
			}
		}
		else if ( Key == 0x01 && i == 0) {
			UART_Transmit( (uint8_t*)"Enter pressed\n" );
			if (step == 0) {
				step = 1;
			}
			else if (step == 1) {
				step = 2;
			}
			else if (step == 2) {
				step = 3;
				if (operation == 0) {
					result = (int64_t)num1 + (int64_t)num2;
					UART_Transmit( (uint8_t*)"Result + passed\n" );
				}
				else if (operation == 1) {
					result = (int64_t)num1 - (int64_t)num2;
					UART_Transmit( (uint8_t*)"Result - passed\n" );
				}
				else if (operation == 2) {
					result = (int64_t)num1 * (int64_t)num2;
					UART_Transmit( (uint8_t*)"Result * passed\n" );
				}
			}
			else if (step == 3) {
				num1 = 0;
				num2 = 0;
				result = 0;
				step = 0;
			}
		}
		else if ( Key == 0x04 && i == 0) {
			UART_Transmit( (uint8_t*)"Operation pressed\n" );
			if (step == 0) {
				num1 = num1 / 10;
			}
			else if (step == 1) {
				operation += 1;
				if (operation == 3) {
					operation = 0;
				}
			}
			else if (step == 2) {
				num2 = num2 / 10;
			}
		}
//		UART_Transmit( (uint8_t*)"Row complete\n" );
//		R = C = L = 0;
//		HAL_Delay(25);
	}
	}
	flag = 0;
	Key = 0x00;
	HAL_Delay(150);
//	UART_Transmit( (uint8_t*)"KB test complete\n");
}
void OLED_KB( uint8_t OLED_Keys[12]) {
	for (int i = 3; i >= 0; i--) {
		oled_SetCursor(56, 5+(4-i)*10);
		for (int j = 0; j < 3; j++) {
			oled_WriteChar(OLED_Keys[j+3*i], Font_7x10, White);
		}
	}
	oled_UpdateScreen();
}
void oled_Reset( void ) {
	oled_Fill(Black);
	oled_SetCursor(0, 0);
	oled_UpdateScreen();
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

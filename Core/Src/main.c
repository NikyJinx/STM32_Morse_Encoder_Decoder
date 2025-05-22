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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>  // Per strcmp
//#include "morse_io_utils.h"
//#include "morse_utils.h"
#include "lcd_i2c.h"
#include "Morse.h"

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
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
/*DEFINIZIONE CASI SWITCH INIZIALE*/
typedef enum {
	STATO_DURATION = 0,
    STATO_SETTINGS,
    STATO_LETTERS_TO_MORSE,
    STATO_MORSE_TO_LETTERS
} StatoPrincipale;

/*DEFINIZIONE CASI SWITCH LETTER2MORSE*/
typedef enum {
    STATO_MENU = 0,
    STATO_MORSE_LED,
    STATO_MORSE_BUZZER,
    STATO_MORSE_LED_BUZZER,
    STATO_MORSE_LED_UART
} StatoMorse;

/*DEFINIZIONE CASI SWITCH MORSE2LETTER*/
typedef enum {
	STATO_SETTING_M2L = 0,
	STATO_NORMAL,
	STATO_DEBUG
}StatoM2L;


uint32_t now = 0;
/* *
 * QUI DEVO DARE LA POSSIBILITà DI MODIFICARE LA VELOCITA' DEL DOT
 * QUINDI METTERE UN PRIMO CASO DOVE IMPOSTO LA VELOCITA CONSIGLIATA TRA 100 E 200
 * POI PASSO ALLA TRASMISSIONE E TRADUZIONE
 * PREMENDO ESC DALLA UART TORNO PER LA SELEZIONE DELLA VELOCITA'
 * */


char comando[32];
uint8_t rx_byte;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
void stato_morse_to_letter();
void stato_morse_to_letter_debug();
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
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  StatoPrincipale stato_principale = STATO_DURATION;
  StatoMorse stato_corrente = STATO_MENU;
  StatoM2L stato_M2L = STATO_SETTING_M2L;

  lcd_init(&hi2c1);
  morse_init(&morse_ctx);
  lcd_clear();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	switch (stato_principale) {
	case STATO_DURATION:
		const char* welcome_msg =
		    "\r\n"
		    "=========================================\r\n"
		    "        MORSE CODE CONVERTER - STM32      \r\n"
		    "=========================================\r\n"
		    "This program allows you to:\r\n"
		    "- Convert letters to Morse code\r\n"
		    "- Decode Morse code into letters\r\n"
		    "- Display output via UART, LED, buzzer,\r\n"
		    "  and I2C 1602 LCD display\r\n"
		    "\r\n"
		    "You can interact via the UART terminal to:\r\n"
		    "- Choose the operating mode (input/output)\r\n"
		    "- Configure Morse signal speed (dot duration)\r\n"
		    "- Send letters or enter Morse using the button\r\n"
		    "\r\n";

		HAL_UART_Transmit(&huart2, (uint8_t*)welcome_msg, strlen(welcome_msg), HAL_MAX_DELAY);

		configura_dot_duration();

		stato_principale=STATO_SETTINGS;

	case STATO_SETTINGS:
			const char *prompt =
				"Select operation mode:\n\r"
					"1 = Letters -> Morse\n\r"
					"2 = Morse -> Letters\n\r";


			HAL_UART_Transmit(&huart2, (uint8_t*)prompt, strlen(prompt), HAL_MAX_DELAY);


			if (ricevi_comando_uart(comando)) {
				stato_principale = STATO_SETTINGS;
				break;
			}

			if (strcmp(comando, "1") == 0) {
				stato_principale = STATO_LETTERS_TO_MORSE;
				stato_corrente = STATO_MENU;  // enter in section LED/BUZZER
			} else if (strcmp(comando, "2") == 0) {
				stato_principale = STATO_MORSE_TO_LETTERS;
			} else {
				const char* msg = "Invalid Command\r\n";
				HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
			}
			break;//end case STATO_SETTINGS

/*************************OPERATION MODE LETTER 2 MORSE*****************************/
	case STATO_LETTERS_TO_MORSE:
		switch (stato_corrente) {
			case STATO_MENU:
				const char *menu =
					"Choose mode:\n\r"
					"1 = Only LED\n\r"
					"2 = Only Buzzer\n\r"
					"3 = LED + Buzzer\n\r"
					"4 = LED + UART\n\r";

				HAL_UART_Transmit(&huart2, (uint8_t*)menu, strlen(menu), HAL_MAX_DELAY);


				if (ricevi_comando_uart(comando)) {
					stato_principale = STATO_DURATION;
					break;
				}

				if (strcmp(comando, "1") == 0) 			stato_corrente = STATO_MORSE_LED;
				else if (strcmp(comando, "2") == 0)     stato_corrente = STATO_MORSE_BUZZER;
				else if (strcmp(comando, "3") == 0)     stato_corrente = STATO_MORSE_LED_BUZZER;
				else if (strcmp(comando, "4") == 0)     stato_corrente = STATO_MORSE_LED_UART;
				else					                stato_corrente = STATO_MENU;  // torna al menu stato 0 di suqesto switch
				break;//STATO_MENU

			case STATO_MORSE_LED:
				esegui_traduzione_morse(1, 0, 0);
				if (ricevi_comando_uart(comando)) {
					stato_principale = STATO_DURATION;
					break;
				}
				break;//STATO_MORSE_LED

			case STATO_MORSE_BUZZER:
				esegui_traduzione_morse(0, 1, 0);
				if (ricevi_comando_uart(comando)) {
					stato_principale = STATO_DURATION;
					break;
				}
				break;//STATO_MORSE_BUZZER:

			case STATO_MORSE_LED_BUZZER:
				esegui_traduzione_morse(1, 1, 0);
				if (ricevi_comando_uart(comando)) {
					stato_principale = STATO_DURATION;
					break;
				}
				break;//STATO_MORSE_LED_BUZZER:

			case STATO_MORSE_LED_UART:
				esegui_traduzione_morse(1, 0, 1);
				if (ricevi_comando_uart(comando)) {
					stato_principale = STATO_DURATION;
					break;
				}
				break;//STATO_MORSE_LED_UART
		}//switch (stato_corrente)
		break;//STATO_LETTERS_TO_MORSE

/***************************************************************************************************/
/*************************END OPERATION MODE LETTER 2 MORSE*****************************/
/***************************************************************************************************/

/******************************OPERATION MODE MORSE 2 LETTER******************************/
//	case STATO_MORSE_TO_LETTERS:
//		stato_morse_to_letter();
//		break;//STATO_MORSE_TO_LETTER

	case STATO_MORSE_TO_LETTERS:
		switch (stato_M2L){

			case STATO_SETTING_M2L:
				char menu[512];
				sprintf(menu,
				    "*********************************************\n\r"
				    "Info:\n\r"
				    "Press user_bottom to send a dot or a dash\n\r"
				    "Dot time < %lu ms\n\r"
				    "Dash and letter gap: %lu > time > %lu ms\n\r"
				    "Word end: %lu > time > %lu ms\n\r"
				    "End conversation wait for %lu ms\n\r"
				    "\n\r"
				    "Please now choose the operation mode: \n\r"
				    "Normal --> send only symbols\n\r"
				    "Debug  --> send symbols with their time\n\r"
				    "\n\r"
				    "Choose mode:\n\r"
				    "1 = Normal\n\r"
				    "2 = Debug\n\r",
				    dot_duration,
				    3 * dot_duration, 3 * dot_duration,
				    7 * dot_duration, 21 * dot_duration,
					21 * dot_duration
				);

				HAL_UART_Transmit(&huart2, (uint8_t*)menu, strlen(menu), HAL_MAX_DELAY);

				if (ricevi_comando_uart(comando)) {
					stato_principale = STATO_SETTINGS;
				break;
				}


				if (strcmp(comando, "1") == 0) 			stato_M2L = STATO_NORMAL;
				else if (strcmp(comando, "2") == 0)     stato_M2L = STATO_DEBUG;
				break;//STATO_SETTING_M2L

			case STATO_NORMAL:
				stato_morse_to_letter();
				break;//STATO_NORMAL

			case STATO_DEBUG:
				stato_morse_to_letter_debug();
//				stato_morse_to_letter();
			break;//STATO_NORMAL


			break;//STATO_MORSE_TO_LETTER
			}


	}//switch (stato_principale)

  }//END while (1)

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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5|GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA5 PA6 */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */


void stato_morse_to_letter(){
	  now = HAL_GetTick();
	  gestisci_inizio_pressione(&morse_ctx, now);
	  gestisci_fine_pressione(&morse_ctx, now);
	  gestisci_spaziatura(&morse_ctx, now);
	  invia_parola(&morse_ctx, now);

	  HAL_Delay(10);
}

void stato_morse_to_letter_debug(){
	  now = HAL_GetTick();
	  gestisci_inizio_pressione(&morse_ctx, now);
	  gestisci_fine_pressione_debug(&morse_ctx, now);
	  gestisci_spaziatura_debug(&morse_ctx, now);
	  invia_parola_debug(&morse_ctx, now);
	  HAL_Delay(10);
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

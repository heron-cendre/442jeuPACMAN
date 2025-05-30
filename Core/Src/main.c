/* DEBUT DU CODE UTILISATEUR EN-TETE */
/**
 ******************************************************************************
 * Fichier           : main.c
 * Description       : Corps principal du programme
 ******************************************************************************
 * Attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * Tous droits reserves.
 *
 * Ce logiciel est sous licence selon les termes du fichier LICENSE dans le dossier racine de ce composant logiciel.
 * Si aucun fichier LICENSE n'est fourni avec ce logiciel, il est fourni TEL QUEL.
 *
 ******************************************************************************
 */
/* FIN DU CODE UTILISATEUR EN-TETE */
/* Inclusions ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "adc.h"
#include "dac.h"
#include "dma2d.h"
#include "i2c.h"
#include "ltdc.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"
#include "stdio.h"
#include "maze.h"

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

// Dimensions de l'ecran LCD
#define LCD_LONGUEUR 480
#define LCD_LARGEUR 272

// Variables globales (definies dans maze.h)

uint32_t joystick_v = 0, joystick_h = 0;
PacMan pacman;  
GameState etat = STATE_MENU;

// Ajouter les variables globales de minuterie manquantes pour l'etat du jeu
uint32_t last_update_time = 0;
uint32_t ghost_update_time = 0;
uint32_t state_change_delay = 0;
uint32_t animation_time = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  Point d'entree de l'application
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
  MX_ADC3_Init();
  MX_DMA2D_Init();
  MX_FMC_Init();
  MX_I2C1_Init();
  MX_I2C3_Init();
  MX_LTDC_Init();
  MX_RTC_Init();
  MX_SPI2_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM5_Init();
  MX_TIM8_Init();
  MX_USART1_UART_Init();
  MX_USART6_UART_Init();
  MX_ADC1_Init();
  MX_DAC_Init();
  MX_UART7_Init();
  /* USER CODE BEGIN 2 */
  // Initialiser l'ecran LCD
  BSP_LCD_Init();
  BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);
  BSP_LCD_LayerDefaultInit(1, LCD_FB_START_ADDRESS + BSP_LCD_GetXSize()*BSP_LCD_GetYSize()*4);
  BSP_LCD_SelectLayer(0);
  BSP_LCD_DisplayOn();
  BSP_LCD_Clear(LCD_COLOR_BLACK);
  
  // Initialiser l'ecran tactile
  BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
  
  // Demarrer l'ADC pour le joystick et les potentiometres
  HAL_ADC_Start(&hadc1);
  HAL_ADC_Start(&hadc3);
  
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

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
  * @brief Configuration de l'horloge du systeme
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configurer la capacite de pilotage LSE */
  HAL_PWR_EnableBkUpAccess();

  /** Configurer la tension de sortie du regulateur principal interne */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initialiser les oscillateurs RCC selon les parametres specifies dans la structure RCC_OscInitTypeDef. */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 400;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activer le mode Over-Drive */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initialiser les horloges CPU, AHB et APB */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */


/* USER CODE END 4 */

/**
  * @brief  Callback de periode ecoulee en mode non bloquant
  * @note   Cette fonction est appelee lorsque l'interruption TIM6 a eu lieu, a l'interieur de HAL_TIM_IRQHandler(). Elle appelle directement HAL_IncTick() pour incrementer une variable globale "uwTick" utilisee comme base de temps de l'application.
  * @param  htim : handle TIM
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  Cette fonction est executee en cas d'erreur.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Debug Error_Handler */
	/* L'utilisateur peut ajouter sa propre implementation pour signaler l'etat de retour d'erreur HAL */
	__disable_irq();
	while (1)
	{
	}
  /* USER CODE END Debug Error_Handler */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Signale le nom du fichier source et le numero de ligne source ou l'erreur assert_param s'est produite.
  * @param  file: pointeur vers le nom du fichier source
  * @param  line: numero de ligne source de l'erreur assert_param
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
	/* L'utilisateur peut ajouter sa propre implementation pour signaler le nom du fichier et le numero de ligne,
     ex: printf("Mauvaise valeur de parametres : fichier %s a la ligne %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

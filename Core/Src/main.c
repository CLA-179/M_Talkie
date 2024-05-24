/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
#include "i2c.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Soft_i2c1.h"
#include "M_talk.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define ADD_W 0x90
#define ADD_R 0x91
#define SQ_TH 0x0100u
#define PTT 0x01
#define PWR_K 0x02
#define RX 0
#define TX 1

#define KEY_PTT HAL_GPIO_ReadPin(PTT_GPIO_Port, PTT_Pin)
#define KEY_PWR HAL_GPIO_ReadPin(PWR_I_GPIO_Port, PWR_I_Pin)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

uint32_t tx_freq[16];
uint32_t rx_freq[16];
uint8_t rt;
uint8_t tx_first;
uint8_t rx_cnt;
uint8_t pwr_open;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

uint8_t key_scan(void)
{
  // static uint8_t key_up = 1;  /* 按键按松�??????标志 */
  uint8_t keyval = 0;

  if ((KEY_PTT == 0)) /* 按键松开标志�??????1, 且有任意�??????个按键按下了 */
  {
    delay_ms(10); /* 去抖�?????? */
    // key_up = 0;

    if (KEY_PTT == 0)
      keyval |= PTT;
  }
  else if (KEY_PTT == 1) /* 没有任何按键按下, 标记按键松开 */
  {
    // key_up = 1;
  }

  if ((KEY_PWR == 0)) /* 按键松开标志�??????1, 且有任意�??????个按键按下了 */
  {
    delay_ms(10); /* 去抖�?????? */
    // key_up = 0;

    if (KEY_PWR == 0)
      keyval |= PWR_K;
  }
  else if (KEY_PWR == 1) /* 没有任何按键按下, 标记按键松开 */
  {
    // key_up = 1;
  }

  return keyval; /* 返回键�?? */
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
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  IIC_Init();
  delay_init(36);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  rx_freq[0] = 0x5283f07bu;
  tx_freq[0] = 0x528a8a8au;

  /*
      while (1)
    {
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
    delay_ms(1000);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
    delay_ms(1000);
    }*/

  rt = RX;
  tx_first = 1;
  rx_cnt = 0;
  pwr_open = 0;
  BK_Set_RX(rx_freq[0]);
  BK_Idle();
  delay_ms(1000);
  BK_Set_RX(rx_freq[0]);

  // uint16_t r2;
  uint8_t key;

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    if (pwr_open == 0)
    {
      if (key_scan() & PWR_K == PWR_K)
      {
        delay_ms(3000);
        if (key_scan() & PWR_K == PWR_K)
        {
          pwr_open = 1;
          HAL_GPIO_WritePin(PWR_O_GPIO_Port, PWR_O_Pin, GPIO_PIN_SET);
        }
      }
      else // 不会运行到这里来
      {
        continue;
      }
    }

    if (rt == RX)
    {
      if (BK_Get_SQ() < SQ_TH)
      {
        if (++rx_cnt > 5 && rx_cnt != 0xff)
        {
          BK_Open_Sound();
          rx_cnt = 0xff;
        }
      }
      else
      {
        if (rx_cnt == 0xff)
        {
          rx_cnt = 5;
        }
        else if (--rx_cnt == 0)
        {
          BK_Close_Sound();
        }
      }
    }

    key = key_scan();

    if (key & PTT == PTT && rt == RX)
    {
      BK_Set_TX(tx_freq[0]);
      rt = TX;
    }
    else if (key & PTT != PTT && rt == TX)
    {
      BK_Set_RX(rx_freq[0]);
      rt = RX;
    }

    if (key & PWR_K == PWR_K)
    {
      delay_ms(3000);
      if (key_scan() & PWR_K == PWR_K)
      {
        pwr_open = 0;
        HAL_GPIO_WritePin(PWR_O_GPIO_Port, PWR_O_Pin, GPIO_PIN_RESET); // 这句话之后松�??就会关机
        while (1)
          ;
      }
    }

    delay_ms(10);
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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

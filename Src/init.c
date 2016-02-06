#include "init.h"

#include "stm32l1xx_hal.h"
#include "stm32l152c_discovery_glass_lcd.h"
#include "tsl.h"
#include "tsl_user.h"

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC_Init(void);
static void MX_LCD_Init(void);
static void MX_TS_Init(void);
static void Timebase_Init(void);

static void Error_Handler(void);

// Also referenced in stm32l1xx_it.c
TIM_HandleTypeDef timHandle;


void InitAll(void)
{
    /* MCU Configuration----------------------------------------------------------*/
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();
    /* Configure the system clock to 32 MHz */
    SystemClock_Config();
    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_ADC_Init();
    MX_LCD_Init();
    MX_TS_Init();
    TSL_user_Init();
    BSP_LCD_GLASS_Init();
    Timebase_Init();

    BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

    HAL_GPIO_WritePin(GPIOB, LD3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, LD4_Pin, GPIO_PIN_RESET);
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInit;
    __PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = 16;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
    RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV3;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_LCD;
    PeriphClkInit.LCDClockSelection = RCC_RTCCLKSOURCE_LSE;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}


void MX_ADC_Init(void)
{
    ADC_ChannelConfTypeDef sConfig;
    /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
    */
    ADC_HandleTypeDef adcHandle;
    adcHandle.Instance = ADC1;
    adcHandle.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
    adcHandle.Init.Resolution = ADC_RESOLUTION12b;
    adcHandle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    adcHandle.Init.ScanConvMode = ADC_SCAN_DISABLE;
    adcHandle.Init.EOCSelection = EOC_SEQ_CONV;
    adcHandle.Init.LowPowerAutoWait = ADC_AUTOWAIT_DISABLE;
    adcHandle.Init.LowPowerAutoPowerOff = ADC_AUTOPOWEROFF_DISABLE;
    adcHandle.Init.ChannelsBank = ADC_CHANNELS_BANK_A;
    adcHandle.Init.ContinuousConvMode = DISABLE;
    adcHandle.Init.NbrOfConversion = 1;
    adcHandle.Init.DiscontinuousConvMode = DISABLE;
    adcHandle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    adcHandle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    adcHandle.Init.DMAContinuousRequests = DISABLE;
    HAL_ADC_Init(&adcHandle);
    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
    sConfig.Channel = ADC_CHANNEL_4;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_4CYCLES;
    HAL_ADC_ConfigChannel(&adcHandle, &sConfig);
}

/* LCD init function */
void MX_LCD_Init(void)
{
    LCD_HandleTypeDef lcdHandle;
    lcdHandle.Instance = LCD;
    lcdHandle.Init.Prescaler = LCD_PRESCALER_1;
    lcdHandle.Init.Divider = LCD_DIVIDER_31;
    lcdHandle.Init.Duty = LCD_DUTY_1_4;
    lcdHandle.Init.Bias = LCD_BIAS_1_3;
    lcdHandle.Init.VoltageSource = LCD_VOLTAGESOURCE_INTERNAL;
    lcdHandle.Init.Contrast = LCD_CONTRASTLEVEL_5;
    lcdHandle.Init.DeadTime = LCD_DEADTIME_0;
    lcdHandle.Init.PulseOnDuration = LCD_PULSEONDURATION_4;
    lcdHandle.Init.MuxSegment = LCD_MUXSEGMENT_ENABLE;
    lcdHandle.Init.BlinkMode = LCD_BLINKMODE_OFF;
    lcdHandle.Init.BlinkFrequency = LCD_BLINKFREQUENCY_DIV32;
    HAL_LCD_Init(&lcdHandle);
}


void MX_TS_Init(void)
{
}

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
     PA6   ------> TS_G2_IO1
     PA7   ------> TS_G2_IO2
     PC4   ------> TS_G9_IO1
     PC5   ------> TS_G9_IO2
     PB0   ------> TS_G3_IO1
     PB1   ------> TS_G3_IO2
*/
void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    /* GPIO Ports Clock Enable */
    __GPIOC_CLK_ENABLE();
    __GPIOA_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();
    /*Configure GPIO pin : IDD_CNT_EN_Pin */
    GPIO_InitStruct.Pin = IDD_CNT_EN_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_VERY_LOW;
    HAL_GPIO_Init(IDD_CNT_EN_GPIO_Port, &GPIO_InitStruct);
    /*Configure GPIO pin : B1_Pin */
    GPIO_InitStruct.Pin = B1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);
    /*Configure GPIO pins : GRP2_Sampling_Pin GRP2_Ground_Pin */
    GPIO_InitStruct.Pin = GRP2_Sampling_Pin|GRP2_Ground_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    /*Configure GPIO pins : GRP9_Sampling_Pin GRP9_Ground_Pin */
    GPIO_InitStruct.Pin = GRP9_Sampling_Pin|GRP9_Ground_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    /*Configure GPIO pins : GRP3_Sampling_Pin GRP3_Ground_Pin */
    GPIO_InitStruct.Pin = GRP3_Sampling_Pin|GRP3_Ground_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    /*Configure GPIO pins : LD4_Pin LD3_Pin */
    GPIO_InitStruct.Pin = LD4_Pin|LD3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_VERY_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}


void Timebase_Init(void) {
    /* Compute the prescaler value to have TIM3 counter clock equal to 10000 Hz */
    uint32_t uwPrescalerValue = (uint32_t)(SystemCoreClock / 10000) - 1;

    timHandle.Instance = TIM3;

    timHandle.Init.Period            = 10000 - 1;
    timHandle.Init.Prescaler         = uwPrescalerValue;
    timHandle.Init.ClockDivision     = 0;
    timHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;

    if (HAL_TIM_Base_Init(&timHandle) != HAL_OK) {
        Error_Handler();
    }

    /*##-2- Start the TIM Base generation in interrupt mode ####################*/
    /* Start Channel1 */
    if (HAL_TIM_Base_Start_IT(&timHandle) != HAL_OK) {
        Error_Handler();
    }
}


static void Error_Handler(void)
{
  BSP_LED_Off(LED3);
  BSP_LED_On(LED4);
  while (1)
  {
      BSP_LED_Toggle(LED_GREEN);
      BSP_LED_Toggle(LED_BLUE);
      HAL_Delay(10);
  }
}


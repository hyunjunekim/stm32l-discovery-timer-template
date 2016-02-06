#include "stm32l1xx_hal.h"


void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
  __HAL_RCC_TIM3_CLK_ENABLE();
  HAL_NVIC_SetPriority(TIM3_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(TIM3_IRQn);
}

void HAL_MspInit(void)
{
    __COMP_CLK_ENABLE();
    __SYSCFG_CLK_ENABLE();
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_0);
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    if (hadc->Instance==ADC1) {
        __ADC1_CLK_ENABLE();
        GPIO_InitStruct.Pin = IDD_Measurement_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(IDD_Measurement_GPIO_Port, &GPIO_InitStruct);
    }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance==ADC1) {
        __ADC1_CLK_DISABLE();
        HAL_GPIO_DeInit(IDD_Measurement_GPIO_Port, IDD_Measurement_Pin);
    }
}

void HAL_LCD_MspInit(LCD_HandleTypeDef* hlcd)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    if (hlcd->Instance==LCD) {
        __LCD_CLK_ENABLE();
        GPIO_InitStruct.Pin = SEG14_Pin|SEG15_Pin|SEG16_Pin|SEG17_Pin
                              |SEG18_Pin|SEG19_Pin|SEG20_Pin|SEG21_Pin
                              |SEG22_Pin|SEG23_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_VERY_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF11_LCD;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
        GPIO_InitStruct.Pin = SEG0_Pin|SEG1_Pin|SEG2_Pin|COM0_Pin
                              |COM1_Pin|COM2_Pin|SEG12_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_VERY_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF11_LCD;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        GPIO_InitStruct.Pin = SEG6_Pin|SEG7_Pin|SEG8_Pin|SEG9_Pin
                              |SEG10_Pin|SEG11_Pin|SEG3_Pin|SEG4_Pin
                              |SEG5_Pin|SEG13_Pin|COM3_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_VERY_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF11_LCD;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
}

void HAL_LCD_MspDeInit(LCD_HandleTypeDef* hlcd)
{
    if (hlcd->Instance==LCD) {
        __LCD_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOC, SEG14_Pin|SEG15_Pin|SEG16_Pin|SEG17_Pin
                        |SEG18_Pin|SEG19_Pin|SEG20_Pin|SEG21_Pin
                        |SEG22_Pin|SEG23_Pin);
        HAL_GPIO_DeInit(GPIOA, SEG0_Pin|SEG1_Pin|SEG2_Pin|COM0_Pin
                        |COM1_Pin|COM2_Pin|SEG12_Pin);
        HAL_GPIO_DeInit(GPIOB, SEG6_Pin|SEG7_Pin|SEG8_Pin|SEG9_Pin
                        |SEG10_Pin|SEG11_Pin|SEG3_Pin|SEG4_Pin
                        |SEG5_Pin|SEG13_Pin|COM3_Pin);
    }
}

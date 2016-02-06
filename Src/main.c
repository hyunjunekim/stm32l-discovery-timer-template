#include "stm32l1xx_hal.h"
#include "stm32l152c_discovery_glass_lcd.h"
#include "tsl.h"
#include "tsl_user.h"
#include "init.h"


void EXTI0_IRQHandler(void);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

void itoa(char* a, int i, int n);

int main(void)
{
    InitAll();

    HAL_GPIO_WritePin(GPIOB, LD3_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, LD4_Pin, GPIO_PIN_SET);

    BSP_LCD_GLASS_DisplayString((uint8_t*)"READY");

    while (1) {
//        // Read touch buttons
//        int buttonId = getTouchButtons();
//        if (buttonId != -1) {
//            // Have to use array syntax or string ends up in flash and can't be modified.
//            char s[] = " 0000 ";
//            s[buttonId + 1] = 255;
//            BSP_LCD_GLASS_DisplayString((uint8_t*)s);
//        }

        // Read touch slider.
        int sliderPercent = getTouchSliderPercent();
        if (sliderPercent == -1) {
            BSP_LCD_GLASS_DisplayString("------");
        }
        else {
            char s[] = "      ";
            itoa(s, sliderPercent, 6);
            BSP_LCD_GLASS_DisplayString((uint8_t*)s);
        }
    }
}

// User button handler.
void EXTI0_IRQHandler(void)
{
    BSP_LED_Toggle(LED_GREEN);
    HAL_GPIO_EXTI_IRQHandler(B1_Pin);
}

// One second timebase handler.
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    BSP_LED_Toggle(LED_BLUE);
}

// Convert {i} to an {n} digit ASCII string stored in {a}.
void itoa(char* a, int i, int n) {
    int j;
    for (j = n - 1; j >= 0; --j) {
        if (i) {
            a[j] = (i % 10) + '0';
            i /= 10;
        }
        else {
            a[j] = '0';
        }
    }
}

//#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
      ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}

//#endif

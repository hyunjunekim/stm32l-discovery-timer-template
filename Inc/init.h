#pragma once

void InitAll(void);

void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_ADC_Init(void);
void MX_LCD_Init(void);
void MX_TS_Init(void);
void Timebase_Init(void);

void Error_Handler(void);

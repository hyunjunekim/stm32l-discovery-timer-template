#pragma once

void InitAll(void);

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC_Init(void);
static void MX_LCD_Init(void);
static void MX_TS_Init(void);
static void Timebase_Init(void);

static void Error_Handler(void);

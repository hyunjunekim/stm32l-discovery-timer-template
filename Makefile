######################################
# Makefile by CubeMX2Makefile.py
# Modified by hand.
######################################

######################################
# target
######################################
TARGET = stm32l1-disco-template

######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -O0

#######################################
# pathes
#######################################
# source path
# Build path
BUILD_DIR = build

######################################
# source
######################################
C_SOURCES = \
  Drivers/CMSIS/Device/ST/STM32L1xx/Source/Templates/system_stm32l1xx.c \
  Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal.c \
  Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_adc.c \
  Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_adc_ex.c \
  Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_cortex.c \
  Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_dma.c \
  Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_flash.c \
  Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_flash_ex.c \
  Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_flash_ramfunc.c \
  Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_gpio.c \
  Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_lcd.c \
  Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_pwr.c \
  Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_pwr_ex.c \
  Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_rcc.c \
  Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_rcc_ex.c \
  Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_tim.c \
  Drivers/STM32_TouchSensing_Library/src/tsl.c \
  Drivers/STM32_TouchSensing_Library/src/tsl_acq.c \
  Drivers/STM32_TouchSensing_Library/src/tsl_acq_stm32l1xx_sw.c \
  Drivers/STM32_TouchSensing_Library/src/tsl_dxs.c \
  Drivers/STM32_TouchSensing_Library/src/tsl_ecs.c \
  Drivers/STM32_TouchSensing_Library/src/tsl_filter.c \
  Drivers/STM32_TouchSensing_Library/src/tsl_globals.c \
  Drivers/STM32_TouchSensing_Library/src/tsl_object.c \
  Drivers/STM32_TouchSensing_Library/src/tsl_linrot.c \
  Drivers/STM32_TouchSensing_Library/src/tsl_time.c \
  Drivers/STM32_TouchSensing_Library/src/tsl_time_stm32l1xx.c \
  Src/main.c \
  Src/stm32l1xx_hal_msp.c \
  Src/stm32l1xx_it.c \
  Src/stm32l152c_discovery.c \
  Src/stm32l152c_discovery_glass_lcd.c \
  Src/tsl_user.c \
  Src/init.c

  # Target part does not have hardware capture.
  #Drivers/STM32_TouchSensing_Library/src/tsl_touchkey.c \
  #Drivers/STM32_TouchSensing_Library/src/tsl_acq_stm32l1xx_hw.c \
  #Drivers/STM32_TouchSensing_Library/src/tsl_acq_tsc.c \

ASM_SOURCES = \
  Drivers/CMSIS/Device/ST/STM32L1xx/Source/Templates/gcc/startup_stm32l152xb.s

#######################################
# binaries
#######################################
CC = arm-none-eabi-gcc
AS = arm-none-eabi-gcc -x assembler-with-cpp
CP = arm-none-eabi-objcopy
AR = arm-none-eabi-ar
SZ = arm-none-eabi-size
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
 
#######################################
# CFLAGS
#######################################
# macros for gcc
AS_DEFS =
C_DEFS = -D__weak="__attribute__((weak))" -D__packed="__attribute__((__packed__))" -DUSE_HAL_DRIVER -DSTM32L152xB

# My device is a "medium density plus".
C_DEFS += -DSTM32L1XX_MDP


# includes for gcc
AS_INCLUDES =
C_INCLUDES = -IInc
C_INCLUDES += -IDrivers/STM32L1xx_HAL_Driver/Inc
C_INCLUDES += -IDrivers/STM32L1xx_HAL_Driver/Inc/Legacy
C_INCLUDES += -IDrivers/CMSIS/Include
C_INCLUDES += -IDrivers/CMSIS/Device/ST/STM32L1xx/Include
C_INCLUDES += -IDrivers/STM32_TouchSensing_Library/inc

# compile gcc flags
ASFLAGS = -mthumb -mcpu=cortex-m3 $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections
CFLAGS = -mthumb -mcpu=cortex-m3 $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections
ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif
# Generate dependency information
CFLAGS += -MD -MP -MF .dep/$(@F).d

#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = STM32L152RBTx_FLASH.ld
# libraries
LIBS = -lc -lm -lnosys
LIBDIR =
LDFLAGS = -mthumb -mcpu=cortex-m3 -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir -p $@		

#######################################
# clean up
#######################################
clean:
	-rm -fR .dep $(BUILD_DIR)
  
#######################################
# dependencies
#######################################
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

# *** EOF ***

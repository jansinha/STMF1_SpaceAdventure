TEMPLATEROOT = /l/arm/STM32-Template

# compilation flags for gdb

CFLAGS  += -g -std=c99 -I ../../Library
ASFLAGS += -g 
LDLIBS	+= -lm

# project files

vpath %.c path_to_ff9/src
vpath %.c path_to_ff9/src/option

OBJS=  $(STARTUP) game.o state.o display_state.o play_level.o landscape.o  
OBJS+= loadBMP.o play_sound.o player.o fataccess.o
OBJS+= ff.o mmcbb.o uart.o spi_drive.o I2C.o
OBJS+= st7735_init.o st7735_draw.o WiiLCDTranslate.o
OBJS+= stm32f10x_gpio.o stm32f10x_rcc.o misc.o core_cm3.o xprintf.o
OBJS+= stm32f10x_usart.o stm32f10x_spi.o stm32f10x_i2c.o
OBJS+= stm32f10x_tim.o stm32f10x_dac.o stm32f10x_dma.o
#OBJS= $(STARTUP) main.o player.o spidma.o ff.o mmcbb.o

# include common make file

include $(TEMPLATEROOT)/Makefile.common

#include "stm32f4xx.h"

#define LCD_REG      (*((volatile unsigned short *) 0x60000000))
#define LCD_RAM      (*((volatile unsigned short *) 0x60020000))


void FSMC_Init(void);
void TFT_Init(void);
void ILI9341_Set_Address(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2);
void draw_rect(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2, uint16_t colour);
void draw_pixel(uint16_t X, uint16_t Y, uint16_t colour);

#include <ili9341.h>
#include "stm32f4xx.h"


void FSMC_Init(void){
	//enable RCC for FSMC and both GPIO ports
	RCC->AHB3ENR |= RCC_AHB3ENR_FSMCEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOEEN;

	//setup pins as AF
	GPIOD->MODER |= GPIO_MODER_MODER0_1 | GPIO_MODER_MODER1_1 | GPIO_MODER_MODER4_1 | GPIO_MODER_MODER5_1 | GPIO_MODER_MODER7_1 | GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1 | GPIO_MODER_MODER11_1 | GPIO_MODER_MODER14_1 | GPIO_MODER_MODER15_1;
	GPIOE->MODER |= GPIO_MODER_MODER7_1 | GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1  | GPIO_MODER_MODER11_1 | GPIO_MODER_MODER12_1 | GPIO_MODER_MODER13_1 | GPIO_MODER_MODER14_1 |  GPIO_MODER_MODER15_1;
	//setup pins as PP
	GPIOD->OTYPER &= 0b0011000001001100;
	GPIOE->OTYPER &= 0b0000000001111111;
	//set speed to 100MHz
	GPIOD->OSPEEDR |= (0b11<<(2*0)) | (0b11<<(2*1)) | (0b11<<(2*4)) | (0b11<<(2*5)) | (0b11<<(2*7)) | (0b11<<(2*8)) | (0b11<<(2*9)) | (0b11<<(2*10)) | (0b11<<(2*11)) | (0b11<<(2*14)) | (0b11<<(2*15));
	GPIOE->OSPEEDR |= (0b11<<(2*7)) | (0b11<<(2*8)) | (0b11<<(2*9)) | (0b11<<(2*10)) | (0b11<<(2*11)) | (0b11<<(2*12)) | (0b11<<(2*13)) | (0b11<<(2*14)) | (0b11<<(2*15));
	//set NO pull-up or pull-down
	GPIOD->PUPDR &= ((0b11<<(2*2)) | (0b11<<(2*3)) | (0b11<<(2*6)) | (0b11<<(2*12)) | (0b11<<(2*13)));
	GPIOE->PUPDR &= ((0b11<<(2*0)) | (0b11<<(2*1)) | (0b11<<(2*2)) | (0b11<<(2*3)) | (0b11<<(2*4)) | (0b11<<(2*5)) | (0b11<<(2*6)));
	//set other pins to AF12 i.e as FSMC pins
	GPIOD->AFR[0] = (GPIOD->AFR[0] & (0b1111<<(4*2) | 0b1111<<(4*3) | 0b1111<<(4*6))) | 0b1100<<(4*0) | 0b1100<<(4*1) | 0b1100<<(4*4) | 0b1100<<(4*5) | 0b1100<<(4*7);
	GPIOD->AFR[1] = (GPIOD->AFR[1] & (0b1111<<(4*(13-8)) | 0b1111<<(4*(12-8)))) | 0b1100<<(4*(8-8)) | 0b1100<<(4*(9-8)) | 0b1100<<(4*(10-8)) | 0b1100<<(4*(11-8)) | 0b1100<<(4*(14-8)) | 0b1100<<(4*(15-8));
	GPIOE->AFR[0] = (GPIOE->AFR[0] & ~(0b1111<<7)) | 0b1100<<(4*7);
	GPIOE->AFR[1] = 0xCCCCCCCC;

	//setup FSMC on Bank1 NORSRAM1
	//setup timings of FSCM
	FSMC_Bank1->BTCR[1] = FSMC_BTR1_ADDSET_1 | FSMC_BTR1_DATAST_1;
	// Bank1 NOR/SRAM control register configuration
	FSMC_Bank1->BTCR[0] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN | FSMC_BCR1_MBKEN;
}

void TFT_Init(void){
	//hardware reset
	GPIOD->ODR &= ~GPIO_ODR_ODR_12;
	HAL_Delay(200);
	GPIOD->ODR |= GPIO_ODR_ODR_12;
	HAL_Delay(120);
	//SOFTWARE RESET
	LCD_REG = 0x01;
	HAL_Delay(1000);

	//POWER CONTROL A
	LCD_REG = 0xCB;
	LCD_RAM = 0x39;
	LCD_RAM = 0x2C;
	LCD_RAM = 0x00;
	LCD_RAM = 0x34;
	LCD_RAM = 0x02;

	//POWER CONTROL B
	LCD_REG = 0xCF;
	LCD_RAM = 0x00;
	LCD_RAM = 0xC1;
	LCD_RAM = 0x30;

	//DRIVER TIMING CONTROL A
	LCD_REG = 0xE8;
	LCD_RAM = 0x85;
	LCD_RAM = 0x00;
	LCD_RAM = 0x78;

	//DRIVER TIMING CONTROL B
	LCD_REG = 0xEA;
	LCD_RAM = 0x00;
	LCD_RAM = 0x00;

	//POWER ON SEQUENCE CONTROL
	LCD_REG = 0xED;
	LCD_RAM = 0x64;
	LCD_RAM = 0x03;
	LCD_RAM = 0x12;
	LCD_RAM = 0x81;

	//PUMP RATIO CONTROL
	LCD_REG = 0xF7;
	LCD_RAM = 0x20;

	//POWER CONTROL,VRH[5:0]
	LCD_REG = 0xC0;
	LCD_RAM = 0x23;

	//POWER CONTROL,SAP[2:0];BT[3:0]
	LCD_REG = 0xC1;
	LCD_RAM = 0x10;

	//VCM CONTROL
	LCD_REG = 0xC5;
	LCD_RAM = 0x3E;
	LCD_RAM = 0x28;

	//VCM CONTROL 2
	LCD_REG = 0xC7;
	LCD_RAM = 0x86;

	//MEMORY ACCESS CONTROL
	LCD_REG = 0x36;
	LCD_RAM = 0x48;

	//PIXEL FORMAT
	LCD_REG = 0x3A;
	LCD_RAM = 0x55;

	//FRAME RATIO CONTROL, STANDARD RGB COLOR
	LCD_REG = 0xB1;
	LCD_RAM = 0x00;
	LCD_RAM = 0x18;

	//DISPLAY FUNCTION CONTROL
	LCD_REG = 0xB6;
	LCD_RAM = 0x08;
	LCD_RAM = 0x82;
	LCD_RAM = 0x27;

	//3GAMMA FUNCTION DISABLE
	LCD_REG = 0xF2;
	LCD_RAM = 0x00;

	//GAMMA CURVE SELECTED
	LCD_REG = 0x26;
	LCD_RAM = 0x01;

	//POSITIVE GAMMA CORRECTION
	LCD_REG = 0xE0;
	LCD_RAM = 0x0F;
	LCD_RAM = 0x31;
	LCD_RAM = 0x2B;
	LCD_RAM = 0x0C;
	LCD_RAM = 0x0E;
	LCD_RAM = 0x08;
	LCD_RAM = 0x4E;
	LCD_RAM = 0xF1;
	LCD_RAM = 0x37;
	LCD_RAM = 0x07;
	LCD_RAM = 0x10;
	LCD_RAM = 0x03;
	LCD_RAM = 0x0E;
	LCD_RAM = 0x09;
	LCD_RAM = 0x00;

	//NEGATIVE GAMMA CORRECTION
	LCD_REG = 0xE1;
	LCD_RAM = 0x00;
	LCD_RAM = 0x0E;
	LCD_RAM = 0x14;
	LCD_RAM = 0x03;
	LCD_RAM = 0x11;
	LCD_RAM = 0x07;
	LCD_RAM = 0x31;
	LCD_RAM = 0xC1;
	LCD_RAM = 0x48;
	LCD_RAM = 0x08;
	LCD_RAM = 0x0F;
	LCD_RAM = 0x0C;
	LCD_RAM = 0x31;
	LCD_RAM = 0x36;
	LCD_RAM = 0x0F;

	//EXIT SLEEP
	LCD_REG = 0x11;
	HAL_Delay(120);

	//TURN ON DISPLAY
	LCD_REG = 0x29;

	//dispaly inversion
	//LCD_REG = 0x21);

	//setup Memory Access Control
	LCD_REG = 0x36;
	LCD_RAM = 0b00111111;
}

void ILI9341_Set_Address(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2){
	//set X min and max
	LCD_REG = 0x2A;
	LCD_RAM = X1>>8;
	LCD_RAM = X1;
	LCD_RAM = X2>>8;
	LCD_RAM = X2;

	//set y min and max
	LCD_REG = 0x2B;
	LCD_RAM = Y1>>8;
	LCD_RAM = Y1;
	LCD_RAM = Y2>>8;
	LCD_RAM = Y2;

	//write data command
	LCD_REG = 0x2C;
}

void draw_rect(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2, uint16_t colour){
	ILI9341_Set_Address(X1, Y1, X2, Y2);
	for( uint32_t a = 0; a < ((X2-X1+1)*(Y2-Y1+1)); a++ ){
		LCD_RAM = colour;
	}
}

void draw_pixel(uint16_t X, uint16_t Y, uint16_t colour){
	ILI9341_Set_Address(X, Y, X, Y);
	LCD_RAM = colour;
}




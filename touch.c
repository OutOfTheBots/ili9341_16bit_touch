#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

//global calibration varibles
float X_scale = 0.39106;
float Y_scale = 0.3012;
int16_t X_translate = 44;
int16_t Y_translate = 40;


void Touch_Init(void){
	  //enable GPIOA clock
	  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	  //enable ADC1 clock
	  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

	  //set X- and y- pull down to make ADC read 0 when not pressed
	  GPIOA->PUPDR = (GPIOA->PUPDR & ~(GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD3)) | GPIO_PUPDR_PUPD2_1 | GPIO_PUPDR_PUPD3_1;
	  //set ODR for X- and Y- pins to low so when set up output the will be low
	  GPIOA->ODR &= ~(GPIO_ODR_OD2 | GPIO_ODR_OD3);
	  //set ODR for X+ and Y+ pins to high so whenever set as output they will be high
	  GPIOA->ODR |= GPIO_ODR_OD0 | GPIO_ODR_OD1;
	  //set both pins as push pull
	  GPIOA->OTYPER &= ~(GPIO_OTYPER_OT0 | GPIO_OTYPER_OT1);

	  //turn ADC1 on and set EOCS to regular conversion
	  ADC1->CR2 |= ADC_CR2_ADON |  ADC_CR2_EOCS;
	  //set shortest sample time
	  ADC1->SMPR1 |= 0b001001;
	  //set resution to 10 bit
	  ADC1->CR1 = (ADC1->CR1 & ~(ADC_CR1_RES)) | ADC_CR1_RES_0;
}

uint16_t median(int n, uint16_t x[]) {
    int temp;
    int i, j;
    // the following two loops sort the array x in ascending order
    for(i=0; i<n-1; i++) {
        for(j=i+1; j<n; j++) {
            if(x[j] < x[i]) {
                // swap elements
                temp = x[i];
                x[i] = x[j];
                x[j] = temp;
            }
        }
    }
    return x[n/2];

}

uint16_t read_touchX(void){

	//reset all modes of lowest 4 pins
	GPIOA->MODER &= ~0b11111111;
	//set Y+ and Y- to output, set X+ to analog and X- to imput
	GPIOA->MODER |= GPIO_MODER_MODE1_0 | GPIO_MODER_MODE3_0 | GPIO_MODER_MODE0;

	//set to ADC to channel 0
	ADC1->SQR3 = 0;

	//we need short delay and this is currently way longer than needed.
	HAL_Delay(4);

	//we will get 5 conversions then use the median 1
	uint16_t adcs[5];
	for(uint8_t number = 0; number < 5; number++){
		//start ADC convertion
		ADC1->CR2 |= ADC_CR2_SWSTART;
		//wait for convertion to finish
		while ((ADC1->SR & ADC_SR_EOC) == 0){
		}
		adcs[number] = ADC1->DR;
	}

	uint16_t med = median(5, adcs);
	if (med < 70){
		return 5000;
	}
	//return 320 - ((med *0.396)- 48); 	//return result
	return med;
}

uint16_t read_touchY(void){

	//reset all modes of lowest 4 pins
	GPIOA->MODER &= ~0b11111111;
	//set X+ and X- to output, set Y+ to analog and Y- to input
	GPIOA->MODER |= GPIO_MODER_MODE0_0 | GPIO_MODER_MODE2_0 | GPIO_MODER_MODE1;

	//set to ADC to channel 1
	ADC1->SQR3 = 1;

	//we need short delay and this is currently way longer than needed.
	HAL_Delay(4);


	//we will get 5 conversions then use the median 1
	uint16_t adcs[5];
	for(uint8_t number = 0; number < 5; number++){
		//start ADC convertion
		ADC1->CR2 |= ADC_CR2_SWSTART;
		//wait for convertion to finish
		while ((ADC1->SR & ADC_SR_EOC) == 0){
		}
		adcs[number] = ADC1->DR;
	}
	//printf("%d\r\n", median(5, adcs));
	//cut off at 120
	uint16_t med = median(5, adcs);
	if (med < 120){
		return 5000;
	}

	//return (med * 0.31) - 41; 	//return result
	return med;
}

void calibrate(void){

	  //clear screen and draw white cross at top left
	  draw_rect(0,0,319,239, 0);
	  draw_rect(0,20,40,20, 0xffff);
	  draw_rect(20,0,20,40, 0xffff);

	  uint16_t X_raw, Y_raw, X_old, Y_old, X_min_med, Y_min_med, X_max_med, Y_max_med;
	  uint16_t X_array[5];
	  uint16_t Y_array[5];
	  uint8_t pos = 0;

	  //read pos of top left white cross
	  while (1){
		  X_raw = read_touchX();
		  Y_raw = read_touchY();
		  if ( (X_raw !=5000) & (Y_raw != 5000) & (abs(X_old - X_raw) < 3) & (abs(Y_old - Y_raw) < 3) ){
			  X_array[pos] = X_raw;
			  Y_array[pos] = Y_raw;
			  pos++;
			  if (pos == 5){
				  break;
			  }
		  }
		  X_old = X_raw;
		  Y_old = Y_raw;
	  }
	  X_min_med = median(5, X_array);
	  Y_min_med = median(5, Y_array);

	  //clear screen
	  draw_rect(0,0,319,239, 0);

	  //wait for no touch;
	  while ((read_touchX() != 5000) & (read_touchY() != 5000)){
	  }
	  //small wait to make sure pen is up
	  HAL_Delay(5000);

	  //draw white cross at bottom right
	  draw_rect(280,220,319,220, 0xffff);
	  draw_rect(300,200,300,239, 0xffff);

	  //read pos of bottom left cross
	  pos = 0;
	  while (1){
		  X_raw = read_touchX();
	  	  Y_raw = read_touchY();
	  	  if ( (X_raw !=5000) & (Y_raw != 5000) & (abs(X_old - X_raw) < 3) & (abs(Y_old - Y_raw) < 3) ){
	 		  X_array[pos] = X_raw;
	 		  Y_array[pos] = Y_raw;
	  		  pos++;
	  		  if (pos == 5){
	  			  break;
	  		  }
	  	  }
	  	  X_old = X_raw;
	  	  Y_old = Y_raw;
	  }
	  X_max_med = median(5, X_array);
	  Y_max_med = median(5, Y_array);

	  //clear screen
	  draw_rect(0,0,319,239, 0);
	  //wait for no touch;
	  while ((read_touchX() != 5000) & (read_touchY() != 5000)){
	  }
	  //small wait to make sure pen is up
	  HAL_Delay(5000);

	  X_scale = (280/(X_min_med - X_max_med +1.0));
	  Y_scale = (200/(Y_max_med - Y_min_med +1.0));
	  X_translate = (X_max_med * X_scale) - 20;
	  Y_translate = (Y_min_med * Y_scale) - 20;
	  printf("Y_scale = %f Y_trans%d\r\n", Y_scale, Y_translate);
}

uint8_t read_touch_cal (uint16_t *Xpos, uint16_t *Ypos){
	uint16_t x,y;
	x = read_touchX();
	y = read_touchY();
	if ( (x == 5000) | (y ==5000)){
		*Xpos = 5000;
		*Ypos = 5000;
		return 0;
	}
	*Xpos = 320 - ((x * X_scale) - X_translate);
	*Ypos = (y * Y_scale) - Y_translate;
	return 1;
}



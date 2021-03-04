/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f0xx.h"
			
void setup_portc() {
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    GPIOC->MODER &= ~(GPIO_MODER_MODER0_1 | GPIO_MODER_MODER1_1 |
                      GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1 |
                      GPIO_MODER_MODER4_1 | GPIO_MODER_MODER5_1 |
                      GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1 |
                      GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1 |
                      GPIO_MODER_MODER10_1);
    GPIOC->MODER |= (GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0 |
                     GPIO_MODER_MODER2_0 | GPIO_MODER_MODER3_0 |
                     GPIO_MODER_MODER4_0 | GPIO_MODER_MODER5_0 |
                     GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0 |
                     GPIO_MODER_MODER8_0 | GPIO_MODER_MODER9_0 |
                     GPIO_MODER_MODER10_0);
    GPIOC->ODR &= ~(0xffff);
    GPIOC->ODR |= 0x03f;
    return;
}

void copy_pa0_pc6() {
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER &= ~(GPIO_MODER_MODER0);
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR0_0);
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR0_1;
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    GPIOC->MODER &= ~(GPIO_MODER_MODER6_1);
    GPIOC->MODER |= GPIO_MODER_MODER6_0;
    for(;;) {
        int pa0 = ((GPIOA->IDR) & 0x1);
        GPIOC->BSRR = ((1<<6)<<16) | (pa0 << 6);
    }
    return;
}

int main(void)
{
    setup_portc();

    copy_pa0_pc6();

	while(1){

	}
}

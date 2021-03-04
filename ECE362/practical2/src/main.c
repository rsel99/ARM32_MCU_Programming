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

int counter = 0;
int enable = 0;

void nano_wait(unsigned int n) {
    asm(    "        mov r0,%0\n"
            "repeat: sub r0,#83\n"
            "        bgt repeat\n" : : "r"(n) : "r0", "cc");
}

void spi_init_oled () {
    nano_wait(1000000);
    spi_cmd(0x38);
    spi_cmd(0x08);
    spi_cmd(0x01);
    nano_wait(2000000);
    spi_cmd(0x06);
    spi_cmd(0x02);
    spi_cmd(0x0c);
}

//============================================================================
// set_row()    (Autotest #5)
// Set the row active on the keypad matrix.
// Parameters: none
//============================================================================
void set_row() {
    GPIOB->ODR |= 1<<2;
}

//============================================================================
// get_cols()    (Autotest #6)
// Read the column pins of the keypad matrix.
// Parameters: none
// Return value: The 4-bit value read from PC[7:4].
//============================================================================
int get_cols() {
    return (GPIOB->IDR >> 4) & 0xf;
}

void setup_spi2 () {
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

    GPIOB->MODER |= GPIO_MODER_MODER12_1 | GPIO_MODER_MODER13_1 | GPIO_MODER_MODER15_1;
    GPIOB->MODER &= ~(GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 | GPIO_MODER_MODER15_0);

    SPI2->CR1 |= SPI_CR1_BR | SPI_CR1_MSTR;
    SPI2->CR1 |= SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE;
    SPI2->CR2 |= SPI_CR2_DS_0 | SPI_CR2_DS_3 | SPI_CR2_NSSP | SPI_CR2_SSOE;
    SPI2->CR2 &= ~(SPI_CR2_DS_1 | SPI_CR2_DS_2);
    SPI2->CR2 |= SPI_CR2_TXDMAEN;
    SPI2->CR1 |= SPI_CR1_SPE;
}

void spi_cmd (int n) {
    while(((SPI2->SR) & SPI_SR_TXE) == 0);
    SPI2->DR = n;
}

void spi_data (int n) {
    while(((SPI2->SR) & SPI_SR_TXE) == 0);
    SPI2->DR = (n | 0x200);
}

void TIM14_IRQHandler() {
    TIM14->SR &= ~TIM_SR_UIF;

    int cols = get_cols();

    if (enable) {
        counter++;
        char buffer[20];
        sprintf(buffer, "%08d", counter);
        char * s = &(buffer[0]);
        spi_cmd(0x02);
        int i;
        for (i = 0; i < 20; i++) {
            spi_data(*s);
            s++;
        }
    }
    if ((cols & 1) == 1) {
        enable = 1;
        counter = 0;
    }
    if ((cols & 2) == 2) {
        enable = 0;
    }
}

void setup_tim14() {
    setup_spi2();
    RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;
    TIM14->PSC = 10000-1;
    TIM14->ARR = 480-1;
    TIM14->DIER |= TIM_DIER_UIE;
    TIM14->CR1 |= TIM_CR1_CEN;
    NVIC->ISER[0] = 1<<19;
}

void enable_ports() {
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN;

    GPIOB->MODER &= ~(GPIO_MODER_MODER0 | GPIO_MODER_MODER1 | GPIO_MODER_MODER2 | GPIO_MODER_MODER3 |
                      GPIO_MODER_MODER4 | GPIO_MODER_MODER5 | GPIO_MODER_MODER6 | GPIO_MODER_MODER7);
    GPIOB->MODER |= GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0 | GPIO_MODER_MODER2_0 | GPIO_MODER_MODER3_0;

    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR4 | GPIO_PUPDR_PUPDR5 | GPIO_PUPDR_PUPDR6 | GPIO_PUPDR_PUPDR7);
    GPIOB->PUPDR |= GPIO_PUPDR_PUPDR4_1 | GPIO_PUPDR_PUPDR5_1 | GPIO_PUPDR_PUPDR6_1 | GPIO_PUPDR_PUPDR7_1;

    GPIOC->MODER &= ~(GPIO_MODER_MODER0 | GPIO_MODER_MODER1 | GPIO_MODER_MODER2 | GPIO_MODER_MODER3 |
                      GPIO_MODER_MODER4 | GPIO_MODER_MODER5 | GPIO_MODER_MODER6 | GPIO_MODER_MODER7 |
                      GPIO_MODER_MODER8 | GPIO_MODER_MODER9 | GPIO_MODER_MODER10);
    GPIOC->MODER |= GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0 | GPIO_MODER_MODER2_0 | GPIO_MODER_MODER3_0 |
                    GPIO_MODER_MODER4_0 | GPIO_MODER_MODER5_0 | GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0 |
                    GPIO_MODER_MODER8_0 | GPIO_MODER_MODER9_0 | GPIO_MODER_MODER10_0;
}

int main(void) {
    enable_ports();
    setup_spi2();
    set_row();
    spi_init_oled();
	setup_tim14();
}

//============================================================================
// ECE 362 lab experiment 10 -- Asynchronous Serial Communication
//============================================================================

#include "stm32f0xx.h"
#include "ff.h"
#include "diskio.h"
#include "fifo.h"
#include "tty.h"
#include <string.h> // for memset()
#include <stdio.h> // for printf()

void advance_fattime(void);
void command_shell(void);

// Write your subroutines below.
void setup_usart5() {
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIODEN;

    GPIOC->MODER &= ~(GPIO_MODER_MODER12);
    GPIOD->MODER &= ~(GPIO_MODER_MODER2);
    GPIOC->MODER |= GPIO_MODER_MODER12_1;
    GPIOD->MODER |= GPIO_MODER_MODER2_1;

    GPIOC->AFR[1] &= ~(GPIO_AFRH_AFR12);
    GPIOD->AFR[0] &= ~(GPIO_AFRL_AFR2);
    GPIOC->AFR[1] |= 2<<16;
    GPIOD->AFR[0] |= 2<<8;

    RCC->APB1ENR |= RCC_APB1ENR_USART5EN;

    USART5->CR1 &= ~(USART_CR1_UE);
    USART5->CR1 &= ~(1<<12 | 1<<28);
    USART5->CR2 &= ~(USART_CR2_STOP);
//    USART5->CR1 &= ~(USART_CR1_PCE);
    USART5->CR1 &= ~(USART_CR1_OVER8);

    USART5->BRR &= ~(0xffff);
    USART5->BRR |= 0x1A1;

    USART5->CR1 |= (USART_CR1_TE | USART_CR1_RE);
    USART5->CR1 |= USART_CR1_UE;

    while ((USART5->ISR & USART_ISR_TEACK & USART_ISR_REACK) != (USART_ISR_TEACK & USART_ISR_REACK));
}

int simple_putchar(int param) {
    while ((USART5->ISR & USART_ISR_TXE) == 0);
    USART5->TDR = param;
    while ((USART5->ISR & USART_ISR_TC) != USART_ISR_TC);
    int trans = USART5->TDR & USART_TDR_TDR;
    return trans;
}

int better_putchar(int param) {
    if (param == 10) {
        while ((USART5->ISR & USART_ISR_TXE) == 0);
        USART5->TDR = 13;
        while ((USART5->ISR & USART_ISR_TC) != USART_ISR_TC);
    }
    while ((USART5->ISR & USART_ISR_TXE) == 0);
    USART5->TDR = param;
    while ((USART5->ISR & USART_ISR_TC) != USART_ISR_TC);
    int trans = USART5->TDR & USART_TDR_TDR;
    return trans;
}

int simple_getchar() {
    while ((USART5->ISR & USART_ISR_RXNE) == 0);
    int rec = USART5->RDR & USART_RDR_RDR;
    while ((USART5->ISR & USART_ISR_RXNE) != 0);
    return rec;
}

int better_getchar() {
    while ((USART5->ISR & USART_ISR_RXNE) == 0);
    int rec = USART5->RDR & USART_RDR_RDR;
    while ((USART5->ISR & USART_ISR_RXNE) != 0);
    if (rec == 13) {
        return 10;
    }
    return rec;
}

int interrupt_getchar() {
    char ch;

    while (fifo_newline(&input_fifo) == 0) {
        asm volatile ("wfi");
    }
    if (fifo_newline(&input_fifo) != 0) {
        ch = fifo_remove(&input_fifo);
    }
    return ch;
}

void USART3_4_5_6_7_8_IRQHandler() {
    if ((USART5->ISR & USART_ISR_ORE) != 0) {
        USART5->ICR |= USART_ICR_ORECF;
    }
    uint8_t rdr = (uint8_t) USART5->RDR;
    if (fifo_full(&input_fifo)) {
        return;
    }
    insert_echo_char(rdr);
}

void enable_tty_interrupt() {
    USART5->CR1 |= USART_CR1_RXNEIE;
    NVIC->ISER[0] = 1<<29;
}

void setup_spi1() {
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    GPIOA->MODER &= ~(GPIO_MODER_MODER4 | GPIO_MODER_MODER5 | GPIO_MODER_MODER6 | GPIO_MODER_MODER7);
    GPIOA->MODER |= GPIO_MODER_MODER4_1 | GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1;

    GPIOA->AFR[0] &= ~(GPIO_AFRL_AFR4 | GPIO_AFRL_AFR5 | GPIO_AFRL_AFR6 | GPIO_AFRL_AFR7);

    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR6);
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR6_0;

    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    SPI1->CR1 &= ~SPI_CR1_SPE;
    SPI1->CR1 |= SPI_CR1_BR;
    SPI1->CR1 &= ~(SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE);
    SPI1->CR1 |= SPI_CR1_MSTR;

    SPI1->CR2 |= SPI_CR2_NSSP;
    SPI1->CR2 &= ~(SPI_CR2_DS);
    SPI1->CR2 |= SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2;
    SPI1->CR2 |= SPI_CR2_FRXTH;

    SPI1->CR1 |= SPI_CR1_SPE;
}

void spi_high_speed () {
    SPI1->CR1 &= ~SPI_CR1_SPE;
    SPI1->CR1 &= ~SPI_CR1_BR;
    SPI1->CR1 |= SPI_CR1_BR_1;
    SPI1->CR1 |= SPI_CR1_SPE;
}

void TIM14_IRQHandler() {
   TIM14->SR &= ~TIM_SR_UIF;
   advance_fattime();
}

void setup_tim14() {
    RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;
    TIM14->PSC = 10000-1;
    TIM14->ARR = 240-1;
    TIM14->DIER |= TIM_DIER_UIE;
    TIM14->CR1 |= TIM_CR1_CEN;
    NVIC->ISER[0] = 1<<19;
}

int __io_putchar(int ch) {
    return better_putchar(ch);
}

int __io_getchar(void) {
    return interrupt_getchar();
}


// Write your subroutines above.

const char testline[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789\r\n";

int main()
{
    setup_usart5();

    // Uncomment these when you're asked to...
    setbuf(stdin, 0);
    setbuf(stdout, 0);
    setbuf(stderr, 0);

    // Test 2.2 simple_putchar()
    //
//    for(;;)
//        for(const char *t=testline; *t; t++)
//            simple_putchar(*t);

    // Test for 2.3 simple_getchar()
    //
//    for(;;)
//        simple_putchar( simple_getchar() );

    // Test for 2.4 and 2.5 __io_putchar() and __io_getchar()
    //
//    printf("Hello!\n");
//    for(;;)
//        putchar( getchar() );

    // Test for 2.6
    //
//    for(;;) {
//        printf("Enter string: ");
//        char line[100];
//        fgets(line, 99, stdin);
//        line[99] = '\0'; // just in case
//        printf("You entered: %s", line);
//    }

    // Test for 2.7
    //
//    enable_tty_interrupt();
//    for(;;) {
//        printf("Enter string: ");
//        char line[100];
//        fgets(line, 99, stdin);
//        line[99] = '\0'; // just in case
//        printf("You entered: %s", line);
//    }

    // Test for 2.8 Test the command shell and clock.
    //
    enable_tty_interrupt();
    setup_tim14();
    FATFS fs_storage;
    FATFS *fs = &fs_storage;
    f_mount(fs, "", 1);
    command_shell();

    return 0;
}

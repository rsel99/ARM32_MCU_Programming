//=============================================================================
// ECE 362 lab experiment 6 -- Analog Input/Output
//=============================================================================

#include "stm32f0xx.h"
#include <math.h>

// Be sure to change this to your login...
const char login[] = "selvara2";

void internal_clock(void);
void display_float(float);
void control(void);

//============================================================================
// setup_adc()    (Autotest #1)
// Configure the ADC peripheral and analog input pins.
// Parameters: none
//============================================================================
void setup_adc(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER |= GPIO_MODER_MODER0 | GPIO_MODER_MODER1;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    RCC->CR2 |= RCC_CR2_HSI14ON;
    while ((RCC->CR2 & RCC_CR2_HSI14RDY) == 0) {}
    ADC1->CR |= ADC_CR_ADEN;
    while ((ADC1->ISR & ADC_ISR_ADRDY) == 0) {}
}

//============================================================================
// start_adc_channel()    (Autotest #2)
// Select an ADC channel, and initiate an A-to-D conversion.
// Parameters: n: channel number
//============================================================================
void start_adc_channel(int n)
{
    ADC1->CHSELR = 0;
    ADC1->CHSELR |= 1 << n;
    while (ADC1->ISR & ADC_ISR_ADRDY == 0) {}
    ADC1->CR |= ADC_CR_ADSTART;
}

//============================================================================
// read_adc()    (Autotest #3)
// Wait for A-to-D conversion to complete, and return the result.
// Parameters: none
// Return value: converted result
//============================================================================
int read_adc(void)
{
    int result;
    while ((ADC1->ISR & ADC_ISR_EOC) == 0) {}
    result = ADC1->DR;
    return result;
}

//============================================================================
// setup_dac()    (Autotest #4)
// Configure the DAC peripheral and analog output pin.
// Parameters: none
//============================================================================
void setup_dac(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER |= GPIO_MODER_MODER4;
    RCC->APB1ENR |= RCC_APB1ENR_DACEN;
    DAC->CR |= DAC_CR_TSEL1 | DAC_CR_TEN1 | DAC_CR_EN1;
}

//============================================================================
// write_dac()    (Autotest #5)
// Write a sample to the right-aligned 12-bit DHR, and trigger conversion.
// Parameters: sample: value to write to the DHR
//============================================================================
void write_dac(int sample)
{
    DAC->DHR12R1 = sample;
    DAC->SWTRIGR |= DAC_SWTRIGR_SWTRIG1;
}


//============================================================================
// Parameters for the wavetable size and expected DAC rate.
//============================================================================
#define N 1000
#define RATE 20000
short int wavetable[N];

//============================================================================
// init_wavetable()    (Autotest #6)
// Write the pattern for one complete cycle of a sine wave into the
// wavetable[] array.
// Parameters: none
//============================================================================
void init_wavetable(void)
{
    for(int i=0; i < N; i++)
        wavetable[i] = 32767 * sin(2 * M_PI * i / N);
}

//============================================================================
// Global variables used for four-channel synthesis.
//============================================================================
int volume = 2048;
int stepa = 0;
int stepb = 0;
int stepc = 0;
int stepd = 0;
int offseta = 0;
int offsetb = 0;
int offsetc = 0;
int offsetd = 0;

//============================================================================
// set_freq_n()    (Autotest #7)
// Set the four step and four offset variables based on the frequency.
// Parameters: f: The floating-point frequency desired.
//============================================================================
void set_freq_a(float f)
{
    if (f != 0) {
        stepa = (f) * N / RATE * (1 << 16);
    }
    else {
        stepa = 0;
        offseta = 0;
    }
}

void set_freq_b(float f)
{
    if (f != 0) {
        stepb = (f) * N / RATE * (1 << 16);
    }
    else {
        stepb = 0;
        offsetb = 0;
    }
}

void set_freq_c(float f)
{
    if (f != 0) {
        stepc = (f) * N / RATE * (1 << 16);
    }
    else {
        stepc = 0;
        offsetc = 0;
    }
}

void set_freq_d(float f)
{
    if (f != 0) {
        stepd = (f) * N / RATE * (1 << 16);
    }
    else {
        stepd = 0;
        offsetd = 0;
    }
}

//============================================================================
// Timer 6 ISR    (Autotest #8)
// The ISR for Timer 6 which computes the DAC samples.
// Parameters: none
// (Write the entire subroutine below.)
//============================================================================
void TIM6_DAC_IRQHandler() {
    TIM6->SR &= ~TIM_SR_UIF;
    DAC->SWTRIGR |= DAC_SWTRIGR_SWTRIG1;
    offseta += stepa;
    offsetb += stepb;
    offsetc += stepc;
    offsetd += stepd;

    if ((offseta>>16) >= N)
        offseta -= N<<16;
    if ((offsetb>>16) >= N)
        offsetb -= N<<16;
    if ((offsetc>>16) >= N)
        offsetc -= N<<16;
    if ((offsetd>>16) >= N)
        offsetd -= N<<16;

    int sample = wavetable[offseta>>16] + wavetable[offsetb>>16] + wavetable[offsetc>>16] + wavetable[offsetd>>16];
    sample = ((sample * volume)>>16) + 2048;

    if (sample > 4095) {
        sample = 4095;
    }
    else if (sample < 0) {
        sample = 0;
    }

    DAC->DHR12R1 = sample;
    start_adc_channel(0);
    volume = read_adc();
}


//============================================================================
// setup_tim6()    (Autotest #9)
// Configure Timer 6 to raise an interrupt RATE times per second.
// Parameters: none
//============================================================================
void setup_tim6(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    TIM6->PSC = 200-1;
    TIM6->ARR = 12-1;
    TIM6->DIER |= TIM_DIER_UIE;
    TIM6->CR1 |= TIM_CR1_CEN;
    NVIC->ISER[0] = 1<<17;
    NVIC_SetPriority(17,1);
}

int main(void)
{
    //internal_clock(); // Use the internal oscillator if you need it
    autotest(); // test all of the subroutines you wrote
    init_wavetable();
    setup_dac();
    setup_adc();
    setup_tim6();
    set_freq_a(261.626); // Middle 'C'
    set_freq_b(329.628); // The 'E' above middle 'C'
    control();
    while(1) {
        for(int out=0; out<4096; out++) {
            if ((TIM6->CR1 & TIM_CR1_CEN) == 0)
                write_dac(out);
            start_adc_channel(0);
            int sample = read_adc();
            float level = 2.95 * sample / 4095;
            display_float(level);
        }
    }
}

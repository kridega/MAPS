/*
 * pwm.c
 *
 *  Created on: Mar 16, 2026
 *      Author: kridega
 */

#include "stm32f4xx.h"
#include "pwm.h"
#include <stdint.h>
#include "systick.h"

void motor_init(void)
{
    /* Enable clocks */

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

    /* -------- DIR PINS (PB0-PB3) -------- */

    GPIOB->MODER &= ~(0xFF);
    GPIOB->MODER |=  (0x55);   // output

    /* -------- PWM PINS (PB6-PB9) -------- */

    GPIOB->MODER &= ~(0xFF000);
    GPIOB->MODER |=  (0xAA000);

    GPIOB->MODER &= ~(0xFF00000);
    GPIOB->MODER |=  (0xAA00000);

    /* Alternate function AF2 for TIM4 */

    GPIOB->AFR[0] &= ~(0xFF000000);
    GPIOB->AFR[0] |=  (0x22000000);

    GPIOB->AFR[1] &= ~(0xFF);
    GPIOB->AFR[1] |=  (0x22);

    /* -------- TIMER CONFIG -------- */

    TIM4->PSC = 15;     // 16 MHz / 16 = 1 MHz timer clock
    TIM4->ARR = 1023;    // 10-bit resolution

    /* PWM Mode 1 */

    TIM4->CCMR1 |= (6<<4)|(1<<3);
    TIM4->CCMR1 |= (6<<12)|(1<<11);

    TIM4->CCMR2 |= (6<<4)|(1<<3);
    TIM4->CCMR2 |= (6<<12)|(1<<11);

    /* Enable channels */

    TIM4->CCER |= (1<<0)|(1<<4)|(1<<8)|(1<<12);

    /* Start timer */

    TIM4->CR1 |= TIM_CR1_ARPE;
    TIM4->CR1 |= TIM_CR1_CEN;
}

void pwm_write(uint8_t ch, uint16_t duty)
{
    if(duty > 1023) duty = 1023;

    if(ch==1) TIM4->CCR1 = duty;
    else if(ch==2) TIM4->CCR2 = duty;
    else if(ch==3) TIM4->CCR3 = duty;
    else if(ch==4) TIM4->CCR4 = duty;
}

void set_dir(uint8_t motor, uint8_t dir)
{
    if(dir) GPIOB->ODR |=  (1<<(motor-1));
    else    GPIOB->ODR &= ~(1<<(motor-1));
}

void move_motor(uint8_t motor, uint16_t duty, uint8_t dir)
{
    set_dir(motor, dir);
    pwm_write(motor, duty);
}

void movey_forward(uint16_t duty){
	move_motor(2, duty, 1);
	move_motor(4, duty, 0);
}

void movey_backward(uint16_t duty){
	move_motor(2, duty, 0);
	move_motor(4, duty, 1);
}

void movex_right(uint16_t duty){
	move_motor(1, duty, 1);
	move_motor(3, duty, 1);
}

void movex_left(uint16_t duty){
	move_motor(1, duty, 0);
	move_motor(3, duty, 0);
}

void stop(){
	move_motor(1, 0, 0);
	move_motor(2, 0, 0);
	move_motor(3, 0, 0);
	move_motor(4, 0, 1);
	systickDelayMs_polling(80);
}

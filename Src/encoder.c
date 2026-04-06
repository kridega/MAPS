/*
 * encoder.c
 *
 *  Created on: Mar 16, 2026
 *      Author: kridega
 */


#include "encoder.h"
#include <stdint.h>
#include "uart.h"
#include <stdio.h>

#define WHEEL_CIRCUM 203 // mm
#define ENCODER_CPR  600       // counts per revolution
// Encoder counts
volatile int32_t count1 = 0;
volatile int32_t count2 = 0;
volatile int32_t count3 = 0;
volatile int32_t count4 = 0;




// Returns x-axis position in mm (uses encoder 1 )
float get_position_x_mm(void)
{
    // position = (counts / counts_per_rev) * wheel_circumference
    return (float)(((float)(int32_t)count1 * (int32_t)WHEEL_CIRCUM) / (float)ENCODER_CPR);
}

float get_position_y_mm(void)
{
	return (float)(((int32_t)count3 * (int32_t)WHEEL_CIRCUM) / ENCODER_CPR);
}

/* ================= INIT ================= */

void encoder_init(void)
{
    // Enable GPIOA clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // Set PA0–PA7 as input
    GPIOA->MODER &= ~(0x0000FFFF);

    // Enable pull-up for stability
    GPIOA->PUPDR |= 0x00005555;

    // Enable SYSCFG clock
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    // Map EXTI0–3 to PA0–PA3
    SYSCFG->EXTICR[0] &= ~(0xFFFF);

    // Enable interrupt mask
    EXTI->IMR |= (1<<0)|(1<<1)|(1<<2)|(1<<3);

    // Rising edge
    EXTI->RTSR |= (1<<0)|(1<<1)|(1<<2)|(1<<3);

    // Falling edge
    EXTI->FTSR |= (1<<0)|(1<<1)|(1<<2)|(1<<3);

    // Enable NVIC interrupts
    NVIC_EnableIRQ(EXTI0_IRQn);
    NVIC_EnableIRQ(EXTI1_IRQn);
    //NVIC_EnableIRQ(EXTI2_IRQn);
    NVIC_EnableIRQ(EXTI3_IRQn);

    NVIC_SetPriority(EXTI0_IRQn, 0);
    NVIC_SetPriority(EXTI1_IRQn, 0);
    //NVIC_SetPriority(EXTI2_IRQn, 0);
    NVIC_SetPriority(EXTI3_IRQn, 0);

    // Enable GPIOC clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

    // Set PC10 and PC12 as input
    GPIOC->MODER &= ~((3<<(10*2)) | (3<<(12*2)));

    // Enable pull-up for PC10, PC12
    GPIOC->PUPDR &= ~((3<<(10*2)) | (3<<(12*2)));
    GPIOC->PUPDR |=  ((1<<(10*2)) | (1<<(12*2)));

    // Map EXTI10 to Port C
    SYSCFG->EXTICR[2] &= ~(0xF << 8);   // clear EXTI10
    SYSCFG->EXTICR[2] |=  (0x2 << 8);   // Port C = 0010

    // Enable EXTI10 interrupt
    EXTI->IMR  |= (1<<10);
    EXTI->RTSR |= (1<<10);
    EXTI->FTSR |= (1<<10);

    // Enable NVIC for EXTI10–15
    NVIC_EnableIRQ(EXTI15_10_IRQn);
    NVIC_SetPriority(EXTI15_10_IRQn, 0);

}



void get_encoder_data(int32_t *enc1, int32_t *enc2, int32_t *enc3, int32_t *enc4){
	*enc1=count1;
	*enc2=count2;
	*enc3=count3;
	*enc4=count4;
}

void reset_encoders(void)
{
    count1 = 0;
    count2 = 0;
    count3 = 0;
    count4 = 0;
}

void log_encoder_data(){
	printf("enc1: %ld , enc2: %ld , enc3: %ld , enc4: %ld \r\n",count1,count2,count3,count4);
	printf("posx: %f , posy : %f \r\n",get_position_x_mm(),get_position_y_mm());
}
// Encoder 1
void EXTI0_IRQHandler(void)
{
    if (EXTI->PR & (1<<0))
    {
        uint8_t A = (GPIOA->IDR & (1<<0)) ? 1 : 0;
        uint8_t B = (GPIOA->IDR & (1<<4)) ? 1 : 0;

        if(A==B) count1--;
        else count1++;

        EXTI->PR |= (1<<0);
    }
}

/*
// Encoder 2
void EXTI1_IRQHandler(void)
{
    if (EXTI->PR & (1<<1))
    {
        uint8_t A = (GPIOA->IDR & (1<<1)) ? 1 : 0;
        uint8_t B = (GPIOA->IDR & (1<<5)) ? 1 : 0;

        if(A==B) count2++;
        else count2--;

        EXTI->PR |= (1<<1);
    }
}
*/
/*
// Encoder 3
void EXTI2_IRQHandler(void)
{
    if (EXTI->PR & (1<<2))
    {
        uint8_t A = (GPIOA->IDR & (1<<2)) ? 1 : 0;
        uint8_t B = (GPIOA->IDR & (1<<6)) ? 1 : 0;

        if(A==B) count3++;
        else count3--;

        EXTI->PR |= (1<<2);
    }
}
*/
/*
// Encoder 4
void EXTI3_IRQHandler(void)
{
    if (EXTI->PR & (1<<3))
    {
        uint8_t A = (GPIOA->IDR & (1<<3)) ? 1 : 0;
        uint8_t B = (GPIOA->IDR & (1<<7)) ? 1 : 0;

        if(A==B) count4++;
        else count4--;

        EXTI->PR |= (1<<3);
    }
}
*/
// encoder 3
void EXTI15_10_IRQHandler(void)
{
    if (EXTI->PR & (1<<10))   // EXTI line 10
    {
        uint32_t idr = GPIOC->IDR;   // read once (important)

        uint8_t A = (idr >> 10) & 1;   // PC10
        uint8_t B = (idr >> 12) & 1;   // PC12

        if (A == B)
            count3++;
        else
            count3--;

        EXTI->PR |= (1<<10);   // clear pending
    }
}

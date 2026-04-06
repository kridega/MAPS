#include "uart.h"
#include "encoder.h"

static uint16_t compute_uart2_brr(uint32_t periphclk,uint32_t baudrate);
void uart2_write(int ch);



void uart2_print(const char* str){
	while(*str){
		uart2_write(*str++);
	}
}

void uart2_rxtx_init(void){
    __disable_irq();

    // clock access GPIOD (bit 3, not bit 0 for GPIOA)
    RCC->AHB1ENR |= 1<<3;

    // PD5, PD6 alternate function mode (bits 11:10 and 13:12)
    GPIOD->MODER |=  (1<<11); GPIOD->MODER &= ~(1<<10);   // PD5
    GPIOD->MODER |=  (1<<13); GPIOD->MODER &= ~(1<<12);   // PD6

    // PD5 AF07 (TX), PD6 AF07 (RX)
    // PD5 → AFR[0] bits [23:20], PD6 → AFR[0] bits [27:24]
    GPIOD->AFR[0] |= (0b0111 << 20);   // PD5 AF7
    GPIOD->AFR[0] |= (0b0111 << 24);   // PD6 AF7

    // clock access UART2
    RCC->APB1ENR |= 1<<17;

    // configure baudrate
    USART2->BRR = compute_uart2_brr(16000000, 115200);

    USART2->CR1 |= 1<<3 | 1<<2;   // TX, RX enable
    __enable_irq();

    // enable UART2
    USART2->CR1 |= 1<<13;
}


static uint16_t compute_uart2_brr(uint32_t periphclk,uint32_t baudrate){
	return (periphclk +(baudrate/2U))/baudrate;
}

char uart2_read(void){
	// make sure receive data register is ready
	while(!(USART2->SR & (1<<5)));
	return USART2->DR;
}

void uart2_write(int ch){
	// make sure transmit data register empty
	while(!(USART2->SR & (1<<7)));
	// write to transmit data register
	USART2->DR=ch & 0xFF;
}


void dma1_stream6_init(uint32_t src,uint32_t dst,uint32_t len){
	// stream 6 , channel 4 USART2_TX
	// enable clock access dma
	RCC->AHB1ENR|=1<<21;
	// disable dma1 stream 6
	DMA1_Stream6->CR &= ~(1<<0);
	// advisable to wait until stream it is disabled
	while(DMA1_Stream6->CR & (1<<0));
	// clear all interupt flag of stream 6
	DMA1->HIFCR |= (1<<16) | (1<<18) | (1<<19) | (1<<20) | (1<<21);
	// set the destination buffer
	DMA1_Stream6->PAR=dst;
	// set the source buffer
	DMA1_Stream6->M0AR=src;
	// set length
	DMA1_Stream6->NDTR=len;
	// select channel 4
	DMA1_Stream6->CR |= 1<<27;
	// enable memory increment
	DMA1_Stream6->CR|=1<<10;
	// select transfer direction
	DMA1_Stream6->CR|=1<<6; // memory to peripheral
	// enable transfer complete interrupt
	DMA1_Stream6->CR|=1<<4;
	// enable direct mode , disable fifo
	DMA1_Stream6->FCR=0;
	// enable USART2 TX DMA
	USART2->CR3|=1<<7;
	// enable dma1 stream 6
	DMA1_Stream6->CR |=1<<0;

	NVIC_EnableIRQ(DMA1_Stream6_IRQn);

}


void uart2_write_floatx(){
	int32_t posx=(int32_t)get_position_x_mm();
	for(int i=0;i<4;i++){
		uart2_write(((unsigned char *)&posx)[i]);
	}
	uart2_write(0x20); // space
}

void uart2_write_floaty(){
	int32_t posy=(int32_t)get_position_y_mm();
	uint8_t *p = (uint8_t*)&posy;
	for(int i=0;i<4;i++){
		uart2_write(p[i]);
	}
	uart2_print("\r\n"); // space
}

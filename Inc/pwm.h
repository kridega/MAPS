/*
 * pwm.h
 *
 *  Created on: Mar 16, 2026
 *      Author: kridega
 */

#ifndef PWM_H_
#define PWM_H_

#include "stm32f4xx.h"
#include <stdint.h>

void motor_init(void);
void move_motor(uint8_t motor, uint16_t duty, uint8_t dir);
void movey_forward(uint16_t duty);
void movey_backward(uint16_t duty);
void movex_right(uint16_t duty);
void movex_left(uint16_t duty);
void stop();

#endif /* PWM_H_ */

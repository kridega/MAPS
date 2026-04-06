/*
 * x_pid.h
 *
 *  Created on: Mar 23, 2026
 *      Author: kridega
 */

#ifndef X_PID_H_
#define X_PID_H_

#include "stm32f4xx.h"
#include <stdint.h>

void x_pid_init(void);
void x_pid_set_target(int32_t target_mm);
void x_pid_update(float dt,uint8_t *flag_x);
void x_pid_reset(void);
void x_get_pid_params(float *kpp,float *kii,float *kdd);
void x_set_pid_params(float *new_kp, float *new_ki, float *new_kd);
//void x_calibrate_min_duty(void);

#endif /* X_PID_H_ */

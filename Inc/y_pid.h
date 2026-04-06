/*
 * y_pid.h
 *
 *  Created on: Mar 24, 2026
 *      Author: parth
 */

#ifndef Y_PID_H_
#define Y_PID_H_


#include <stdint.h>

void y_pid_init(void);
void y_pid_set_target(int32_t target_mm);
void y_pid_update(float dt,uint8_t *flag_y);
void y_pid_reset(void);
void y_get_pid_params(float *kpp,float *kii,float *kdd);
void y_set_pid_params(float *new_kp, float *new_ki, float *new_kd);

#endif /* Y_PID_H_ */

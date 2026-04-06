/*
 * encoder.h
 *
 *  Created on: Mar 16, 2026
 *      Author: kridega
 */

#ifndef ENCODER_H_
#define ENCODER_H_

#include "stm32f4xx.h"
#include <stdint.h>

void encoder_init(void);
void get_encoder_data(int32_t *enc1, int32_t *enc2, int32_t *enc3, int32_t *enc4);
float get_position_x_mm(void);
float get_position_y_mm(void);
void reset_encoders(void);
void log_encoder_data();

#endif /* ENCODER_H_ */

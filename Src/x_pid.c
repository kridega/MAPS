/*
 * x_pid.c
 *
 *  PID position controller for the X axis.
 *  Target is set in mm via x_pid_set_target().
 *  x_pid_update(dt) must be called at a fixed interval (e.g. every 10 ms).
 *
 *  Motors 1 & 3 drive the X axis.
 */

#include "x_pid.h"
#include "encoder.h"
#include "pwm.h"
#include <stdint.h>
#include <math.h>

/* ------------------------------------------------------------------ */
/*  Tuning parameters                                                   */
/* ------------------------------------------------------------------ */

#define X_INTEGRAL_LIMIT   5000.0f
#define X_DEADBAND_MM       100.0f
#define X_SPEED_THRESHOLD   200.0f    // mm/s
#define MAX_PWM 500.0f


/* ------------------------------------------------------------------ */
/*  Internal state                                                      */
/* ------------------------------------------------------------------ */

static float   kp = 1.0f;
static float   ki = 0.01f;
static float   kd = 0.1f;

static int32_t target_mm   = 0;
static float   integral    = 0.0f;
static float   prev_error  = 0.0f;
static uint8_t pid_active  = 0;
static float   output_max  = 1.0f;    // set dynamically on each new target

static uint16_t x_min_duty = 150;    // fallback default

uint16_t temp=0;

/* ------------------------------------------------------------------ */
/*  Public API                                                          */
/* ------------------------------------------------------------------ */

void x_pid_init(void)
{
    target_mm  = 0;
    integral   = 0.0f;
    prev_error = 0.0f;
    pid_active = 0;
    output_max = 1.0f;
}

void x_pid_set_target(int32_t new_target_mm)
{
    if (new_target_mm != target_mm)
    {
        integral   = 0.0f;
        prev_error = 0.0f;

        /* dynamic output normalisation —
         * captures max expected PID output at the start of this move
         * so mapping to [0,1023] works for any target distance         */
        float current_mm  = get_position_x_mm();
        float   initial_err = fabsf((float)(new_target_mm - current_mm));
        output_max = (kp * initial_err) * 1.2f;   // 1.2 headroom for I and D terms

        /* safety floor — prevent divide-by-zero on zero-distance move */
        if (output_max < 1.0f) output_max = 1.0f;
    }

    target_mm  = new_target_mm;
    pid_active = 1;
}

void x_pid_reset(void)
{
    integral   = 0.0f;
    prev_error = 0.0f;
    pid_active = 0;
    output_max = 1.0f;
    stop();
}

/* ------------------------------------------------------------------ */
/*  PID update — call every dt seconds from TIM2 ISR                  */
/* ------------------------------------------------------------------ */

void x_pid_update(float dt,uint8_t *flag_x)
{
    if (!pid_active || dt <= 0.0f) return;

    /* 1. Measure current position */
    float current_mm = get_position_x_mm();

    /* 2. Compute error (mm) */
    float error = (float)(target_mm - current_mm);

    /* 3. Speed estimate (mm/s) */
    float speed = fabsf(error - prev_error) / dt;

    /* 4. Deadband + speed check — declare arrival */
    if (fabsf(error) < X_DEADBAND_MM && speed < X_SPEED_THRESHOLD)
    {
        stop();
        integral   = 0.0f;
        prev_error = 0.0f;
        pid_active = 0;    // disarm — timer keeps running, ready for next target
        *flag_x=1;
        return;
    }

    /* 5. Proportional term */
    float p_term = kp * error;

    /* 6. Integral term with anti-windup clamp */
    integral += error * dt;
    if      (integral >  X_INTEGRAL_LIMIT) integral =  X_INTEGRAL_LIMIT;
    else if (integral < -X_INTEGRAL_LIMIT) integral = -X_INTEGRAL_LIMIT;
    float i_term = ki * integral;

    /* 7. Derivative term (on error — avoids setpoint kick) */
    float derivative = (error - prev_error) / dt;
    float d_term     = kd * derivative;
    prev_error = error;

    /* 8. Sum → raw output */
    float output = p_term + i_term + d_term;

    /* 9. Normalise raw output → [0, 1023]
     *    output_max is in mm (PID units)
     *    1023 is PWM ceiling (timer counts)
     *    ratio converts mm-space → PWM-space                          */
    float abs_out = fabsf(output);
    float mapped  = (abs_out / output_max) * MAX_PWM;

    if (mapped > MAX_PWM) mapped = MAX_PWM;              // hard ceiling — CCR never > ARR
    if (mapped > 0.0f && mapped < (float)x_min_duty){ 		// stall floor
    	mapped = (float)x_min_duty;
    }


    uint16_t duty = (uint16_t)mapped;
    temp=duty;

    /* 10. Drive motors via helpers — correct wiring guaranteed */
    if (output >= 0.0f) movex_right(duty);
    else                movex_left(duty);
}

void x_get_pid_params(float *kpp,float *kii,float *kdd){
	*kpp=kp; *kii=ki; *kdd=kd;
}

void x_set_pid_params(float *new_kp, float *new_ki, float *new_kd)
{
    kp = *new_kp;
    ki = *new_ki;
    kd = *new_kd;

}

/*
void x_calibrate_min_duty(void)
{
    int32_t pos_before, pos_after;
    uint16_t test_duty = 50;          // start very low

    // reset encoder so we have a clean reference
    count1 = 0;
    count2 = 0;

    while (test_duty <= 500)          // safety ceiling — never burn motor
    {
        // apply test duty forward
        movex_right(test_duty);

        // wait long enough for motor to respond
        HAL_Delay(30);                // 30ms — enough for one encoder pulse

        pos_after = get_position_x_mm();

        // did the encoder register any movement?
        if (pos_after != 0)
        {
            x_min_duty = test_duty + 20;   // +20 safety margin above bare minimum
            break;
        }

        test_duty += 10;              // ramp up slowly
    }

    // stop motor after calibration
    stop();

    // reset encoder back to zero after nudge
    count1 = 0;
    count2 = 0;
}
*/

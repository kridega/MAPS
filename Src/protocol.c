/*
 * protocol.c
 *
 *  Created on: Mar 29, 2026
 *      Author: parth
 */
#include "protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include "uart6.h"
#include "x_pid.h"
#include "y_pid.h"
#include <string.h>
#include "main.h"

#define ACK 0xFA
#define NACK 0xFB
#define START 0xFC

#define CMD_BUF_SIZE  64

extern int32_t data_arr_x[3000];
extern int32_t data_arr_y[3000];
extern uint16_t count_x;
extern uint16_t count_y;

void send_data(int32_t* data_x, int32_t* data_y, uint16_t count_x, uint16_t count_y){

    printf("X\r\n");

    for(int i=0; i<count_x; i++){
        printf("%ld;\r\n",data_x[i]);
    }

    printf("Y\r\n");

    for(int i=0; i<count_y; i++){
        printf("%ld;\r\n",data_y[i]);
    }


}

void read_32(int32_t *data){
    *data = ((int32_t)uart6_read() << 0);
    *data |= ((int32_t)uart6_read() << 8);
    *data |= ((int32_t)uart6_read() << 16);
    *data |= ((int32_t)uart6_read() << 24);
}


void handshake(int32_t *posx, int32_t *posy){
	uint8_t tem;

	tem=uart6_read();
	if(tem==START){
		uart6_write(ACK);
	}

	read_32(posx);
	read_32(posy);

}




static void proto_send_ok(void)
{
    printf("<OK>\r\n");
}

static void proto_send_err(const char *reason)
{
    printf("<ERR %s>\r\n", reason);
}



/* ------------------------------------------------------------------ */
/*  Command parser                                                      */
/* ------------------------------------------------------------------ */

static void handle_command(char *cmd)
{
    /* ---- P : get both X and Y PID params --------------------------- */
    if (strcmp(cmd, "P") == 0)
    {
        float kp, ki, kd;
        x_get_pid_params(&kp, &ki, &kd);
        printf("<PIDX %.4f %.4f %.4f>\r\n", kp, ki, kd);
        y_get_pid_params(&kp, &ki, &kd);
        printf("<PIDY %.4f %.4f %.4f>\r\n", kp, ki, kd);
        return;
    }

    /* ---- SX kp ki kd : set X axis PID params ----------------------- */
    if (cmd[0] == 'S' && cmd[1] == 'X' && cmd[2] == ' ')
    {
        float kp, ki, kd;
        int parsed = sscanf(cmd + 3, "%f %f %f", &kp, &ki, &kd);
        if (parsed != 3)
        {
            proto_send_err("SX needs 3 floats");
            return;
        }
        x_set_pid_params(&kp, &ki, &kd);
        proto_send_ok();
        printf("<PIDX %.4f %.4f %.4f>\r\n", kp, ki, kd);   // echo back
        return;
    }

    /* ---- SY kp ki kd : set Y axis PID params ----------------------- */
    if (cmd[0] == 'S' && cmd[1] == 'Y' && cmd[2] == ' ')
    {
        float kp, ki, kd;
        int parsed = sscanf(cmd + 3, "%f %f %f", &kp, &ki, &kd);
        if (parsed != 3)
        {
            proto_send_err("SY needs 3 floats");
            return;
        }
        y_set_pid_params(&kp, &ki, &kd);
        proto_send_ok();
        printf("<PIDY %.4f %.4f %.4f>\r\n", kp, ki, kd);   // echo back
        return;
    }

    /* ---- T xpos ypos : move + send data ---------------------------- */
    if (cmd[0] == 'T' && cmd[1] == ' ')
    {
        int32_t tx, ty;
        int parsed = sscanf(cmd + 2, "%ld %ld", &tx, &ty);
        if (parsed != 2)
        {
            proto_send_err("T needs 2 int32");
            return;
        }
        proto_send_ok();
        count_x = 0;
        count_y = 0;
        reach_pos(tx, ty);
        send_data(data_arr_x, data_arr_y, count_x, count_y);
        return;
    }

    if (strcmp(cmd, "D") == 0)
        {
    	    //proto_send_ok();
            send_data(data_arr_x, data_arr_y, count_x, count_y);
            return;
        }

    /* ---- R xpos ypos : move silently ------------------------------- */
    if (cmd[0] == 'R' && cmd[1] == ' ')
    {
        int32_t tx, ty;
        int parsed = sscanf(cmd + 2, "%ld %ld", &tx, &ty);
        if (parsed != 2)
        {
            proto_send_err("R needs 2 int32");
            return;
        }
        proto_send_ok();
        count_x = 0;
        count_y = 0;
        reach_pos(tx, ty);
        printf("<DONE>\r\n");
        return;
    }

    /* ---- Unknown --------------------------------------------------- */
    proto_send_err("unknown cmd");
}

/* ------------------------------------------------------------------ */
/*  Main receive loop — call from main loop                            */
/* ------------------------------------------------------------------ */

void protocol_run(void)
{
    static char    buf[CMD_BUF_SIZE];
    static uint8_t idx      = 0;
    static uint8_t in_frame = 0;

    int c = uart6_read();
    if (c < 0) return;

    char ch = (char)c;

    if (ch == '<')
    {
        in_frame = 1;
        idx      = 0;
        memset(buf, 0, CMD_BUF_SIZE);
        return;
    }

    if (ch == '>' && in_frame)
    {
        in_frame = 0;
        buf[idx] = '\0';
        handle_command(buf);
        return;
    }

    if (in_frame)
    {
        if (idx < CMD_BUF_SIZE - 1)
            buf[idx++] = ch;
        else
        {
            in_frame = 0;
            idx      = 0;
            proto_send_err("cmd too long");
        }
    }
}

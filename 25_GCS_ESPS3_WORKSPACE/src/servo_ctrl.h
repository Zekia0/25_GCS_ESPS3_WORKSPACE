#ifndef SERVO_CTRL_H
#define SERVO_CTRL_H

void classifier_pos(int pos_x,int pos_y,int class_ID);
float angle_calc(int pos_x,int pos_y);
float clockwise_pid_kp_change(int deta_angle);
void servo_init();

#endif
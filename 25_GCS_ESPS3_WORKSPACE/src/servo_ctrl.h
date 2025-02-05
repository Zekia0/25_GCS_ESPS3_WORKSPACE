#ifndef SERVO_CTRL_H
#define SERVO_CTRL_H

// void classifier_pos(int pos_x,int pos_y,int class_ID);
// float angle_calc(int pos_x,int pos_y);
// float clockwise_pid_kp_change(int deta_angle);
void servo_init();
int angle_cal_2(float pos_x,float pos_y);
int delta_angle(float pos_x,float pos_y,int tar_ID);
void servo_ctrl(int posx,int posy,int tar_ID);
float value_cal(int posx,int posy,int tar_ID);

#endif
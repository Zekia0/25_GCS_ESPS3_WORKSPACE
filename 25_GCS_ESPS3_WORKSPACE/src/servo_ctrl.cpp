#include <Arduino.h>
#include <ESP32Servo.h>
#include "Servo_ctrl.h"

#define PI 3.14
Servo CLASSIFIER;

/*****************************************初始化参数********************************************/
int pos_x_centre = 160;
int pos_y_centre = 120;

int angle_tar_recyclbled = 45;
int angle_tar_kitch = 135;
int angle_tar_waste = 225;
int angle_tar_else = 315;

int deta_angle;
int servo_speed_max_clockwise = 135; 
int servo_speed_stop = 107;
int clockwise_variation_range = abs(90-servo_speed_max_clockwise);
int clockwise_real_speed;
float clockwise_pid_kp_init = 0.2;
float clockwise_pid_kp = clockwise_pid_kp_init;
/*****************************************初始化参数********************************************/

/*****************************************转动盘舵机分类算法&&PID控制********************************************/
void classifier_pos(int pos_x,int pos_y,int class_ID){
    if(class_ID = 2/*可回收垃圾*/){
        deta_angle = abs(angle_tar_recyclbled - angle_calc(pos_x,pos_y));
        clockwise_real_speed = servo_speed_stop+deta_angle*clockwise_pid_kp_change(deta_angle);
        CLASSIFIER.write(clockwise_real_speed);
    }
    if(class_ID = 3/*厨余垃圾*/){
        deta_angle = abs(angle_tar_kitch - angle_calc(pos_x,pos_y));
        clockwise_real_speed = servo_speed_stop+deta_angle*clockwise_pid_kp_change(deta_angle);
        CLASSIFIER.write(clockwise_real_speed);
    }
    if(class_ID = 4/*有害垃圾*/){
        deta_angle = abs(angle_tar_waste - angle_calc(pos_x,pos_y));
        clockwise_real_speed = servo_speed_stop+deta_angle*clockwise_pid_kp_change(deta_angle);
        CLASSIFIER.write(clockwise_real_speed);
    }
    if(class_ID = 5/*其他垃圾*/){
        deta_angle = abs(angle_tar_else - angle_calc(pos_x,pos_y));
        clockwise_real_speed = servo_speed_stop+deta_angle*clockwise_pid_kp_change(deta_angle);
        CLASSIFIER.write(clockwise_real_speed);
    }
}

float clockwise_pid_kp_change(int deta_angle){
    if(deta_angle > 200 && deta_angle < 360){
        clockwise_pid_kp = clockwise_pid_kp_init + 0.1;
    }
    if(deta_angle > 100 && deta_angle < 200){
        clockwise_pid_kp = clockwise_pid_kp_init + 0.05;
    } 
    if(deta_angle < 100 && deta_angle > 0){
        clockwise_pid_kp = clockwise_pid_kp_init - 0.05;
    }
    return clockwise_pid_kp;
}

float angle_calc(int pos_x,int pos_y){
    float angle = 0;
    float distance = sqrt(pow(pos_x-pos_x_centre,2)+pow(pos_y-pos_y_centre,2));
    if(((pos_x-pos_x_centre >= 0)&&(pos_y-pos_y_centre < 0))||((pos_x-pos_x_centre <= 0)&&(pos_y-pos_y_centre < 0))){
        angle = acos((pos_x-pos_x_centre)/distance);
    }
    else if(((pos_x-pos_x_centre >= 0)&&(pos_y-pos_y_centre > 0))||((pos_x-pos_x_centre <= 0)&&(pos_y-pos_y_centre > 0))){
        angle = asin(-(pos_x-pos_x_centre)/distance);
    }
    return angle*180/PI;
}

void servo_init(){
    CLASSIFIER.attach(14);
}
/*****************************************转动盘舵机分类算法&&PID控制********************************************/
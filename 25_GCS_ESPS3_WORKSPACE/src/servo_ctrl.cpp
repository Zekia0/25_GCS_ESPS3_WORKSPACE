#include <Arduino.h>
#include <ESP32Servo.h>
#include "Servo_ctrl.h"

#define PI 3.14
Servo CLASSIFIER;

/*****************************************初始化参数********************************************/
int pos_x_centre = 320;
int pos_y_centre = 150;

int angle_tar_recyclbled = 25;
int angle_tar_kitch = 155;
int angle_tar_waste = -55;
int angle_tar_else = -125;
/*****************************************一代算法参数********************************************/
// int deta_angle;
// int servo_speed_max_clockwise = 135; 
// int servo_speed_stop = 90;
// int clockwise_variation_range = abs(90-servo_speed_max_clockwise);
// int clockwise_real_speed;
// float clockwise_pid_kp_init = 0.2;
// float clockwise_pid_kp = clockwise_pid_kp_init;
/*****************************************一代算法参数********************************************/
/*****************************************二代算法参数********************************************/

float k_p = 0.5;
int max_stop_value = 105;
int min_stop_value = 75;
int stop_value = 90;

/*****************************************二代算法参数********************************************/
/*****************************************初始化参数********************************************/
/*****************************************转动盘舵机分类算法&&PID控制第一代********************************************/
// void classifier_pos(int pos_x,int pos_y,int class_ID){
//     if(class_ID = 2/*可回收垃圾*/){
//         deta_angle = abs(angle_tar_recyclbled - angle_calc(pos_x,pos_y));
//         clockwise_real_speed = servo_speed_stop+deta_angle*clockwise_pid_kp_change(deta_angle);
//         CLASSIFIER.write(clockwise_real_speed);
//     }
//     if(class_ID = 3/*厨余垃圾*/){
//         deta_angle = abs(angle_tar_kitch - angle_calc(pos_x,pos_y));
//         clockwise_real_speed = servo_speed_stop+deta_angle*clockwise_pid_kp_change(deta_angle);
//         CLASSIFIER.write(clockwise_real_speed);
//     }
//     if(class_ID = 4/*有害垃圾*/){
//         deta_angle = abs(angle_tar_waste - angle_calc(pos_x,pos_y));
//         clockwise_real_speed = servo_speed_stop+deta_angle*clockwise_pid_kp_change(deta_angle);
//         CLASSIFIER.write(clockwise_real_speed);
//     }
//     if(class_ID = 5/*其他垃圾*/){
//         deta_angle = abs(angle_tar_else - angle_calc(pos_x,pos_y));
//         clockwise_real_speed = servo_speed_stop+deta_angle*clockwise_pid_kp_change(deta_angle);
//         CLASSIFIER.write(clockwise_real_speed);
//     }
// }

// float clockwise_pid_kp_change(int deta_angle){
//     if(deta_angle > 200 && deta_angle < 360){
//         clockwise_pid_kp = clockwise_pid_kp_init + 0.1;
//     }
//     if(deta_angle > 100 && deta_angle < 200){
//         clockwise_pid_kp = clockwise_pid_kp_init + 0.05;
//     } 
//     if(deta_angle < 100 && deta_angle > 0){
//         clockwise_pid_kp = clockwise_pid_kp_init - 0.05;
//     }
//     return clockwise_pid_kp;
// }

// float angle_calc(int pos_x,int pos_y){
//     float angle = 0;
//     float distance = sqrt(pow(pos_x-pos_x_centre,2)+pow(pos_y-pos_y_centre,2));
//     if(((pos_x-pos_x_centre >= 0)&&(pos_y-pos_y_centre < 0))||((pos_x-pos_x_centre <= 0)&&(pos_y-pos_y_centre < 0))){
//         angle = acos((pos_x-pos_x_centre)/distance);
//     }
//     else if(((pos_x-pos_x_centre >= 0)&&(pos_y-pos_y_centre > 0))||((pos_x-pos_x_centre <= 0)&&(pos_y-pos_y_centre > 0))){
//         angle = asin(-(pos_x-pos_x_centre)/distance);
//     }
//     return angle*180/PI;
// }


void servo_init(){
    CLASSIFIER.attach(14);
}
/*****************************************转动盘舵机分类算法&&PID控制第一代********************************************/
/*****************************************舵机算法第二代********************************************/
int angle_cal_2(float pos_x,float pos_y){
    int delta_x = pos_x - pos_x_centre;
    int delta_y = - pos_y + pos_y_centre;
    float angle = atan2(delta_y,delta_x)*180/PI;
    return angle;
}

int delta_angle(float pos_x,float pos_y,int tar_ID){
    if (tar_ID == 2){
        return angle_cal_2(pos_x,pos_y)-angle_tar_recyclbled;
    }
    if (tar_ID == 3)
    {
        return angle_cal_2(pos_x,pos_y)-angle_tar_kitch;
    }
    if (tar_ID == 4)
    {
        return angle_cal_2(pos_x,pos_y)-angle_tar_waste;
    }
    if (tar_ID == 5)
    {
        return angle_cal_2(pos_x,pos_y)-angle_tar_else;
    }  
}

float value_cal(int posx,int posy,int tar_ID){
    if(delta_angle(posx,posy,tar_ID) > 5){
        return max_stop_value+abs(delta_angle(posx,posy,tar_ID)*k_p);
    }
    else if(delta_angle(posx,posy,tar_ID) < -5){
        return min_stop_value-abs(delta_angle(posx,posy,tar_ID)*k_p);
    }
    else{
        return stop_value;
    }
}

void servo_ctrl(int posx,int posy,int tar_ID){
    CLASSIFIER.write(value_cal(posx,posy,tar_ID));
}
/*****************************************舵机算法第二代********************************************/

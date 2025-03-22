#include <Arduino.h>
#include <ESP32Servo.h>
#include "Servo_ctrl.h"

#define PI 3.14
#define CLASSIFIER_PIN 14

#define recycled_pin 37
#define kitch_pin 38
#define waste_pin 39
#define else_pin 40

Servo CLASSIFIER;
Servo RECYCLED;
Servo KITCH;
Servo WASTE;
Servo ELSE;
/*****************************************初始化参数********************************************/
int pos_x_centre = 305;
int pos_y_centre = 148;

int angle_tar_recyclbled = -35;
int angle_tar_kitch = -5;
int angle_tar_waste = -140;
int angle_tar_else = -175;

float k_p = 0.5;
int max_stop_value = 105;
int min_stop_value = 75;

void F_init(void){
    pinMode(4, OUTPUT);
    digitalWrite(4, LOW);
}

void F_down(void){
    digitalWrite(4, HIGH);
}

void F_up(void){
    digitalWrite(4, LOW);
}

void servo_init(){
    CLASSIFIER.attach(CLASSIFIER_PIN);
    RECYCLED.attach(recycled_pin);
    KITCH.attach(kitch_pin);
    WASTE.attach(waste_pin);
    ELSE.attach(else_pin);
}

void cleaner_done(uint16_t class_ID){
    if(class_ID == 2){
        RECYCLED.write(180);
        delay(5000);
        RECYCLED.write(90);
    }
    if(class_ID == 3){
        KITCH.write(0);
        delay(5000);
        KITCH.write(90);
    }
    if(class_ID == 4){
        WASTE.write(0);
        delay(5000);
        WASTE.write(90);
    }
    if(class_ID == 5){
        ELSE.write(180);
        delay(5000);
        ELSE.write(90); 
    } 
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
        return 90;
    }
}

bool servo_task_done(int posx,int posy,int tar_ID){
    if(abs(delta_angle(posx,posy,tar_ID))<5){
        return true;
    }else{
        return false;
    }
}

void servo_ctrl(int posx,int posy,int tar_ID){
    CLASSIFIER.write(value_cal(posx,posy,tar_ID));
}

void classifier_stop(void){
    CLASSIFIER.write(90);
}
/*****************************************舵机算法第二代********************************************/


/*****************************************分类舵机驱动**********************************************/
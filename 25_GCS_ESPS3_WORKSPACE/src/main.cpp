#include <Arduino.h>
#include <ESP32Servo.h>
#include <servo_ctrl.h>
#include <serial.h>

void setup(){
    servo_init();
    serial_init();
}

void loop(){
    serial_read_data();
    classifier_pos(pos_x,pos_y,class_ID);
}
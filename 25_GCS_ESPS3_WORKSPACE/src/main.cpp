#include <Arduino.h>
#include <servo_ctrl.h>
#include <serial.h>

void setup(){
    servo_init();
    serial_init();
}

void loop(){
    serial_read_data();
    servo_ctrl(pos_x,pos_y,class_ID);
    print_system_info();
}
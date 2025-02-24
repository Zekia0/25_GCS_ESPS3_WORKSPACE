#include <Arduino.h>
#include <servo_ctrl.h>
#include <serial.h>
#include <stepper.h>

int flag = 0;

void setup(){
    servo_init();
    serial_init();
    init_stepMottor();
    motor_direction(true);
}

void loop(){
    // serial_read_data();
    // servo_ctrl(pos_x,pos_y,class_ID);
    // print_system_info();
    while(flag == 0){
        motor_down();
        flag = 1;
    }
}
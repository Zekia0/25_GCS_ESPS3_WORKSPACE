#include <Arduino.h>
#include <servo_ctrl.h>
#include <serial.h>
#include <stepper.h>
#include <microswitch.h>

int flag = 0;
int task_state = 0;
void setup(){
    servo_init();
    serial_init();
    trigger_init();
    stepMottor_init();//！！！加了trigger_switch(),没有安装限位开关之前别下程序
    motor_direction(false);
}

void loop(){
    if(class_ID==0){
        classifier_stop();
    }
    serial_read_data();
    // servo_ctrl(pos_x,pos_y,class_ID);
    print_system_info();
    while(class_ID != 0){
        if(task_state == 0){
            serial_read_data();
            print_system_info();
            servo_ctrl(pos_x,pos_y,class_ID);
            if(servo_task_done(pos_x,pos_y,class_ID) && (class_ID != 0)){
                task_state = 1;
            }
        }
        if(task_state == 1){
            delay(1000);
            motor_down();
            delay(1000);
            cleaner_done(class_ID);
            delay(1000);
            motor_up();
            delay(1000);
            task_state = 0;
            class_ID = 0;
            break;
        }
        
    }
}
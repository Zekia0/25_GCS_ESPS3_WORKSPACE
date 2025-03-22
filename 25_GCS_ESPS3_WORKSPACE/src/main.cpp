#include <Arduino.h>
#include <servo_ctrl.h>
#include <serial.h>
#include <stepper.h>
#include <microswitch.h>
#include <serial_screen.h>
#include <red_light.h>

int flag = 0;
int find_trush_and_show_flag = 0;
int task_state = 0;

void setup(){
    servo_init();
    red_light_init();
    serial_init();
    screen_init();
    F_init();
    trigger_init();
    stepMottor_init();//！！！加了trigger_switch(),没有安装限位开关之前别下
}

void loop(){
    //没有检测到物体，进入静默状态
        if(class_ID==6 || class_ID==0){
            motor_stop();
            classifier_stop();
            serial_read_data();
            call_work();
        }

        //检测到物体，进入工作状态
        //核心转盘机构转动
        else if(class_ID == 2 || class_ID == 3 || class_ID == 4 || class_ID == 5){
            //停止播放视频
            while(flag == 0){
                stop_video();
                flag = 1;
            }
            //显示一次识别结果
            while(find_trush_and_show_flag == 0){
                screen_main_info_update(class_ID);
                find_trush_and_show_flag = 1;
            }
            if(task_state == 0){
                serial_read_data();
            servo_ctrl(pos_x,pos_y,class_ID);
            if(servo_task_done(pos_x,pos_y,class_ID) && (class_ID != 0)){
                task_state = 1;
            }
        }
        //下压机构工作
        if(task_state == 1){
            //更新识别事件状态
            find_trush_and_show_flag = 0;
            call_rest();
            F_down();
            delay(2000);
            motor_down();
            delay(1000);
            cleaner_done(class_ID);
            delay(1000);
            motor_up();
            delay(1000);
            screen_num_change(class_ID);
            F_up();
            delay(2000);
            task_state = 2;
        }
        //反应阶段，反应垃圾是否被清理，并进入第二轮识别分类
        if(task_state == 2){
            motor_stop();
            classifier_stop();
            for(int i = 0;i < 500;i++){
                serial_read_data();
            }   
            task_state = 0;
        }    
    }
    //满载循环扫描检测
    full_warning();
}
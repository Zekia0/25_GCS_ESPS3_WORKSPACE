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
int last_processed_class_ID = 0;  // 记录上次处理的垃圾ID
int last_processed_pos_x = 0;      // 记录上次处理的垃圾X坐标
int last_processed_pos_y = 0;      // 记录上次处理的垃圾Y坐标
#define POSITION_THRESHOLD 30      // 位置变化阈值，超过此值认为是新垃圾

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
    // if(flag==0){motor_down();flag=1;}

    //没有检测到物体，进入静默状态
        if(class_ID==6 || class_ID==0){
            motor_stop();
            classifier_stop();
            serial_read_data();
            call_work();
            // 重置已处理的垃圾ID和位置，允许重新识别
            last_processed_class_ID = 0;
            last_processed_pos_x = 0;
            last_processed_pos_y = 0;
            task_state = 0;
            flag = 0;
            find_trush_and_show_flag = 0;
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
            // 计算位置变化距离的平方（避免使用sqrt，提高效率）
            int pos_delta_x = abs(pos_x - last_processed_pos_x);
            int pos_delta_y = abs(pos_y - last_processed_pos_y);
            int pos_distance_sq = pos_delta_x * pos_delta_x + pos_delta_y * pos_delta_y;
            int threshold_sq = POSITION_THRESHOLD * POSITION_THRESHOLD;
            
            // 判断是否是新垃圾：分类完成 && 不是上次已处理的垃圾（位置或类型不同）
            bool is_new_trash = (class_ID != last_processed_class_ID) || 
                               (pos_distance_sq > threshold_sq);
            
            if(servo_task_done(pos_x,pos_y,class_ID) && (class_ID != 0) && is_new_trash){
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
            // 增加延迟，确保F装置完全上升到顶
            delay(3000);  // 从2000ms增加到3000ms，确保F装置完全上升
            // 记录已处理的垃圾ID和位置信息
            last_processed_class_ID = class_ID;
            last_processed_pos_x = pos_x;
            last_processed_pos_y = pos_y;
            task_state = 2;
        }
        //反应阶段，反应垃圾是否被清理，并进入第二轮识别分类
        if(task_state == 2){
            motor_stop();
            classifier_stop();
            // 等待上位机识别到垃圾已被清理（class_ID变为0或6），或者等待足够长时间
            int wait_count = 0;
            while(wait_count < 1000){
                serial_read_data();
                // 如果识别到垃圾已被清理（class_ID变为0或6），可以提前退出
                if(class_ID == 0 || class_ID == 6){
                    delay(100);  // 再等待一小段时间确保稳定
                    break;
                }
                wait_count++;
                delay(1);  // 每次循环延迟1ms，总共最多等待1秒
            }
            // 重置状态，准备处理下一个垃圾
            // 注意：这里不重置last_processed_class_ID和位置，因为需要防止重复处理
            // 只有当上位机识别到垃圾已被清理（class_ID变为0或6）时才会重置
            task_state = 0;
            flag = 0;  // 重置flag，允许下次识别时重新显示
        }    
    }
    //满载循环扫描检测
    full_warning();
}
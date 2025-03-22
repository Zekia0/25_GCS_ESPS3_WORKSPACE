#include <Arduino.h>
#include "serial_screen.h"

int video_flag = 0; 

uint8_t Order = 0;

uint8_t recyc_done_num = 0;
uint8_t kitch_done_num = 0;
uint8_t waste_done_num = 0;
uint8_t else_done_num = 0;

uint8_t Null = 0x00;

byte recyc_add[2] = {0x03, 0x00};
byte kitch_add[2] = {0x03, 0x01};
byte waste_add[2] = {0x03, 0x02};
byte else_add[2] = {0x03, 0x03};
byte Order_add[2] = {0x04, 0x00};
byte Num_add[2] = {0x04, 0x01};

byte num_change_frame_header[4] = {0xA5, 0x5A, 0x05, 0x82};
byte text_frame_header[6] = {0xA5, 0x5A, 0x0B, 0x82, 0x02, 0x00};

byte text_recyc[8] = {0xBF, 0xC9, 0xBB, 0xD8, 0xCA, 0xD5, 0xCE, 0xEF};
byte text_kitch[8] = {0xB3, 0xF8, 0xD3, 0xE0, 0xC0, 0xAC, 0xBB, 0xF8};
byte text_waste[8] = {0xD3, 0xD0, 0xBA, 0xA6, 0xC0, 0xAC, 0xBB, 0xF8};
byte text_else[8] = {0xC6, 0xE4, 0xCB, 0xFB, 0xC0, 0xAC, 0xBB, 0xF8};

byte classify_successfully[14] = {0xA5, 0x5A, 0x0B, 0x82, 0x06, 0x00, 0xB7, 0xD6, 0xC0, 0xE0, 0xB3, 0xC9, 0xB9, 0xA6}; 

byte music[7] = {0xA5, 0x5A, 0x04, 0x80, 0x68, 0x5A, 0x60};
byte cycle[13] = {0xA5, 0x5A, 0x0A, 0x80, 0x60, 0x5A, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
byte play[6] = {0xA5, 0x5A, 0x03, 0x80, 0x6A, 0x5A};
byte stop[6] = {0xA5, 0x5A, 0x03, 0x80, 0x6B, 0x5A};

void screen_init(void){
    Serial0.begin(115200);
    for(int i = 0; i < 7; i++){
        Serial0.write(music[i]);
    }
    for(int i = 0; i < 13; i++){
        Serial0.write(cycle[i]);
    }
    for(int i = 0; i < 6; i++){
        Serial0.write(play[i]);
    }
    //视频初始化

    //所有类别垃圾数量清零
    for(int i = 0; i < 4; i++){
        Serial0.write(num_change_frame_header[i]);
    }
    for(int i = 0; i < 2; i++){
        Serial0.write(recyc_add[i]);
    }
    Serial0.write(Null);
    Serial0.write(Null);
    for(int i = 0; i < 4; i++){
        Serial0.write(num_change_frame_header[i]);
    }
    for(int i = 0; i < 2; i++){
        Serial0.write(kitch_add[i]);
    }
    Serial0.write(Null);
    Serial0.write(Null);
    for(int i = 0; i < 4; i++){
        Serial0.write(num_change_frame_header[i]);
    }
    for(int i = 0; i < 2; i++){
        Serial0.write(waste_add[i]);
    }
    Serial0.write(Null);
    Serial0.write(Null);
    for(int i = 0; i < 4; i++){
        Serial0.write(num_change_frame_header[i]);
    }
    for(int i = 0; i < 2; i++){
        Serial0.write(else_add[i]);
    }
    Serial0.write(Null);
    Serial0.write(Null);
    //所有类别垃圾数量清零
}

void stop_video(void){
    while (video_flag == 0)
    {
        for(int i = 0; i < 6; i++){
            Serial0.write(stop[i]);
        }
        video_flag = 1;
    }
    
}

void screen_num_change(int Cl){
    switch (Cl)
    {
        case 2:
            recyc_done_num++;
            for(int i = 0; i < 4; i++){
                Serial0.write(num_change_frame_header[i]);
            }
            for(int i = 0; i < 2; i++){
                Serial0.write(recyc_add[i]);
            }
            Serial0.write(Null);
            Serial0.write(recyc_done_num);
            break;
        case 3:
            kitch_done_num++;
            for(int i = 0; i < 4; i++){
                Serial0.write(num_change_frame_header[i]);
            }
            for(int i = 0; i < 2; i++){
                Serial0.write(kitch_add[i]);
            }
            Serial0.write(Null);
            Serial0.write(kitch_done_num);
            break;
        case 4:
            waste_done_num++;
            for(int i = 0; i < 4; i++){
                Serial0.write(num_change_frame_header[i]);
            }
            for(int i = 0; i < 2; i++){
                Serial0.write(waste_add[i]);
            }
            Serial0.write(Null);
            Serial0.write(waste_done_num);
            break;
        case 5:
            else_done_num++;
            for(int i = 0; i < 4; i++){
                Serial0.write(num_change_frame_header[i]);
            }
            for(int i = 0; i < 2; i++){
                Serial0.write(else_add[i]);
            }
            Serial0.write(Null);
            Serial0.write(else_done_num);
            break;
        default:
            break;
    }
}

void screen_main_info_update(int cl){
    Order++;
    switch (cl){
        case 2:
            for(int i = 0; i < 4; i++){
                Serial0.write(num_change_frame_header[i]);
            }
            for(int i = 0; i < 2; i++){
                Serial0.write(Order_add[i]);
            }
            Serial0.write(Null);
            Serial0.write(Order);

            for(int i = 0; i < 6; i++){
                Serial0.write(text_frame_header[i]);
            }
            for(int i = 0; i < 8; i++){
                Serial0.write(text_recyc[i]);
            }
            //数量
            for(int i = 0; i < 4; i++){
                Serial0.write(num_change_frame_header[i]);
            }
            for(int i = 0; i < 2; i++){
                Serial0.write(Num_add[i]);
            }
            Serial0.write(Null);
            Serial0.write(0x01);
            break;
        case 3:
            for(int i = 0; i < 4; i++){
                Serial0.write(num_change_frame_header[i]);
            }
            for(int i = 0; i < 2; i++){
                Serial0.write(Order_add[i]);
            }
            Serial0.write(Null);
            Serial0.write(Order);
            
            for(int i = 0; i < 6; i++){
                Serial0.write(text_frame_header[i]);
            }
            for(int i = 0; i < 8; i++){
                Serial0.write(text_kitch[i]);
            }

            for(int i = 0; i < 4; i++){
                Serial0.write(num_change_frame_header[i]);
            }
            for(int i = 0; i < 2; i++){
                Serial0.write(Num_add[i]);
            }
            Serial0.write(Null);
            Serial0.write(0x01);
            break;
        case 4:
            for(int i = 0; i < 4; i++){
                Serial0.write(num_change_frame_header[i]);
            }
            for(int i = 0; i < 2; i++){
                Serial0.write(Order_add[i]);
            }
            Serial0.write(Null);
            Serial0.write(Order);

            for(int i = 0; i < 6; i++){
                Serial0.write(text_frame_header[i]);
            }
            for(int i = 0; i < 8; i++){
                Serial0.write(text_waste[i]);
            }
            for(int i = 0; i < 4; i++){
                Serial0.write(num_change_frame_header[i]);
            }
            for(int i = 0; i < 2; i++){
                Serial0.write(Num_add[i]);
            }
            Serial0.write(Null);
            Serial0.write(0x01);
            break;
        case 5:
            for(int i = 0; i < 4; i++){
                Serial0.write(num_change_frame_header[i]);
            }
            for(int i = 0; i < 2; i++){
                Serial0.write(Order_add[i]);
            }
            Serial0.write(Null);
            Serial0.write(Order);
            for(int i = 0; i < 6; i++){
                Serial0.write(text_frame_header[i]);
            }
            for(int i = 0; i < 8; i++){
                Serial0.write(text_else[i]);
            }
            for(int i = 0; i < 4; i++){
                Serial0.write(num_change_frame_header[i]);
            }
            for(int i = 0; i < 2; i++){
                Serial0.write(Num_add[i]);
            }
            Serial0.write(Null);
            Serial0.write(0x01);
            break;
    }
    for(int i = 0; i < 14; i++){
        Serial0.write(classify_successfully[i]);
    }
}

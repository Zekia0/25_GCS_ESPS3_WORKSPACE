#include <Arduino.h>

#define recyc 1
#define kitch 2
#define waste 42
#define else  41

byte back_info[7] = {0xA5, 0x5A, 0x04, 0x80, 0x03, 0x00, 0x00};
byte recyc_full[7] = {0xA5, 0x5A, 0x04, 0x80, 0x03, 0x00, 0x04};
byte kitch_full[7] = {0xA5, 0x5A, 0x04, 0x80, 0x03, 0x00, 0x01};
byte waste_full[7] = {0xA5, 0x5A, 0x04, 0x80, 0x03, 0x00, 0x03};
byte else_full[7] = {0xA5, 0x5A, 0x04, 0x80, 0x03, 0x00, 0x02};

void red_light_init(void){
    pinMode(recyc, INPUT_PULLDOWN);
    pinMode(kitch, INPUT_PULLDOWN);
    pinMode(waste, INPUT_PULLDOWN);
    pinMode(else, INPUT_PULLDOWN);
}

void full_warning(void){
    if(digitalRead(recyc)==HIGH){
        delay(1500);
        if(digitalRead(recyc)==HIGH){
            for(int i = 0; i < 7; i++){
                Serial0.write(recyc_full[i]);
            }
            delay(3000);
            for(int i = 0; i < 7; i++){
                Serial0.write(back_info[i]);
            }
        }
    }
    if(digitalRead(kitch)==HIGH){
        delay(1500);
        if(digitalRead(kitch)==HIGH){
            for(int i = 0; i < 7; i++){
                Serial0.write(waste_full[i]);
            }
            delay(3000);
            for(int i = 0; i < 7; i++){
                Serial0.write(back_info[i]);
            }
        }
    }
    if(digitalRead(waste)==HIGH){
        delay(1500);
        if(digitalRead(waste)==HIGH){
            for(int i = 0; i < 7; i++){
                Serial0.write(kitch_full[i]);
            }
            delay(3000);
            for(int i = 0; i < 7; i++){
                Serial0.write(back_info[i]);
            }
        }
    }
    if(digitalRead(else)==HIGH){
        delay(1500);
        if(digitalRead(else)==HIGH){
            for(int i = 0; i < 7; i++){
                Serial0.write(else_full[i]);
            }
            delay(3000);
            for(int i = 0; i < 7; i++){
                Serial0.write(back_info[i]);
            }
        }
    }
}
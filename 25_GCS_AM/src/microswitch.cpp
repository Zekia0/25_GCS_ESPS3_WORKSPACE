#include <Arduino.h>

#define trigger_pin 12

void trigger_init(void){
    pinMode(trigger_pin,INPUT_PULLUP);
}

bool trigger_switch(void){
    if(digitalRead(trigger_pin)==LOW){
        return true;
    }else{
        return false;
    }
}
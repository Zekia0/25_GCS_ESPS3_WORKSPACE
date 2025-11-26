#include "Arduino.h"
#include "stepper.h"
#include "microswitch.h"

#define pul_pin 35
#define dir_pin 36
#define pul_delay 50
#define cirlcle 12.5
#define step_times 6400*cirlcle

void motor_direction(bool dir){
    digitalWrite(dir_pin,dir);
}

void stepMottor_init(void){
    pinMode(pul_pin,OUTPUT);
    pinMode(dir_pin,OUTPUT);
    motor_up();
}

// void step_2(void){
//     digitalWrite(pul_pin,HIGH);
//     delayMicroseconds(pul_delay);
//     digitalWrite(pul_pin,LOW);
//     delayMicroseconds(pul_delay);
//     digitalWrite(pul_pin,HIGH);
//     delayMicroseconds(pul_delay);
//     digitalWrite(pul_pin,LOW);
//     delayMicroseconds(pul_delay);
// }

void step(void){
    digitalWrite(pul_pin,HIGH);
    delayMicroseconds(pul_delay);
    digitalWrite(pul_pin,LOW);
    delayMicroseconds(pul_delay);
}

void motor_down(void){
    motor_direction(false);
    for(int i = 0; i < step_times; i++){
      step();
    }
}

void motor_up(void){
    while(!trigger_switch()){
        if(trigger_switch()){
            motor_stop();
            break;
        }
        motor_direction(true);
        step();
    }
}

void motor_stop(void){
    digitalWrite(pul_pin,LOW);
}
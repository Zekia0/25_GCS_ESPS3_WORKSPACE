#include "Arduino.h"
#include "stepper.h"

#define pul_pin 35
#define dir_pin 36
#define pul_delay 200
#define step_distance 120

void motor_direction(bool dir){
    digitalWrite(dir_pin,dir);
}

void init_stepMottor(void){
    pinMode(pul_pin,OUTPUT);
    pinMode(dir_pin,OUTPUT);
}

void step(void){
    digitalWrite(pul_pin,HIGH);
    delayMicroseconds(pul_delay);
    digitalWrite(pul_pin,LOW);
    delayMicroseconds(pul_delay);
}

void rotateCircles(int num){
    for(int c = 0; c < num; c++){
        for(int i = 0; i < 200; i++){
            step();
        }
    } 
}

void motor_up(void){
    motor_direction(false);
    rotateCircles(step_distance);
}

void motor_down(void){
    motor_direction(true);
    rotateCircles(step_distance); 
}

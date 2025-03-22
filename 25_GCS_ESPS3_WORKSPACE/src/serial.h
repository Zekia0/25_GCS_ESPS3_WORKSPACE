#ifndef SERIAL_H
#define SERIAL_H

#include <Arduino.h>

extern uint16_t class_ID, pos_x, pos_y ;
void serial_init(void);
void serial_read_data(void);
void print_system_info(void);
void call_rest(void);
void call_work(void);

#endif
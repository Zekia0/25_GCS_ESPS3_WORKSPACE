#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import serial

def int_to_bytes(value, length, byte_order='big'):
    return value.to_bytes(length, byte_order)

def sender(posx, posy, class_type):
    data_x = int_to_bytes(posx, 2, 'big')
    data_y = int_to_bytes(posy, 2, 'big')
    class_id = int_to_bytes(class_type, 1, 'little')
	
    frame_head = b'\x21\x2c'
    frame_foot = b'\x5b'

    data_pack = frame_head + class_id + data_x + data_y + frame_foot

    serial_port = '/dev/ttyS0' 
    baud_rate = 115200 

    with serial.Serial(serial_port, baud_rate) as ser:
        ser.write(data_pack) 

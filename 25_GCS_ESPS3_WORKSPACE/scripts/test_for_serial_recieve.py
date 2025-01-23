#备注：验证下位机和香橙派通信脚本
#!/usr/bin/env python3
import sys
sys.path.append('C:/Users/Zekia/AppData/Roaming/Python/Python38/site-packages')
import serial

def read_and_extract_data(serial_port):
    # 打开串口
    with serial.Serial(serial_port, 9600, timeout=1) as ser:
        while True:
            # 读取8个字节
            data = ser.read(8)
            if len(data) == 8:
                # 检查帧头和帧尾
                if data[0] == '!' and data[1] == ',' and data[7] == '[':
                    # 提取中间五个字节的数据
                    data_frame = data[2:6]
                    print("提取的数据帧:", data_frame)
            else:
                print("未读取到完整的数据帧")

def output_data(serial_port):
    with serial.Serial(serial_port, 9600) as ser:
        while True:
            # 读取8个字节
            data = ser.read(8)
            data_int = int.from_bytes(data, byteorder='big')
            data_hex = hex(data_int)
            print("提取的数据帧:", data_hex)

# 使用串口12
serial_port = 'COM12'  # 在Windows系统中，串口通常表示为'COMx'
read_and_extract_data(serial_port)




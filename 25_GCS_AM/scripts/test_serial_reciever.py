#备注：验证数据包采样脚本
import sys
sys.path.append('C:/Users/Zekia/AppData/Roaming/Python/Python38/site-packages')
import serial
import time as t

serial_port = 'COM12' 
serial_baud = 9600

ser = serial.Serial(serial_port, serial_baud)

ser.flush()

i = 0

def parse_data_packet(packet):
    # 检查数据包长度和帧头帧尾
    if packet.startswith(b'!,') and packet.endswith(b'['):
        # 提取数据位
        data_bytes = packet[2:7]

        # 存储第五个变量作为分类的ID
        class_ID = int.from_bytes(data_bytes[5:6], byteorder='big')

        # 拼接第四位和第五位作为Pos_X，第六位和第七位作为Pos_Y
        pos_x = int.from_bytes(data_bytes[1:3], byteorder='big')
        pos_y = int.from_bytes(data_bytes[3:5], byteorder='big')

        return class_ID, pos_x, pos_y
    else:
        return None, None, None

if __name__ == '__main__':
    while True:
        data = ser.read(8)
        print(f"原始数据：{data}\n" )
        class_ID, pos_x, pos_y = parse_data_packet(data)
        print(f"Class ID:{class_ID}\n" )
        print(f"Pos_X:{pos_x}\n" )
        print(f"Pos_Y:{pos_y}\n" )
        print(f"第{i}次循环\n" )
        i = i + 1
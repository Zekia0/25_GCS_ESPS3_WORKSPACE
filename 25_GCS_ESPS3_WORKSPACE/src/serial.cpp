#include <Arduino.h>
#include <serial.h>
#include <servo_ctrl.h>

uint8_t buffer[8];
int bufferIndex = 0;

int print_times=1;
uint8_t frame_header_1 = 0x21;
uint8_t frame_header_2 = 0x2c;
uint8_t frame_foot = 0x5b;

uint16_t class_ID, pos_x, pos_y;

void serial_init(void) {
  Serial0.begin(115200);
  Serial2.begin(115200);
}

void serial_read_data(void){
    if (Serial2.available() > 0) {
        uint8_t byte = Serial2.read();
        // Serial0.print("Received byte: ");

        if (byte == frame_header_1 && bufferIndex == 0) {
            buffer[bufferIndex++] = byte; 
        }else if(bufferIndex > 0){
            buffer[bufferIndex++] = byte;
            // Serial0.println(buffer[bufferIndex-1],HEX);
            // Serial0.println(bufferIndex);
            if(bufferIndex == 8){
                if(buffer[0] == frame_header_1 && buffer[1] == frame_header_2 && buffer[7] == frame_foot){
                    class_ID = buffer[2];
                    pos_x = (buffer[3] << 8) | buffer[4];
                    pos_y = (buffer[5] << 8) | buffer[6];
                    // Serial0.println(class_ID);
                    // Serial0.println(pos_x);
                    // Serial0.println(pos_y);
                }
                bufferIndex = 0;
            }
        }
    }
}

void print_system_info(void){
    Serial0.print(print_times);
    Serial0.print(".");
    Serial0.print("pos_x:");
    Serial0.print(pos_x);
    Serial0.print(" pos_y:");
    Serial0.print(pos_y);
    Serial0.print(" class_ID:");
    Serial0.print(class_ID);
    Serial0.print(" value:");
    Serial0.print(value_cal(pos_x, pos_y,class_ID));
    Serial0.print(" delta_angle:");
    Serial0.println(delta_angle(pos_x, pos_y,class_ID));
    print_times++;
}

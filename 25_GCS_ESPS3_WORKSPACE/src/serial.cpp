#include <Arduino.h>
#include <serial.h>

uint8_t buffer[8];
int bufferIndex = 0;

uint8_t frame_header_1 = 0x21;
uint8_t frame_header_2 = 0x2c;
uint8_t frame_foot = 0x5b;

uint16_t class_ID, pos_x, pos_y;

void serial_init(void) {
  Serial0.begin(9600);
}

void serial_read_data(void){
    if (Serial0.available() > 0) {
        uint8_t byte = Serial0.read();
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
                    Serial0.println(class_ID);
                    Serial0.println(pos_x);
                    Serial0.println(pos_y);
                }
                bufferIndex = 0;
            }
        }
    }
}
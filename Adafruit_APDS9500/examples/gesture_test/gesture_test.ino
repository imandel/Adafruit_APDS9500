#include <SPI.h>
#include <Wire.h>
#include <Adafruit_APDS9500.h>

#define reg_bank1   0x72
#define reg_bank1_spi_out  0x7E

Adafruit_APDS9500 apds;


int frameCount = 0;
const int numFrames = 3;
uint8_t frames[numFrames][30][30];
const int cs = 15;

void setup(void) {
  Serial.begin(115200);
  while (!Serial) delay(10);     // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit APDS9500 test!");

  // Try to initialize!
  if (! apds.begin()) {
    Serial.println("Failed to find APDS9500 chip");
    while (1) { delay(10); }
  }
  Serial.println("APDS9500 Found!");
  Serial.println("");
  analogWrite(23, 24000000);
  //enable SPI by writing 0x01 to reg bank 1 
  apds_write_reg(reg_bank1, 0x01);
  apds_write_reg(reg_bank1_spi_out, 0x01);
  
}
void apds_write_reg(byte reg_addr, byte data) {
    apds_com_begin();

  //send adress of the register, with MSBit = 1 to indicate it's a write
  SPI.transfer(reg_addr | 0x80 );
  //sent data
  SPI.transfer(data);

  delayMicroseconds(20); // tSCLK-NCS for write operation
  apds_com_end();
  delayMicroseconds(100); // tSWW/tSWR (=120us) minus tSCLK-NCS. Could be shortened, but is looks like a safe lower bound

}
byte apds_read_reg(byte reg_addr) {
  apds_com_begin();

  // send adress of the register, with MSBit = 0 to indicate it's a read
  SPI.transfer(reg_addr & 0x7f );
  delayMicroseconds(100); // tSRAD
  // read data
  byte data = SPI.transfer(0);

  delayMicroseconds(1); // tSCLK-NCS for read operation is 120ns
  apds_com_end();
  delayMicroseconds(19); //  tSRW/tSRR (=20us) minus tSCLK-NCS

  return data;
}

void apds_com_begin() {
  digitalWrite(cs, LOW);
}

void apds_com_end() {
  digitalWrite(cs, HIGH);
}
void loop() {


//   uint16_t gesture_flags = apds.getDetectedGestures();
//   if(gesture_flags == -1){
//     Serial.println("Issue reading gesture state");
//   }
//
//   if(gesture_flags & 0x01) Serial.println("UP event detected");
//   if(gesture_flags & 0x02) Serial.println("DOWN event detected");
//   if(gesture_flags & 0x04) Serial.println("LEFT event detected");
//   if(gesture_flags & 0x08) Serial.println("RIGHT event detected");
//   if(gesture_flags & 0x10) Serial.println("FORWARD event detected");
//   if(gesture_flags & 0x20) Serial.println("BACKWARD event detected");
//   if(gesture_flags & 0x40) Serial.println("CLOCKWISE event detected");
//   if(gesture_flags & 0x80) Serial.println("COUNTERCLOCKWISE event detected");
//
//   if((gesture_flags >> 8) & 0x01) Serial.println("WAVE event detected");
//   if((gesture_flags >> 8) & 0x02) Serial.println("PROXIMITY event detected");
//   if((gesture_flags >> 8) & 0x04) Serial.println("HAS OBJECT event detected");
//   if((gesture_flags >> 8) & 0x08) Serial.println("WAKE UP TRIGGER event detected");
//   if((gesture_flags >> 8) & 0x80) Serial.println("NO OBJECT event detected");

  delay(100);
}

void updateFrame() {
  for (int i = 0; i < numFrames; i++) {
    //trigger the capture
//    adns_write_reg(REG_Frame_Capture, 0x93);
//    adns_write_reg(REG_Frame_Capture, 0xc5);

    //wait for at least two frames
    delayMicroseconds(1000);

    //check bit zero of motion register to see if new frame is ready
//    byte checkFrame = bitRead(adns_read_reg(REG_Motion), 0);
//    while(checkFrame != 1){
//      checkFrame = bitRead(adns_read_reg(REG_Motion), 0);
//    }
  
      //if (checkFrame == 1) {
        for (int j = 0; j < 30; j++) {
          for (int k = 0; k < 30; k++) {
            //read individual pixels
//            frames[i][j][k] = (uint8_t)adns_read_reg(REG_Pixel_Burst);
      //    }
        }
      }
    
//    delayMicroseconds(4);
  }
}

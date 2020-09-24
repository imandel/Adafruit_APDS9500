#include <SPI.h>
#include <avr/pgmspace.h>

// Registers
#define REG_Product_ID                           0x00
#define REG_Revision_ID                          0x01
#define REG_Motion                               0x02
#define REG_Delta_X_L                            0x03
#define REG_Delta_X_H                            0x04
#define REG_Delta_Y_L                            0x05
#define REG_Delta_Y_H                            0x06
#define REG_SQUAL                                0x07
#define REG_Pixel_Sum                            0x08
#define REG_Maximum_Pixel                        0x09
#define REG_Minimum_Pixel                        0x0a
#define REG_Shutter_Lower                        0x0b
#define REG_Shutter_Upper                        0x0c
#define REG_Frame_Period_Lower                   0x0d
#define REG_Frame_Period_Upper                   0x0e
#define REG_Configuration_I                      0x0f
#define REG_Configuration_II                     0x10
#define REG_Frame_Capture                        0x12
#define REG_SROM_Enable                          0x13
#define REG_Run_Downshift                        0x14
#define REG_Rest1_Rate                           0x15
#define REG_Rest1_Downshift                      0x16
#define REG_Rest2_Rate                           0x17
#define REG_Rest2_Downshift                      0x18
#define REG_Rest3_Rate                           0x19
#define REG_Frame_Period_Max_Bound_Lower         0x1a
#define REG_Frame_Period_Max_Bound_Upper         0x1b
#define REG_Frame_Period_Min_Bound_Lower         0x1c
#define REG_Frame_Period_Min_Bound_Upper         0x1d
#define REG_Shutter_Max_Bound_Lower              0x1e
#define REG_Shutter_Max_Bound_Upper              0x1f
#define REG_LASER_CTRL0                          0x20
#define REG_Observation                          0x24
#define REG_Data_Out_Lower                       0x25
#define REG_Data_Out_Upper                       0x26
#define REG_SROM_ID                              0x2a
#define REG_Lift_Detection_Thr                   0x2e
#define REG_Configuration_V                      0x2f
#define REG_Configuration_IV                     0x39
#define REG_Power_Up_Reset                       0x3a
#define REG_Shutdown                             0x3b
#define REG_Inverse_Product_ID                   0x3f
#define REG_Motion_Burst                         0x50
#define REG_SROM_Load_Burst                      0x62
#define REG_Pixel_Burst                          0x64

const int ncs = 10;
int inByte = 0;         // incoming serial byte

int frameCount = 0;
const int numFrames = 3;
uint8_t frames[numFrames][30][30];

int frameRate = 0;
byte initComplete = 0;

extern const unsigned short firmware_length;
extern const unsigned char firmware_data[];

void setup() {
  Serial.begin(9600);
  pinMode (ncs, OUTPUT);

  SPI.begin();
  SPI.setDataMode(SPI_MODE3);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(8);

  performStartup();
//  Serial.println("ADNS9800 Framegrabber");
//  dispRegisters();
  delay(1000);

  initComplete = 9;

//  setFrameRate();
  initFrameGrab();
}
void setFrameRate() {
  
    Serial.print("configuration II: ");
    Serial.println(adns_read_reg(REG_Configuration_II), BIN);
    adns_write_reg(REG_Configuration_II, 0x8);
    Serial.println(adns_read_reg(REG_Configuration_II), BIN);

  Serial.print("Frame period max bound upper: ");
  Serial.println(adns_read_reg(REG_Frame_Period_Max_Bound_Upper), HEX);
  Serial.print("Frame period max bound lower: ");
  Serial.println(adns_read_reg(REG_Frame_Period_Max_Bound_Lower), HEX);
  
    adns_write_reg(REG_Frame_Period_Max_Bound_Upper, 0x1b);
    adns_write_reg(REG_Frame_Period_Max_Bound_Lower, 0x58);
    delay(2);

  Serial.print("Frame period min bound upper: ");
  Serial.println(adns_read_reg(REG_Frame_Period_Min_Bound_Upper), HEX);
  Serial.print("Frame period min bound lower: ");
  Serial.println( adns_read_reg(REG_Frame_Period_Min_Bound_Lower), HEX);

  Serial.print("Shutter max bound upper: ");
  Serial.println(adns_read_reg(REG_Shutter_Max_Bound_Upper), HEX);
  Serial.print("Shutter max bound lower: ");
  Serial.println(adns_read_reg(REG_Shutter_Max_Bound_Lower), HEX);

  Serial.print("Frame period max bound upper: ");
  Serial.println(adns_read_reg(REG_Frame_Period_Max_Bound_Upper), HEX);
  Serial.print("Frame period max bound lower: ");
  Serial.println(adns_read_reg(REG_Frame_Period_Max_Bound_Lower), HEX);

}
void initFrameGrab() {
//  resetChip();
  initOptics();
}
byte adns_read_reg(byte reg_addr) {
  adns_com_begin();

  // send adress of the register, with MSBit = 0 to indicate it's a read
  SPI.transfer(reg_addr & 0x7f );
  delayMicroseconds(100); // tSRAD
  // read data
  byte data = SPI.transfer(0);

  delayMicroseconds(1); // tSCLK-NCS for read operation is 120ns
  adns_com_end();
  delayMicroseconds(19); //  tSRW/tSRR (=20us) minus tSCLK-NCS

  return data;
}

void adns_write_reg(byte reg_addr, byte data) {
  adns_com_begin();

  //send adress of the register, with MSBit = 1 to indicate it's a write
  SPI.transfer(reg_addr | 0x80 );
  //sent data
  SPI.transfer(data);

  delayMicroseconds(20); // tSCLK-NCS for write operation
  adns_com_end();
  delayMicroseconds(100); // tSWW/tSWR (=120us) minus tSCLK-NCS. Could be shortened, but is looks like a safe lower bound
}

void performStartup(void) {
  resetChip();
  // read registers 0x02 to 0x06 (and discard the data)
  adns_read_reg(REG_Motion);
  adns_read_reg(REG_Delta_X_L);
  adns_read_reg(REG_Delta_X_H);
  adns_read_reg(REG_Delta_Y_L);
  adns_read_reg(REG_Delta_Y_H);

  //upload the firmware
  adns_upload_firmware();
  delay(10);

  initOptics();
}

void resetChip() {
  adns_com_end(); // ensure that the serial port is reset
  adns_com_begin(); // ensure that the serial port is reset
  adns_com_end(); // ensure that the serial port is reset
  adns_write_reg(REG_Power_Up_Reset, 0x5a); // force reset
  delay(50); // wait for it to reboot
}

void initOptics() {
  // enable laser(bit 0 = 0b), in normal mode (bits 3,2,1 = 000b)
  // reading the actual value of the register is important because the real
  // default value is different from what is said in the datasheet, and if you
  // change the reserved bytes (like by writing 0x00...) it would not work.
  //should be writing 010b to bits 3,2,1 to set on continuous wave
  
  byte laser_ctrl0 = adns_read_reg(REG_LASER_CTRL0);
  adns_write_reg(REG_LASER_CTRL0, laser_ctrl0 & 0xf0 );
  Serial.print("Laser Reg pre init: "); Serial.println(laser_ctrl0);
//  adns_write_reg(REG_LASER_CTRL0, laser_ctrl0 & 0x010b );
//  
//  laser_ctrl0 = adns_read_reg(REG_LASER_CTRL0);
  Serial.print("Laser Reg post init: "); Serial.println(laser_ctrl0);
  delay(1000);

  //Serial.println("Optical Chip Initialized");
}

void adns_com_begin() {
  digitalWrite(ncs, LOW);
}

void adns_com_end() {
  digitalWrite(ncs, HIGH);
}
void adns_upload_firmware() {
  // send the firmware to the chip, cf p.18 of the datasheet
  //Serial.println("Uploading firmware...");
  // set the configuration_IV register in 3k firmware mode
  adns_write_reg(REG_Configuration_IV, 0x02); // bit 1 = 1 for 3k mode, other bits are reserved

  // write 0x1d in SROM_enable reg for initializing
  adns_write_reg(REG_SROM_Enable, 0x1d);

  // wait for more than one frame period
  delay(10); // assume that the frame rate is as low as 100fps... even if it should never be that low

  // write 0x18 to SROM_enable to start SROM download
  adns_write_reg(REG_SROM_Enable, 0x18);

  // write the SROM file (=firmware data)
  adns_com_begin();
  SPI.transfer(REG_SROM_Load_Burst | 0x80); // write burst destination adress
  delayMicroseconds(15);

  // send all bytes of the firmware
  unsigned char c;
  for (int i = 0; i < firmware_length; i++) {
    c = (unsigned char)pgm_read_byte(firmware_data + i);
    SPI.transfer(c);
    delayMicroseconds(15);
  }
  adns_com_end();
}

void dispRegisters(void) {
  int oreg[7] = {
    0x00, 0x3F, 0x2A, 0x02
  };
  char* oregname[] = {
    "Product_ID", "Inverse_Product_ID", "SROM_Version", "Motion"
  };
  byte regres;

  digitalWrite(ncs, LOW);

  int rctr = 0;
  for (rctr = 0; rctr < 4; rctr++) {
    SPI.transfer(oreg[rctr]);
    delay(1);
    Serial.println("---");
    Serial.println(oregname[rctr]);
    Serial.println(oreg[rctr], HEX);
    regres = SPI.transfer(0);
    Serial.println(regres, BIN);
    Serial.println(regres, HEX);
    delay(1);
  }
  digitalWrite(ncs, HIGH);
}

void updateFrame() {
  if (initComplete == 9) {
  for (int i = 0; i < numFrames; i++) {
    //trigger the capture
    adns_write_reg(REG_Frame_Capture, 0x93);
    adns_write_reg(REG_Frame_Capture, 0xc5);

    //wait for at least two frames
    delayMicroseconds(1000);

    //check bit zero of motion register to see if new frame is ready
    byte checkFrame = bitRead(adns_read_reg(REG_Motion), 0);
//    while(checkFrame != 1){
//      checkFrame = bitRead(adns_read_reg(REG_Motion), 0);
//    }
  
      //if (checkFrame == 1) {
        for (int j = 0; j < 30; j++) {
          for (int k = 0; k < 30; k++) {
            //read individual pixels
            frames[i][j][k] = (uint8_t)adns_read_reg(REG_Pixel_Burst);
      //    }
        }
      }
    }
//    delayMicroseconds(4);
  }
}
void loop() {
  
/*  if (Serial.available() > 0) {
  // get incoming byte:
    numFrames = (int)Serial.read() - 48;
    Serial.println(numFrames);
  } 
*/

  while(frameCount < numFrames){
    updateFrame();
    frameCount += numFrames;
  }
  if(frameCount == numFrames){
    for (int i = 0; i < numFrames; i++) {
      for (int j = 0; j < 30; j++) {
        for (int k = 0; k < 30; k++) {
          Serial.print(frames[i][j][k]);
          Serial.print(",");
          delayMicroseconds(1);
          frames[i][j][k] = 0;
          }
          Serial.println("");
      }
       Serial.println("\n");
    }
    frameCount++;
  }
  frameCount = 0; 
  
} 

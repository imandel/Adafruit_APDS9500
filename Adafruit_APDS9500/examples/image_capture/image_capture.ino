#include <SPI.h>
#include <Wire.h>
#include <Adafruit_APDS9500.h>
#include <Adafruit_SI5351.h>

#define R_RegBankSel      0xEF
#define reg_bank1         0x72
#define Trigger           0x5B    //bank 0
#define R_SRAM_Read_EnH   0x77    //bank 1
#define R_SPIOUT_PXDNUM_1 0x7C    //bank 1
#define R_SPIOUT_PXDNUM_2 0x7D    //bank 1
#define R_SPIOUT_EnH      0x7E    //bank 1

#define R_ImageHeight     0xAA   //bank 0
#define R_ImageWidth      0xAB   //bank 0
#define Cmd_HSize         0x00   //bank 1
#define Cmd_VSize         0x01   //bank 1
#define Cmd_HStart        0x02   //bank 1
#define Cmd_VStart        0x03   //bank 1
#define R_LS_Comp_DAvg_V  0x04   //bank 1
#define R_LS_Comp_DAvg_H  0x04   //bank 1
#define Cmd_ASkip_V       0x04   //bank 1
#define Cmd_ASkip_H       0x04   //bank 1
#define Cmd_DAvg_V        0x04   //bank 1
#define Cmd_DAvg_H        0x04   //bank 1

#define Cmd_HSize         0x00   //bank 1
#define Cmd_VSize         0x01   //bank 1
#define Cmd_HStart        0x02   //bank 1
#define Cmd_VStart        0x03   //bank 1
#define Cmd_ASkip_V       0x04   //bank 1
#define Cmd_ASkip_H       0x04   //bank 1
#define Cmd_DAvg_V        0x04   //bank 1
#define Cmd_VFlip         0x04   //bank 1
#define Cmd_HFlip         0x04   //bank 1


Adafruit_SI5351 clockgen = Adafruit_SI5351();
Adafruit_APDS9500 apds;


int frameCount = 9;
const int numFrames = 3;
uint8_t frames[numFrames][30][30];
const int cs = 15;

void setup(void) {
  Serial.begin(115200);
  while (!Serial) delay(10);     // will pause Zero, Leonardo, etc until serial console opens
//
//  Serial.println("Adafruit APDS9500 test!");
//  if (clockgen.begin() != ERROR_NONE){
//    /* There was a problem detecting the IC ... check your connections */
//    Serial.print("Ooops, no Si5351 detected ... Check your wiring or I2C ADDR!");
//    while(1);
//  }
//  Serial.println("OK!");
//  clockgen.setupPLLInt(SI5351_PLL_A, 15);
//  //output = pll/(div + n/d) = 375/(15+10/16) = 24mhz
//  //clockgen.setupMultisynth(clock id 0 1 or 2, SI5351_PLL_x, pll, n, d);
//  clockgen.setupMultisynth(0, SI5351_PLL_A, 15, 10, 16);
//    
//    /* Enable the clocks */
//  clockgen.enableOutputs(true);
  delay(10);
  // Try to initialize
  if (! apds.begin()) {
    Serial.println("Failed to find APDS9500 chip");
    while (1) { delay(10); }
  }
  Serial.println("APDS9500 Found!");
  Serial.println("");
  
  //enable SPI by writing 0x01 to reg bank 1 
//  apds_write_reg(R_RegBankSel, 0x01);
//  delayMicroseconds(1);
//  apds_write_reg(reg_bank1, 0x01);
//  delayMicroseconds(2);
//  apds_write_reg(R_SPIOUT_EnH, 0x01);
//  delayMicroseconds(2);
//  apds_write_reg(R_SRAM_Read_EnH, 0x01);
//  delayMicroseconds(2);
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
  Serial.println("here");

//  apds_write_reg(R_RegBankSel, 0x01);
  apds_write_reg(Trigger, 0x01);
  Serial.println(apds_read_reg(R_SPIOUT_PXDNUM_1));
  Serial.println(apds_read_reg(R_SPIOUT_PXDNUM_2));
  delayMicroseconds(1000);
//  delay(100);
}

void updateFrame() {
//  for (int i = 0; i < numFrames; i++) {
    //trigger the capture
   
  apds_write_reg(R_RegBankSel, 0x01);
  apds_write_reg(Trigger, 0x01);
  Serial.println(apds_read_reg(R_SPIOUT_PXDNUM_1));
  Serial.println(apds_read_reg(R_SPIOUT_PXDNUM_2));
  delayMicroseconds(1000);
//    for (int j = 0; j < 30; j++) {
//      for (int k = 0; k < 30; k++) {
//        //read individual pixels
//        frames[i][j][k] = (uint8_t)apds_read_reg(R_SPIOUT_PXDNUM_1);
//        Serial.println(frames[i][j][k]);
//       }
//     }
    
    delayMicroseconds(4);
//  } 
}

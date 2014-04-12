/****************************************
* File Name          : ds18b20.c
* Author             : Shang LI
* Version            : V1.0.0
* Date               : 03/04/2014
* Description        : external sensor DS18B20
******************************************/
#include "hal_ds18b20.h"

#define HAL_DS_SET_DQ() P2_0 = 1;	 
#define HAL_DS_CLR_DQ() P2_0 = 0;	
#define HAL_DS_OUT_DQ() P2DIR |= (1<<0); 
#define HAL_DS_IN_DQ()  P2DIR &= ~(1<<0);
#define HAL_DS_GET_DQ() P2_0

//LOCAL FUNCTIONS
void delay_us(unsigned int timeout);
void ResetOneWire(void);
unsigned char ReadOneWire(void);
void WriteOneWire(unsigned char data);
unsigned char* ReadSensorID(void);
unsigned int ReadSensorTempData(void);
//@fn: delay_us(uint16)
//@para: timeout
//@note: assume main f = 32MHz
void delay_us(unsigned int timeout) {
    while (timeout--)
    {
        asm("NOP");
        asm("NOP");
        asm("NOP");
    }
}

//@fn: ResetOneWire()
//@para: 
//@note: reset the sensor
void ResetOneWire(void) {
  HAL_DS_OUT_DQ(); 
  HAL_DS_CLR_DQ();
  delay_us(450);
  HAL_DS_SET_DQ();
  delay_us(60);
  HAL_DS_IN_DQ();
  delay_us(10);
  while(!(HAL_DS_GET_DQ()));
  HAL_DS_OUT_DQ();
  HAL_DS_SET_DQ();
}

//@fn: ReadOneWire()
//@para: void
//@return: uchar data, a byte once
unsigned char ReadOneWire(void) {
  unsigned data = 0, i = 0;
  for (i = 0; i < 8; i++) {
    HAL_DS_OUT_DQ();
    HAL_DS_CLR_DQ();
    data=data>>1;
    HAL_DS_SET_DQ();
    HAL_DS_IN_DQ();
    delay_us(8);
    if(HAL_DS_GET_DQ())data|=0x80;
    HAL_DS_OUT_DQ();
    HAL_DS_SET_DQ();
    delay_us(60);
  }
  return data;
}

//@fn: WriteOneWire(unsigned char data)
//@para: unsigned char data
//@brief: write one byte to the bus
void WriteOneWire(unsigned char data) {
  unsigned char i=0;
  HAL_DS_OUT_DQ();
  HAL_DS_SET_DQ();
  delay_us(16);
  for(i=0;i<8;i++) {
    HAL_DS_CLR_DQ();
    if(data&0x01) {
      HAL_DS_SET_DQ();
    } else {
      HAL_DS_CLR_DQ();
    }
    data=data>>1;
    delay_us(40);
    HAL_DS_SET_DQ();
  }
}

//@fn: ReadSensorID(void)
//@para: void
//@return: uchar* data, return to a pointer pointing the ID
//@note: this might cause memory overflow, be care to use this func
unsigned char* ReadSensorID(void) { 
    unsigned char ID[8],i,*ptr;
    ResetOneWire(); 
    WriteOneWire(0x33);
    for (i=0;i<8;i++) {
      ID[i]=ReadOneWire();
    }
    ptr=ID;
    return ptr;
}

//@fn: ReadSensorTempData(void)
//@para: void
//@return: int value of temperature, see comments below
//@brief: start a conversion
unsigned int ReadSensorTempData(void) {
  unsigned char temp_low, temp_high;
  unsigned int temp_data;
  //start a conversion first
  ResetOneWire(); 
  WriteOneWire(0xCC); //skip ROM cmd
  WriteOneWire(0x44);
  //then read temp data
  ResetOneWire();
  WriteOneWire(0xCC); //skip ROM cmd
  WriteOneWire(0xBE);
  temp_low = ReadOneWire();
  temp_high = ReadOneWire();
  temp_data = (unsigned int)((temp_high<<8|temp_low)*6.25);//e.g. return 2550 if real temp is 25.50 C 
  return temp_data;
}


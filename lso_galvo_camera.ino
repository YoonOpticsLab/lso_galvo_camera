/*  Arduino UNO R4 Minima code for DAC12 demonstration
 *
 *  Susan Parker - 19th August 2023.
 *
 * This code is "AS IS" without warranty or liability. 

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/

// RA4M1 User’s Manual: Hardware
// This doc has all the register discriptions I use:
// https://www.renesas.com/us/en/document/mah/renesas-ra4m1-group-users-manual-hardware

// ARM-developer - Accessing memory-mapped peripherals
// https://developer.arm.com/documentation/102618/0100

//#define MAX_DEVIATION 1024 // Use this to make mirror stay vertical
#define MAX_DEVIATION 4096 // Use this to make mirror scan

#define CENTER_PARK_MODE 0 // 0==Normal, 1=Keep in center
 
// Low Power Mode Control - See datasheet section 10
#define SYSTEM 0x40010000 // System Registers
#define SYSTEM_SBYCR   ((volatile unsigned short *)(SYSTEM + 0xE00C))      // Standby Control Register
#define SYSTEM_MSTPCRA ((volatile unsigned int   *)(SYSTEM + 0xE01C))      // Module Stop Control Register A

#define MSTP 0x40040000 // Module Registers
#define MSTP_MSTPCRB   ((volatile unsigned int   *)(MSTP   + 0x7000))      // Module Stop Control Register B
#define MSTPB2   2  // CAN0
#define MSTPB8   8  // IIC1
#define MSTPB9   9  // IIC0
#define MSTPB18 18  // SPI1
#define MSTPB19 19  // SPI0
#define MSTPB22 22  // SCI9
#define MSTPB29 29  // SCI2
#define MSTPB30 30  // SCI1
#define MSTPB31 31  // SCI0

#define MSTP_MSTPCRC   ((volatile unsigned int   *)(MSTP + 0x7004))        // Module Stop Control Register C
#define MSTP_MSTPCRD   ((volatile unsigned int   *)(MSTP + 0x7008))        // Module Stop Control Register D
#define MSTPD2   2  // AGT1   - Asynchronous General Purpose Timer 1 Module
#define MSTPD3   3  // AGT0   - Asynchronous General Purpose Timer 0 Module
#define MSTPD5   5  // GPT320 and GPT321 General 32 bit PWM Timer Module
#define MSTPD6   6  // GPT162 to GPT167 General 16 bit PWM Timer Module
#define MSTPD14 14  // POEG   - Port Output Enable for GPT Module Stop
#define MSTPD16 16  // ADC140 - 14-Bit A/D Converter Module
#define MSTPD19 19  // DAC8   -  8-Bit D/A Converter Module
#define MSTPD20 20  // DAC12  - 12-Bit D/A Converter Module
#define MSTPD29 29  // ACMPLP - Low-Power Analog Comparator Module
#define MSTPD31 31  // OPAMP  - Operational Amplifier Module

// The Mode Control bits are read as 1, the write value should be 1.
// Bit value 0: Cancel the module-stop state 
// Bit value 1: Enter the module-stop state.


// ====  Asynchronous General Purpose Timer (AGT) =====
#define AGTBASE 0x40084000 
#define AGT0_AGTCR    ((volatile unsigned char  *)(AGTBASE + 0x008))  // AGT Control Register

// 12-Bit D/A Converter
#define DACBASE 0x40050000          // DAC Base - DAC output on A0 (P014 AN09 DAC)
#define DAC12_DADR0    ((volatile unsigned short *)(DACBASE + 0xE000))      // D/A Data Register 0 
#define DAC12_DACR     ((volatile unsigned char  *)(DACBASE + 0xE004))      // D/A Control Register
#define DAC12_DADPR    ((volatile unsigned char  *)(DACBASE + 0xE005))      // DADR0 Format Select Register
#define DAC12_DAADSCR  ((volatile unsigned char  *)(DACBASE + 0xE006))      // D/A A/D Synchronous Start Control Register
#define DAC12_DAVREFCR ((volatile unsigned char  *)(DACBASE + 0xE007))      // D/A VREF Control Register

// =========== Ports ============
// 19.2.5 Port mn Pin Function Select Register (PmnPFS/PmnPFS_HA/PmnPFS_BY) (m = 0 to 9; n = 00 to 15)
#define PORTBASE 0x40040000 /* Port Base */

#define P000PFS 0x0800  // Port 0 Pin Function Select Register
#define PFS_P000PFS ((volatile unsigned int *)(PORTBASE + P000PFS))            // 
#define PFS_P001PFS ((volatile unsigned int *)(PORTBASE + P000PFS + ( 1 * 4))) // 
#define PFS_P002PFS ((volatile unsigned int *)(PORTBASE + P000PFS + ( 2 * 4))) // 
#define PFS_P003PFS ((volatile unsigned int *)(PORTBASE + P000PFS + ( 3 * 4))) // 
#define PFS_P004PFS ((volatile unsigned int *)(PORTBASE + P000PFS + ( 4 * 4))) // 
#define PFS_P005PFS ((volatile unsigned int *)(PORTBASE + P000PFS + ( 5 * 4))) // 
#define PFS_P006PFS ((volatile unsigned int *)(PORTBASE + P000PFS + ( 6 * 4))) // 
#define PFS_P007PFS ((volatile unsigned int *)(PORTBASE + P000PFS + ( 7 * 4))) // 
#define PFS_P008PFS ((volatile unsigned int *)(PORTBASE + P000PFS + ( 8 * 4))) // 
// #define PFS_P009PFS ((volatile unsigned int *)(PORTBASE + P000PFS + ( 9 * 4))) // Does not exist
#define PFS_P010PFS ((volatile unsigned int *)(PORTBASE + P000PFS + (10 * 4))) // 
#define PFS_P011PFS ((volatile unsigned int *)(PORTBASE + P000PFS + (11 * 4))) // 
#define PFS_P012PFS ((volatile unsigned int *)(PORTBASE + P000PFS + (12 * 4))) // 
#define PFS_P013PFS ((volatile unsigned int *)(PORTBASE + P000PFS + (13 * 4))) // 
#define PFS_P014PFS ((volatile unsigned int *)(PORTBASE + P000PFS + (14 * 4))) // A0 / DAC12
#define PFS_P015PFS ((volatile unsigned int *)(PORTBASE + P000PFS + (15 * 4))) // 

#define PFS_P100PFS_BY ((volatile unsigned char  *)(PORTBASE + 0x0843))   // 8 bits - A5
#define PFS_P101PFS_BY ((volatile unsigned char  *)(PORTBASE + 0x0843 + ( 1 * 4))) // A4
#define PFS_P102PFS_BY ((volatile unsigned char  *)(PORTBASE + 0x0843 + ( 2 * 4))) // D5
#define PFS_P103PFS_BY ((volatile unsigned char  *)(PORTBASE + 0x0843 + ( 3 * 4))) // D4
#define PFS_P104PFS_BY ((volatile unsigned char  *)(PORTBASE + 0x0843 + ( 4 * 4))) // D3
#define PFS_P105PFS_BY ((volatile unsigned char  *)(PORTBASE + 0x0843 + ( 5 * 4))) // D2
#define PFS_P106PFS_BY ((volatile unsigned char  *)(PORTBASE + 0x0843 + ( 6 * 4))) // D6
#define PFS_P107PFS_BY ((volatile unsigned char  *)(PORTBASE + 0x0843 + ( 7 * 4))) // D7
#define PFS_P108PFS_BY ((volatile unsigned char  *)(PORTBASE + 0x0843 + ( 8 * 4))) // SWDIO
#define PFS_P109PFS_BY ((volatile unsigned char  *)(PORTBASE + 0x0843 + ( 9 * 4))) // D11 / MOSI
#define PFS_P110PFS_BY ((volatile unsigned char  *)(PORTBASE + 0x0843 + (10 * 4))) // D12 / MISO
#define PFS_P111PFS_BY ((volatile unsigned char  *)(PORTBASE + 0x0843 + (11 * 4))) // D13 / SCLK
#define PFS_P112PFS_BY ((volatile unsigned char  *)(PORTBASE + 0x0843 + (12 * 4))) // D10 / CS
#define PFS_P300PFS_BY ((volatile unsigned char  *)(PORTBASE + 0x08C3))            // SWCLK (P300)
#define PFS_P301PFS_BY ((volatile unsigned char  *)(PORTBASE + 0x08C3 + (01 * 4))) // D0 / RxD (P301)
#define PFS_P302PFS_BY ((volatile unsigned char  *)(PORTBASE + 0x08C3 + (02 * 4))) // D1 / TxD (P302) 
#define PFS_P303PFS_BY ((volatile unsigned char  *)(PORTBASE + 0x08C3 + (03 * 4))) // D9
#define PFS_P304PFS_BY ((volatile unsigned char  *)(PORTBASE + 0x08C3 + (04 * 4))) // D8

int16_t loop_count = 0; 
uint8_t mode;
uint8_t time_slower;
#define MODE_POWERON 0
#define MODE_FLYBACK 2
#define MODE_FORWARD 1

void setup()
  {                                                 
  *PFS_P107PFS_BY = 0x04;               // Set D7 output low - DAC time flag pin
  setup_dac();

  *AGT0_AGTCR = 0;                     // disable Millis counter, delay etc. don't want this Interrupt messing up output stability

  //CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  //ITM->LAR = 0xc5acce55;
  //DWT->CYCCNT = 0;
  //DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

  loop_count=0;

  pinMode(2, INPUT);
  if ( (digitalRead(2)==HIGH) || CENTER_PARK_MODE ) {
    *DAC12_DADR0 = 4096/2;
    mode=MODE_POWERON;
    while(1) {}
  }

  mode=MODE_FORWARD;

  time_slower=0;
  }


#define STEP_SIZE 8
// (4096/512)
// 4096/512 = 8

void loop()                            // Total loop()          - takes c. 667nS per loop; or c. 750nS per loop with if()
  {

  time_slower += 1;                                                                                      
  if (time_slower==20) {

    if(loop_count == 0) {               // loop() and loop_count - takes c. 500nS
      *PFS_P107PFS_BY = 0x05;              // Set D7 output high    - takes c.  83nS                   // ... when test is true, adds c. 83nS to loop time to reset counter
    } else {
      *PFS_P107PFS_BY = 0x04;              // Set D7 output low     - takes c.  83nS
    }

    if ((loop_count % STEP_SIZE)==0 ) {
    *DAC12_DADR0 = loop_count;         // DAC update            - takes c. 210nS  - DAC ignores top 4 bits
    //delayMicroseconds(1);
    };

    if ((loop_count % STEP_SIZE==1) && (mode==MODE_FORWARD )){ // Line trigger -- a bit after the galvo command
      *PFS_P106PFS_BY = 0x05;              
    } else {
      *PFS_P106PFS_BY = 0x04;
    }

    if(loop_count >= ((STEP_SIZE*512)) ) {               // loop() and loop_count - takes c. 500nS
      loop_count = 0;                    // ... when test is true, adds c. 83nS to loop time to reset counter
    } else {
      ;              // Set D7 output low     - takes c.  83nS
    }

    if (mode==MODE_FORWARD) {
      loop_count++;

      if (loop_count >= MAX_DEVIATION) {
        loop_count--;
        mode = MODE_FLYBACK;
      }
    } else {
      loop_count--;
      if (loop_count < 0) {
        loop_count=0;
        mode = MODE_FORWARD;
      }
    }

    time_slower=0;
    }
}                                    // bare loop()           - takes c. 210nS


void setup_dac(void)       // Note make sure ADC is stopped before setup DAC
  {
  *MSTP_MSTPCRD &= ~(0x01 << MSTPD20);  // Enable DAC12 module
  *DAC12_DADPR    = 0x00;               // DADR0 Format Select Register - Set right-justified format
//  *DAC12_DAADSCR  = 0x80;               // D/A A/D Synchronous Start Control Register - Enable
  *DAC12_DAADSCR  = 0x00;               // D/A A/D Synchronous Start Control Register - Default
// 36.3.2 Notes on Using the Internal Reference Voltage as the Reference Voltage
  *DAC12_DAVREFCR = 0x00;               // D/A VREF Control Register - Write 0x00 first - see 36.2.5
  *DAC12_DADR0    = 0x0000;             // D/A Data Register 0 
   delayMicroseconds(10);               // Needed delay - see datasheet
  *DAC12_DAVREFCR = 0x03;               // D/A VREF Control Register - Select AVCC0/AVSS0 for Vref
  *DAC12_DACR     = 0x5F;               // D/A Control Register - 
   delayMicroseconds(5);                // 
  *DAC12_DADR0    = 0x0800;             // D/A Data Register 0 
  *PFS_P014PFS   = 0x00000000;          // Port Mode Control - Make sure all bits cleared
  *PFS_P014PFS  |= (0x1 << 15);         // ... use as an analog pin
  }
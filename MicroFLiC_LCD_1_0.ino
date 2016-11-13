/****************************/
/* Load essential libraries */
/****************************/


// Library for I2C communication
#include <Wire.h>

// Library to communicate with the RTC chip
#include <RTClib.h>

// Library for communication with MCP4738 quad-channel DAC
#include "mcp4728.h"

// Library for controlling the LCD
#include <LiquidCrystal.h>

/****************************/
/*   Initialize libraries   */
/****************************/

// initialize the library with the numbers of the interface pins
// LCD RS pin     to pin A0
// LCD Enable pin to pin A1
// LCD D4 pin     to pin A2
// LCD D5 pin     to pin A3
// LCD D6 pin     to pin A4
// LCD D7 pin     to pin A5
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

// Initialize the DAC library
// Originally from:
// https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/neuroelec/mcp4728_library-v1.3.zip
// unzip into libraries folder in the Arduino working directory
// Change mcp4728.h file in line 7 from
// #include "WProgram.h"
// into
// #include "Arduino.h"
// Change all Wire.send with Wire.write in m4728.cpp
// Change all Wire.receive with Wire.read in m4728.cpp

mcp4728 dac = mcp4728(0); // instantiate mcp4728 object, Device ID = 0


/****************************/
/* Define global variables  */
/****************************/

// Variables for LCD
uint8_t LCDlastChar;
uint8_t LCDindexW;
uint8_t LCDindexR;
uint8_t LCDindex;
uint8_t LCDdata[80];
uint8_t LCDnew[80];

// Variables for DAC
uint16_t brightness;
uint16_t contrast;
uint16_t brightnessOld;
uint16_t contrastOld;
boolean DACdata;

/* Section defining DAC channel addresses */
#define contrastCh    1
#define brightnessCh  0

// Variables for RTC
// byte-long buffer to hold RTC data
uint8_t RTCbuf;
// 12-byte-long buffer holding current time obtained from the RTC
uint8_t RTCcurTime[12];
// 12-byte-long buffer holding last time obtained from the RTC
// If is set to 0xFF, just to make sure the first time around
// the time and date is displayed on the LCD in full. Time or date
// is never 0xFF
uint8_t RTColdTime[12] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
// boolean controlling whether I2C bus can be accessed by the Micro.
// When TRUE, no time is read from the RTC
boolean RTCoff = false;
/* End of variables for RTC */

byte commandADD;
byte LCDposition;
//mcp4728 dac = mcp4728(0); // instantiate mcp4728 object, Device ID = 0
volatile uint8_t portbhistory = 0xFF;     // default is high because the pull-up

/********************/
/* Define constants */
/********************/

/* Constants for LCD */
// lookup table for HEX numbers
const byte HEXASCII[16] = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 65, 66, 67, 68, 59, 70};
// lookup table of LCD positions of time and date
const byte timePos[12] = {7, 6, 4, 3, 1, 0, 10, 9, 13, 12, 16, 15};
/* End of constants for LCD */

void setup()
{
  // Set relevant inputs and outputs of the Arduino.
  initLCDpins();

  // Setup the LCD with 20 columns and 4 rows:
  lcd.begin(20, 4);
  setLCD();

  // put your setup code here, to run once:

  Serial.begin(115200);  // initialize serial interface for print()

  // Initialize the RTC
  Wire.begin();
  
  // attach interrupt to trigger input data processing
  setInt();
  
  dac.begin();  // initialize i2c interface
  //dac.vdd(5000); // set VDD(mV) of MCP4728 for correct conversion between LSB and Vout
  /* 
  Voltage reference settings
    Vref setting = 1 (internal), Gain = 0 (x1)   ==> Vref = 2.048V
    Vref setting = 1 (internal), Gain = 1 (x2)   ==> Vref = 4.096V
    Vref setting = 0 (external), Gain = ignored  ==> Vref = VDD
  */
  //dac.setVref(1,1,1,1);    // set to use internal voltage reference (2.048V)
  //dac.setGain(1, 1, 1, 1); // set the gain of internal voltage reference ( 0 = gain x1, 1 = gain x2 )
}

void loop()
{
  // Read the current time and process it
  readRTC();
  
  // Write all characters in the buffer onto the LCD
  while(LCDindex > 0)
  {  
    updateLCD();
  }

  // Update DAC if needed
  {
    if (DACdata)
    {
      // Check if brightness setting has changed
      if (brightness != brightnessOld)
      {
        // Update brightness setting and store it into EEPROM
        updateDAC(brightnessCh, brightness);
      }

      // Check if contrast setting has changed
      if (contrast != contrastOld)
      {
        // Update contrast setting and store it into EEPROM
        updateDAC(contrastCh, contrast);
      }
      // No more new DAC data
      DACdata = false;
    }
  }
  // put your main code here, to run repeatedly:
//  Serial.print("PINC - ");
//  Serial.println(PINC, BIN);
//  Serial.print("PIND - ");
//  Serial.println(PIND, BIN);
  //dac.analogWrite(0,0,0,0);
  //delay(2000);
  //dac.analogWrite(2800,0,0,0);
  //delay(5000);
  //for (int i = 0; i < 2800; i++)
  //{
    //dac.analogWrite((i)%4095,500,(i+0)%4095,(i+0)%4095);
    //if ((i%100) == 0)
    //{
      //int value = dac.getValue(0); // get current input register value of channel 0
      //Serial.print("input register value of channel 0 = ");
      //Serial.println(value, DEC); // serial print of value
      //value = dac.getValue(1); // get current input register value of channel 1
      //Serial.print("input register value of channel 1 = ");
      //Serial.println(value, DEC); // serial print of value
    //}
  //lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  //lcd.print(millis() / 1000);
  //}
  delay(200);  //Serial.println("hello");

  //dac.analogWrite(0,500,0,0);
  //lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  //lcd.print(millis() / 1000);
//  //delay(1000);
//      Wire.beginTransmission(0x68);
//    Wire.write(0); // MSB
//    Wire.endTransmission();
//    Wire.requestFrom(0x68, 7);
//    for (byte c = 0; c < 7; c++ )
//    {
//      if (Wire.available())
//      {
//        Serial.print(Wire.read());
//        Serial.print(",");
//      }
//    }
//    Serial.println(";");
}



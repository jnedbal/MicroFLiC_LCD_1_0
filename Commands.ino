/* Section defining all commands sent to the Arduino */
#define resetLCD      1
#define startI2Ccomm  2
#define stopI2Ccomm   3
#define initializeDAC 11
#define brightnessMSB 12
#define brightnessLSB 13
#define contrastMSB   14
#define contrastLSB   15






void setInt(void)
{
  attachInterrupt(digitalPinToInterrupt(7), processInt, FALLING);
}


void processInt(void)
{
  // If address bit 0 (A0) is HIGH, LCD data is being transferred
  if (PIND & 0b10000000)
  {
    // If data bit 7 (D7) is HIGH, it is the LCD character
    if (PINB & 0b10000000)
    {
      // Store the LCD character
      LCDlastChar = PINB & 0b01111111;
    }
    // If data bit 7 (D7) is LOW, itis the position on the LCD
    else
    {
      // Store the data in the LCD draw buffer
      loadLCDdata(PINB, LCDlastChar);
    }
  }
  else
  {
    // control packet
    //Serial.print("CMD: ");
    //Serial.print(PINB & 0x0F, BIN);
    
    //Serial.print("DATA: ");
    //Serial.println((PINB & 0xF0) >> 4, BIN);
    switch(PINB & 0x0F)
    {
      case resetLCD:
        // Clear LCD, print static characters
        setLCD();
        break;

      case startI2Ccomm:
        // Allow I2C communication with RTC
        RTCoff = false;
        break;

      case stopI2Ccomm:
        // Stop I2C communication with RTC
        RTCoff = true;
        break;

      case initializeDAC:
        // Set the default DAC settings and store them in the internal
        // EEPROM so they are used each time the LCD is started.
        setDACdefault();
        break;

      // The following functions let us update the DAC controlling the
      // LCD contrast and backlight brightness. While the DAC is 12-bit,
      // Not more than 8 bits are needed. The transfer happens in two steps.
      // Each transfer brings four bits that are bit-shifted and
      // Concatenated into a 12-bit value for the DAC in the following form.
      // xxxxyyyy0000, where xxxx are the first 4 bits transferred and
      //               yyyy are the second 4 bits transferred.
      //               0000 are simply four zeros.*/
      case brightnessMSB:
        // First store the original brightness
        brightnessOld = brightness;
        // Process most significant four bits of brightness
        brightness = ((uint16_t) (PINB & 0xF0)) << 4;
        break;

      case brightnessLSB:
        // Process least significant four bits of brightness
        brightness |= ((uint16_t) (PINB & 0xF0));
        // New DAC setting has been made
        DACdata = true;
        break;

      case contrastMSB:
        // First store the original contrast
        contrastOld = contrast;
        // Process most significant four bits of contrast
        contrast = ((uint16_t) (PINB & 0xF0)) << 4;
        Serial.print("M");
        Serial.print(contrastOld);
        break;

      case contrastLSB:
        // Process least significant four bits of contrast
        contrast |= ((uint16_t) (PINB & 0xF0));
        // New DAC setting has been made
        DACdata = true;
        Serial.print("L");
        Serial.println(contrast);
        break;
    }
  }
}

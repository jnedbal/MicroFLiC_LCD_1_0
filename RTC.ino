// Read RTC packet
void readRTC(void)
{
  // if RTC is switched off, leave the function
  if (RTCoff)
  {
    return;
  }
  // Tell RTC on I2C address 0x68 to move pointer to address 0x00
  Wire.beginTransmission(0x68);
  Wire.write(0); // MSB
  Wire.endTransmission();
  // Tell RTC on I2C address 0x68 to give 7 bytes of data
  Wire.requestFrom(0x68, 7);
  // read seconds
  RTCbuf = Wire.read();
  RTCcurTime[0] = RTCbuf & 0x0F;        // seconds
  RTCcurTime[1] = RTCbuf >> 4;          // tens of seconds
  // read minutes
  RTCbuf = Wire.read();
  RTCcurTime[2] = RTCbuf & 0x0F;        // minutes
  RTCcurTime[3] = RTCbuf >> 4;          // tens of minutes
  // read hours
  RTCbuf = Wire.read();
  RTCcurTime[4] = RTCbuf & 0x0F;        // hours
  RTCcurTime[5] = (RTCbuf >> 4) & 0b11; // tens of hours
  // read weekdays
  Wire.read();
  // read days
  RTCbuf = Wire.read();
  RTCcurTime[6] = RTCbuf & 0x0F;        // days
  RTCcurTime[7] = (RTCbuf >> 4);        // tens of days
  // read months
  RTCbuf = Wire.read();
  RTCcurTime[8] = RTCbuf & 0x0F;        // months
  RTCcurTime[9] = (RTCbuf >> 4) & 0b1;  // tens of months
  // read years
  RTCbuf = Wire.read();
  RTCcurTime[10] = RTCbuf & 0x0F;       // years
  RTCcurTime[11] = RTCbuf >> 4;         // tens of years
  for (byte c = 0; c < 12; c++)
  {
    if (RTCcurTime[c] != RTColdTime[c])
    {
      loadLCDdata(timePos[c], HEXASCII[RTCcurTime[c]]);
      RTColdTime[c] = RTCcurTime[c];
    }
  }
}


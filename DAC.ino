void initDAC(void)
{
  // initialize i2c interface
  dac.begin();
}

void setDACdefault(void)
{
  // set to use internal voltage reference (2.048V)
  dac.setVref(1, 1, 1, 1);
  // set the gain of internal voltage reference ( 0 = gain x1, 1 = gain x2 )
  dac.setGain(1, 0, 0, 0);
  // set Power-Down ( 0 = Normal , 1-3 = shut down most channel circuit, no voltage out )
  //                ( 1 = 1 kOhms to GND , 2 = 100 kOhms to GND , 3 = 500 kOhms to GND )
  dac.setPowerDown(0, 0, 1, 1);
  // write all input register values and settings to EEPROM
  dac.eepromWrite(); 
  // writing to EEPROM takes about 50ms
  delay(100);
}

void updateDAC(uint8_t channel, uint16_t value)
{
  // write to input register of a DAC. Channel 0-3, Value 0-4095
  dac.analogWrite(channel, value);
  //dac.analogWrite(0, 1700);
  // write to EEPROM of DAC. Channel 0-3, Value 0-4095
  dac.eepromWrite(channel, value);
  Serial.print("CH: ");
  Serial.print(channel, DEC);
  Serial.print(", V: ");
  Serial.println(value, DEC);
  //writing to EEPROM takes about 50ms
  delay(100);
}

// Initialize pins for LCD operation
void initLCDpins(void)
{
  /* signal  pin  port
   *   D0    SS   PB0
   *   D1    SCK  PB1
   *   D2    MO   PB2
   *   D3    MI   PB3
   *   D4     8   PB4
   *   D5     9   PB5
   *   D6    10   PB6
   *   D7    11   PB7
   *   CS     7   PE6
   *   A0     6   PD7
   *   A1     5   PC6
   */
  DDRB = 0x00;          // PORTB all inputs
  pinMode(5, INPUT);    // A1_5V    address bit 1
  pinMode(6, INPUT);    // A0_5V    address bit 0
  pinMode(7, INPUT);    // ~CS_5V   Arduino chip select

}

// Set up the LCD
void setLCD(void)
{
  // Reset the counters
  LCDindexW = 0;
  LCDindexR = 0;
  // Print a message to the LCD.
  lcd.setCursor(0, 0);
  lcd.print("                    ");
  lcd.setCursor(0, 1);
  lcd.print("                    ");
  lcd.setCursor(0, 2);
  lcd.print("                    ");
  lcd.setCursor(0, 3);
  lcd.print("                    ");
  lcd.setCursor(4, 3);
  lcd.print("Hello Guys!");

  loadLCDdata(2, 58);   // Add a colon between hour and minute
  loadLCDdata(5, 58);   // Add a colon between minute and second
  loadLCDdata(11, 47);  // Add a slash between day and month
  loadLCDdata(14, 47);  // Add a slash between month and year
  loadLCDdata(40, 83);  // Add an "S" for (S)tir
  loadLCDdata(41, 116); // Add an "t" for S(t)ir
  loadLCDdata(42, 105); // Add an "i" for St(i)r
  loadLCDdata(43, 114); // Add an "r" for Sti(r)
}

// Function draws characters onto the LCD
void updateLCD(void)
{
  if (LCDindex == 0)
  {
    return;
  }
  byte tmpIn = LCDindex - 1;
  lcd.setCursor(LCDnew[tmpIn] % 20, LCDnew[tmpIn] / 20);
  lcd.write(LCDdata[LCDnew[tmpIn]]);
  LCDindex--;
}

// Function updates LCDdata register, LCDnew
// register and increments LCDindex
// index is between 0 and 79 and is defining the position of each character
// data is the ASCII character in a given position
// LCDdata is the 80 bytes long array holding each character
// LCDindexW is the latest added index
void loadLCDdata(byte index, byte data)
{
//  if (LCDdata[index] == data)
//  {
//    return;
//  }
  LCDdata[index] = data;
  LCDnew[LCDindex] = index;
  LCDindex++;
}

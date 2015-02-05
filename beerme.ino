
#include <Wire.h>  
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>

OneWire ds(10);  // on pin 10
int degree, degree_fract, degree_sign;

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);


void setup() 
{
  Serial.begin(9600);

  lcd.begin(20,4); 

  lcd.setCursor(0,0); //Start at character 4 on line 0
  lcd.print("Beer me!");
  
  lcd.setCursor(0,3);
  lcd.print("Temp: ");

  ReadTemp();
}

void loop()
{
  ReadTemp();
  lcd.setCursor(6,3);
  
  if (degree_sign)
    lcd.print("-");
  
  lcd.print(degree);
  lcd.print(".");
  if (degree_fract < 10)
    lcd.print("0");
  lcd.print(degree_fract); 
}

void ReadTemp()
{
  int HighByte, LowByte, TReading, Tc_100;
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];

  ds.reset_search();
  if ( !ds.search(addr)) {
      ds.reset_search();
      return;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
      //CRC is not valid!
      return;
  }
    
  ds.reset();
  ds.select(addr);
  ds.write(0x44,0);
  
  delay(1000);     // maybe 750ms is enough, maybe not

  ds.reset();
  ds.select(addr);
  ds.write(0xBE);         // Read Scratchpad

  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }
  
  LowByte = data[0];
  HighByte = data[1];
  TReading = (HighByte << 8) + LowByte;
  degree_sign = TReading & 0x8000;  // test most sig bit
  if (degree_sign) // negative
  {
    TReading = (TReading ^ 0xffff) + 1; // 2's comp
  }
  Tc_100 = (6 * TReading) + TReading / 4;    // multiply by (100 * 0.0625) or 6.25

  degree = Tc_100 / 100;  // separate off the whole and fractional portions
  degree_fract = Tc_100 % 100;
}


#include <LiquidCrystal.h>
#include <Wire.h>

int tmp102Address = 0x48;
float high = 0;
float low = 99;

LiquidCrystal lcd( 8, 9, 4, 5, 6, 7 );

void setup(){
  Serial.begin(9600);
  Wire.begin();
  lcd.begin(16, 2);
  lcd.print("Current:");
  lcd.setCursor(0,1);
  lcd.print("H:");
  lcd.setCursor(9,1);
  lcd.print("L:");
}

void loop(){

  float celsius = getTemperature();
  lcd.setCursor(9,0);
  lcd.print(celsius);
  if( celsius > high){
    high = celsius;
  }
  lcd.setCursor(2,1);
  lcd.print(high);
  if(celsius < low){
    low = celsius;  
  }
  lcd.setCursor(11,1);
  lcd.print(low);

  delay(5000); //just here to slow down the output. You can remove this
}

float getTemperature(){
  Wire.requestFrom(tmp102Address,2); 

  byte MSB = Wire.read();
  byte LSB = Wire.read();

  //it's a 12bit int, using two's compliment for negative
  int TemperatureSum = ((MSB << 8) | LSB) >> 4; 

  float celsius = TemperatureSum*0.0625;
  return celsius;
}

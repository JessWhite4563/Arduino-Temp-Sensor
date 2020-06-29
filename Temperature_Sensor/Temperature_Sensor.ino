#include <LiquidCrystal.h>
#include <Wire.h>
const int numReadings = 20;
const int tmp102Address = 0x48;

float high = 0;
float low = 99;
float total = 0;
float readings[numReadings];
int readIndex = 0;
int count = 0;

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
  writeDisplay(1, celsius);
  
  if( celsius > high){
    high = celsius;
    writeDisplay(2, celsius);
  }
  
  if(celsius < low){
    low = celsius;  
    writeDisplay(3, celsius);
  }
  delay(5000);
}

void writeDisplay(int type, float value){
  bool set = false;
  switch(type){
    case 1:
      lcd.setCursor(9,0);
      set = true;
      break;
    case 2:
      lcd.setCursor(3,1);
      set = true;
      break;
    case 3:
      lcd.setCursor(12,1);
      set = true;
      break;
  }
  if(set){
    lcd.print(value,1);
  }
}

float getCurrentTemperature(){
  Wire.requestFrom(tmp102Address,2); 

  byte MSB = Wire.read();
  byte LSB = Wire.read();

  //it's a 12bit int, using two's compliment for negative
  int TemperatureSum = ((MSB << 8) | LSB) >> 4; 

  float celsius = TemperatureSum*0.0625;
  return celsius;
}

float getTemperature(){
  float current = getCurrentTemperature();
  total = total - readings[readIndex];
  readings[readIndex] = current;
  total = total + current;
  readIndex++;
  count++;

  if(readIndex >= numReadings){
    readIndex = 0;
  }
  if(count > numReadings){
    count = numReadings;
  }
  float average = total / count;
  return average;
}

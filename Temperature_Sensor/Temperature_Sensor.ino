#include <LiquidCrystal.h>
#include <Wire.h>

#define BUTTON_ADC_PIN           A0  // A0 is the button ADC input
#define LCD_BACKLIGHT_PIN         3  // D3 controls LCD backlight
// ADC readings expected for the 5 buttons on the ADC input
#define RIGHT_10BIT_ADC           0  // right
#define UP_10BIT_ADC            145  // up
#define DOWN_10BIT_ADC          329  // down
#define LEFT_10BIT_ADC          505  // left
#define SELECT_10BIT_ADC        741  // right
#define BUTTONHYSTERESIS         10  // hysteresis for valid button sensing window
//return values for ReadButtons()
#define BUTTON_NONE               0  // 
#define BUTTON_RIGHT              1  // 
#define BUTTON_UP                 2  // 
#define BUTTON_DOWN               3  // 
#define BUTTON_LEFT               4  // 
#define BUTTON_SELECT             5  // 

const int numReadings = 20;
const int tmp102Address = 0x48;

float high = 0;
float low = 99;
float total = 0;
float readings[numReadings];
int readIndex = 0;
int count = 0;
int displayCounter = 0;
int buttonCounter = 0;
byte buttonWas = BUTTON_NONE;
byte buttonJustPressed  = false;
byte buttonJustReleased = false;   
bool indicator = false;

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
  updateDisplay();
}

void loop(){
  if(++displayCounter > 5000){
    updateDisplay();
    displayCounter = 0;
  }

  if(++buttonCounter > 100){
  
    switch(ReadButtons()){
      case BUTTON_SELECT :
        resetTemperature();
        updateDisplay();
        break;
    }
    buttonCounter = 0;
  }

  delay(1);
}



void updateDisplay(){
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
  if(indicator){
    lcd.setCursor(15,0);
    lcd.print(1);
  }else{
    lcd.setCursor(15,0);
    lcd.print(0);
  }
  indicator = !indicator;
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

void resetTemperature(){
  high = 0;
  low = 99;
  total = 0;
  readIndex = 0;
  count = 0;

  for (int x = 0; x < sizeof(readings) / sizeof(readings[0]); x++)
  {
    readings[x] = 0;
  }
}

byte ReadButtons() {
   unsigned int buttonVoltage;
   byte button = BUTTON_NONE;
   
   buttonVoltage = analogRead( BUTTON_ADC_PIN );
   if( buttonVoltage < ( RIGHT_10BIT_ADC + BUTTONHYSTERESIS ) ){
      button = BUTTON_RIGHT;
   }
   else if(   buttonVoltage >= ( UP_10BIT_ADC - BUTTONHYSTERESIS )
           && buttonVoltage <= ( UP_10BIT_ADC + BUTTONHYSTERESIS ) ){
      button = BUTTON_UP;
   }
   else if(   buttonVoltage >= ( DOWN_10BIT_ADC - BUTTONHYSTERESIS )
           && buttonVoltage <= ( DOWN_10BIT_ADC + BUTTONHYSTERESIS ) ) {
      button = BUTTON_DOWN;
   }
   else if(   buttonVoltage >= ( LEFT_10BIT_ADC - BUTTONHYSTERESIS )
           && buttonVoltage <= ( LEFT_10BIT_ADC + BUTTONHYSTERESIS ) ) {
      button = BUTTON_LEFT;
   }
   else if(   buttonVoltage >= ( SELECT_10BIT_ADC - BUTTONHYSTERESIS )
           && buttonVoltage <= ( SELECT_10BIT_ADC + BUTTONHYSTERESIS ) ) {
      button = BUTTON_SELECT;
   }
   //handle button flags for just pressed and just released events
   if( ( buttonWas == BUTTON_NONE ) && ( button != BUTTON_NONE ) ) {
      //the button was just pressed, set buttonJustPressed, this can optionally be used to trigger a once-off action for a button press event
      //it's the duty of the receiver to clear these flags if it wants to detect a new button change event
      buttonJustPressed  = true;
      buttonJustReleased = false;
   }
   if( ( buttonWas != BUTTON_NONE ) && ( button == BUTTON_NONE ) ) {
      buttonJustPressed  = false;
      buttonJustReleased = true;
   }
   buttonWas = button;
   
   return( button );
}

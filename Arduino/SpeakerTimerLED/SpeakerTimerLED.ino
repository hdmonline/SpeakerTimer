/* This program makes a timer to time the speaker's presentation by lighting up three LEDs in different color
 *  Start with green, after 20 min the yellow LED lights up and after 30 min the red one lights up.
*/

#include <TimerOne.h>
#include <avr/sleep.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>

// I2C address of the display.
#define DISPLAY_ADDRESS   0x70

// Create display and DS1307 objects.
Adafruit_7segment timerDisplay = Adafruit_7segment();

// on and off every second.
volatile bool displayColon = true;

// LED pins
//const byte ledPin = 13;
const uint8_t ledGreen = 3;
const uint8_t ledYellow = 4;
const uint8_t ledRed = 5;


// button pin
const uint8_t button = 2;

// time couunter in second
volatile uint32_t seconds = 0;

// ISRs
void secondIsr();
void buttonIsr();

// calculate left time
//lcdTime_t getTimeLeft(const uint32_t second);

// LED control functions
void ledsOff();
void ledOn(uint8_t led);
void setLeds(const uint32_t second);

// LCD control functions
//void lcdInit();
void segPrintLeftTime(const uint32_t second);

void setup() {
  // set pin3 in fast PWM mode (),
  noInterrupts();
  pinMode(ledGreen, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(button, INPUT_PULLUP);


  digitalWrite(ledGreen, LOW);
  digitalWrite(ledYellow, LOW);
  digitalWrite(ledRed, LOW);

  Timer1.initialize(1000000);
  Timer1.attachInterrupt( secondIsr );
  
  interrupts();
  Serial.begin(9600);
  delay(1000);
  Serial.println("xxx");
  cli();
  attachInterrupt(digitalPinToInterrupt(button), buttonIsr, FALLING);
  sei();
  interrupts();

  // Setup the display.
  timerDisplay.begin(DISPLAY_ADDRESS);
  
  
//  lcdTime = getTimeLeft(seconds);
//  lcdTime.second = 0;
//  lcdTime.minute = 30;


//  set_sleep_mode(SLEEP_MODE_IDLE);
//  cli();
//  sleep_bod_disable();
//  sei();
//  sleep_mode();
}

void loop() {
  delay(20);
//  sleep_mode();
}

// calculate remaining time
//lcdTime_t getTimeLeft(const uint32_t second) {
//  lcdTime_t lcdTime;
//  uint8_t tempSec = second%60;
//  if (tempSec == 0){
//    lcdTime.second = 0;
//    lcdTime.minute = 30 - second / 60;
//  }else {
//    lcdTime.second = 60 - second%60;
//    lcdTime.minute = 30 - (second + lcdTime.second)/60;
//  }
//  return lcdTime;
//}

// timer1 ISR
void secondIsr()
{
  Serial.println(seconds);
  noInterrupts();
  seconds++;
  setLeds(seconds);
  segPrintLeftTime(seconds);
  interrupts();
}

// no need to debounce
void buttonIsr() 
{
//  Serial.println("button pressed");
//  digitalWrite(ledPin, HIGH);
  noInterrupts();
  seconds = 0;
  Timer1.restart();
  interrupts();
  
}

void ledsOff(){
  digitalWrite(ledGreen, LOW);
  digitalWrite(ledYellow, LOW);
  digitalWrite(ledRed, LOW);
}

void ledOn(uint8_t led){
  switch(led){
    case ledGreen:
      digitalWrite(ledGreen, HIGH);
      digitalWrite(ledYellow, LOW);
      digitalWrite(ledRed, LOW);
      break;
    case ledYellow:
      digitalWrite(ledGreen, LOW);
      digitalWrite(ledYellow, HIGH);
      digitalWrite(ledRed, LOW);
      break;
    case ledRed:
      digitalWrite(ledGreen, LOW);
      digitalWrite(ledYellow, LOW);
      digitalWrite(ledRed, HIGH);
      break;
    default:
      digitalWrite(ledGreen, LOW);
      digitalWrite(ledYellow, LOW);
      digitalWrite(ledRed, LOW);
  }
}

void setLeds(const uint32_t second) {
  if (second <= 1080){
    ledOn(ledGreen);
  }
  else if (second <= 1200){
    if(second%2){
      ledsOff();
    }else{
      ledOn(ledGreen);
    }
  }
  else if (second <= 1680){
    ledOn(ledYellow);
  }
  else if (second <= 1800){
    if(second%2){
      ledsOff();
    }else{
      ledOn(ledYellow);
    }
  }
  else{
    ledOn(ledRed);
  }
}

//void lcdInit(){
//  lcd.setBrightness(30);
//  lcd.noCursor();
//  lcd.home();
//  lcd.print(" Speaker Timer  ");
//  lcd.setCursor(2,1);
//  lcd.print("Time Left: 30:00");
//}

void segPrintLeftTime(const uint32_t second) {
  // calcute remaining time
  uint8_t tempSec = second%60;
  uint8_t segSec;
  uint8_t segMin;
//  if (second%2){
//    displayColon = true;
//  }else{
//    displayColon = false;
//  }
  
  timerDisplay.setBrightness(15);
  if (second > 1800) {
    segSec = 0;
    segMin = 0;
    if(second%2){
      timerDisplay.setBrightness(0);
    }else{
      timerDisplay.setBrightness(15);
    }
  }
  else if (tempSec == 0){
    segSec = 0;
    segMin = 30 - second / 60;
  }else {
    segSec = 60 - tempSec;
    segMin = 30 - (second + segSec)/60;
  }

  // print time
  uint16_t displayValue = segMin*100 + segSec;
  timerDisplay.drawColon(displayColon);
  timerDisplay.print(displayValue, DEC);
  timerDisplay.writeDisplay();
}


/* This program makes a timer to time the speaker's presentation by lighting up three LEDs in different colors
 *  and displaying the remaining time on the 7-segment LED display.
 *  0 - 18 min      green
 *  18 - 20 min     green bink
 *  20 - 28 min     yellow
 *  28 - 30 min     yeloow blink
 *  30+ min         red
*/

#include <TimerOne.h>
#include <avr/sleep.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>

// time parameters
const float timeGreenBlink = 18;
const float timeYellow = 20;
const float timeYellowBlink = 28;
const float timeRed = 30;
const uint32_t timer1Period = 1000000;

// LED & button pins
const byte ledPin = 13;
const uint8_t ledGreen = 3;
const uint8_t ledYellow = 4;
const uint8_t ledRed = 5;
const uint8_t button = 2;

// time in seconds
const uint16_t timeGreenBlinkSec = round(timeGreenBlink * 60);
const uint16_t timeYellowSec = round(timeYellow * 60);
const uint16_t timeYellowBlinkSec = round(timeYellowBlink * 60);
const uint16_t timeRedSec = round(timeRed * 60);

// I2C address of the display.
#define DISPLAY_ADDRESS   0x70

// Create display and DS1307 objects.
Adafruit_7segment timerDisplay = Adafruit_7segment();

// time couunter in second
volatile uint32_t seconds = 0;

// timer1 flag
volatile bool fTimer1 = false;

// ISRs
void secondIsr();
void buttonIsr();

// LED control functions
void ledsOff();
void ledOn(uint8_t led);
void setLeds(const uint32_t second);

// 7-SEG control functions
void getDisplayDigits(const uint16_t secs, const uint16_t mins);
void segPrintLeftTime(const uint32_t second);

void setup() {

  // setting pin modes
  pinMode(ledPin, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(button, INPUT_PULLUP);

  // turn off LEDs
  digitalWrite(ledPin, HIGH);
  digitalWrite(ledGreen, HIGH);
  digitalWrite(ledYellow, HIGH);
  digitalWrite(ledRed, HIGH);

  // attach button interrupt
  cli();
  attachInterrupt(digitalPinToInterrupt(button), buttonIsr, FALLING);
  sei();
  interrupts();

  // Setup the display.
  timerDisplay.begin(DISPLAY_ADDRESS);
  getDisplayDigits(timeRed, 0);
  timerDisplay.writeDisplay();
}

void loop() {
  delay(20);
  timerDisplay.writeDisplay();
}

// timer1 ISR
void secondIsr()
{
  noInterrupts();
  setLeds(seconds);
  segPrintLeftTime(seconds);
  seconds++;
  interrupts();
}

// debouncing button
void buttonIsr() 
{
  static uint32_t lastInterruptTime = 0;
  uint32_t interruptTime = millis();
  
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interruptTime - lastInterruptTime > 200) {
    seconds = 0;
    ledOn(ledGreen);
    if (fTimer1) {
      Timer1.stop();
      Timer1.restart();
    }else {
      Timer1.initialize(timer1Period);
      Timer1.attachInterrupt(secondIsr);
      Timer1.restart();
      seconds = 0;
      fTimer1 = true;
    }
  }
  lastInterruptTime = interruptTime;
}

/*---------------------------------------------------------------
 *--------LED functions------------------------------------------
 *-------------------------------------------------------------*/
void ledsOff(){
  digitalWrite(ledGreen, HIGH);
  digitalWrite(ledYellow, HIGH);
  digitalWrite(ledRed, HIGH);
}

// HIGH -> turn on, LOW -> turn off
void ledOn(uint8_t led){
  switch(led){
    case ledGreen:
      digitalWrite(ledGreen, LOW);
      digitalWrite(ledYellow, HIGH);
      digitalWrite(ledRed, HIGH);
      break;
    case ledYellow:
      digitalWrite(ledGreen, HIGH);
      digitalWrite(ledYellow, LOW);
      digitalWrite(ledRed, HIGH);
      break;
    case ledRed:
      digitalWrite(ledGreen, HIGH);
      digitalWrite(ledYellow, HIGH);
      digitalWrite(ledRed, LOW);
      break;
    default:
      digitalWrite(ledGreen, HIGH);
      digitalWrite(ledYellow, HIGH);
      digitalWrite(ledRed, HIGH);
  }
}

void setLeds(const uint32_t second) {
  if (second <= timeGreenBlinkSec){
    ledOn(ledGreen);
  }
  else if (second <= timeYellowSec){
    if(second%2){
      ledsOff();
    }else{
      ledOn(ledGreen);
    }
  }
  else if (second <= timeYellowBlinkSec){
    ledOn(ledYellow);
  }
  else if (second <= timeRedSec){
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

/*---------------------------------------------------------------
 *--------7 SEG LED functions------------------------------------
 *-------------------------------------------------------------*/
void getDisplayDigits(const uint16_t mins, const uint16_t secs) {
  timerDisplay.writeDigitNum(0, mins/10, false);
  timerDisplay.writeDigitNum(1, mins%10, false);
  timerDisplay.drawColon(true);
  timerDisplay.writeDigitNum(3, secs/10, false);
  timerDisplay.writeDigitNum(4, secs%10, false);
}

void segPrintLeftTime(const uint32_t second) {
  // calcute remaining time
  uint16_t tempSec = second%60;
  uint16_t segSec;
  uint16_t segMin;
  
  if (second > timeRedSec) {
    segSec = 0;
    segMin = 0;
    if(second%2 == 1){
      timerDisplay.clear();
    }else{
      getDisplayDigits(segMin,segSec);
    }
  }
  else {
    if (tempSec == 0){
      segSec = 0;
      segMin = timeRed - second / 60;
    } else {
      segSec = 60 - tempSec;
      segMin = timeRed - (second + segSec)/60;
    }
    getDisplayDigits(segMin,segSec);
  }
  delay(10);
}


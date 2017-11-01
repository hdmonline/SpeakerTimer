/* This program makes a timer to time the speaker's presentation by lighting up three LEDs in different color
 *  Start with green, after 20 min the yellow LED lights up and after 30 min the red one lights up.
*/

#include <TimerOne.h>
#include <avr/sleep.h>


//const byte ledPin = 13;
const uint8_t ledGreen = 3;
const uint8_t ledYellow = 4;
const uint8_t ledRed = 5;

const uint8_t button = 2;

volatile uint32_t seconds = 0;

// interrupts
void secondIsr();
void buttonHandler();

// LED control functions
void ledsOff();
void ledOn(uint8_t led);
// flag
//volatile byte isStop = LOW;
//volatile byte rpmCalculated = LOW;

// Loop counter to wait
//volatile int timeCount = 0;

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
//  TCCR1A = 0;
//  TCCR1B = _BV(WGM12) | _BV(CS12);
//  TIMSK1 = _BV(OCIE1A)
//  OCR1A = 31250;

  Timer1.initialize(1000000);
  Timer1.attachInterrupt( secondIsr );
  
  interrupts();

  
  Serial.begin(9600);
  delay(1000);
  Serial.println("xxx");
  cli();
  attachInterrupt(digitalPinToInterrupt(button), buttonHandler, FALLING);
  sei();
  interrupts();
  
  set_sleep_mode(SLEEP_MODE_IDLE);
  cli();
  sleep_bod_disable();
  sei();
  sleep_mode();
}

void loop() {
  delay(20);
  sleep_mode();
}

void secondIsr()
{
  Serial.println(seconds);
  noInterrupts();
  seconds++;
  if (seconds <= 1080){
    ledOn(ledGreen);
  }
  else if (seconds <= 1200){
    if(seconds%2){
      ledsOff();
    }else{
      ledOn(ledGreen);
    }
  }
  else if (seconds <= 1680){
    ledOn(ledYellow);
  }
  else if (seconds <= 1800){
    if(seconds%2){
      ledsOff();
    }else{
      ledOn(ledYellow);
    }
  }
  else{
    ledOn(ledRed);
  }
  interrupts();
}

void buttonHandler() 
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


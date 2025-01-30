//www.elegoo.com
//2016.12.12

//IR Remote
#include <IRremote.h>
#include <ir_Lego_PF_BitStreamEncoder.h>
const int irReceiver = 8;
IRrecv irrecv(irReceiver);
decode_results results;
unsigned long key_value = 0;
char receivedVal0;  //used to store a new input value
char receivedVal1 = '?';  //used to store the value input before the newest to prevent the same signal from triggering events repeatedly
char irSignal;
int irSignal_int;
unsigned long lastPressTime;    //stores the time when an IR button press was detected

#define irCheckDelay    20    //time in milliseconds that system rests before checking if the IR sensor has detected a signal
#define irDebounceDelay 800  //debounce time for IR signal inputs

//Configuring 7-Seg Display & Shift Register
//* define the LED digit patterns, from 0 - 9
//* 1 = LED on, 0 = LED off, in this order:
//*                74HC595 pin     Q0,Q1,Q2,Q3,Q4,Q5,Q6,Q7
//*                Mapping to      a,b,c,d,e,f,g of Seven-Segment LED
byte seven_seg_digits[11] = { B00000011,  // = 0
                              B10011111,  // = 1
                              B00100101,  // = 2
                              B00001101,  // = 3
                              B10011001,  // = 4
                              B01001001,  // = 5
                              B01000001,  // = 6
                              B00011111,  // = 7
                              B00000001,  // = 8
                              B00011001,   // = 9
                              B11111111   // = null (0x0a)
                            };

// connect to the ST_CP of 74HC595 (pin 3,latch pin)
int latchPin = 3;
// connect to the SH_CP of 74HC595 (pin 4, clock pin)
int clockPin = 4;
// connect to the DS of 74HC595 (pin 2)
int dataPin = 2;

void setup() {
  // Set latchPin, clockPin, dataPin as output
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  //IR Remote
  irrecv.enableIRIn();
  irrecv.blink13(false);

  sevenSegWrite(0xa); //hex value 'a', which prints a null character (all leds off)
}

// display a number on the digital segment display
void sevenSegWrite(byte digit) {
  // set the latchPin to low potential, before sending data
  digitalWrite(latchPin, LOW);

  // the original data (bit pattern)
  shiftOut(dataPin, clockPin, LSBFIRST, seven_seg_digits[digit]);

  // set the latchPin to high potential, after sending data
  digitalWrite(latchPin, HIGH);
}

void loop() {
  irSignal = decodeIR();
  irSignal_int = irSignal - '0'; //convert irSignal value to an integer

  if ((irSignal_int >= 0) && (irSignal_int <= 9)) {
    sevenSegWrite(irSignal_int);
  }

  delay(10);
}

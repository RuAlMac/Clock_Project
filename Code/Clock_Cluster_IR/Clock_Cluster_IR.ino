/* *****Clock Cluster IR Board*****
    Peripherals:
        Shift Register for 7-Seg Display
        IR Receiver
        I2C Bus for communication w/ Clock Cluster Main Board and other Clock Cluster Boards
*/

//**************UNIQUE SETTINGS FOR THIS BOARD**************
#define boardAddress    51    //address in decimal
#define irRegDelay      500   //time in milliseconds for how long the same IR remote button must be pressed by the user in order for the program to recognize it as a separate input
//**********************************************************

//I2C
#include <Wire.h>

//IR Remote
#include <IRremote.h>
#include <ir_Lego_PF_BitStreamEncoder.h>
const int irReceiver = 6;
IRrecv irrecv(irReceiver);
decode_results results;
unsigned long key_value = 0;
char receivedVal0;  //used to store a new input value
char receivedVal1 = '?';  //used to store the value input before the newest to prevent the same signal from triggering events repeatedly

//Variables
bool clockSetMode = false;    //changed by the checkIR megafunction, puts the system into Clock Setting Mode if true
bool irUpdate = false;
unsigned long time0 = 0;      //millis raw value
unsigned long time1 = 0;      //value for use in if statements

void setup() {
  //Serial monitor (debug)
  Serial.begin(9600);
  while (!Serial) {
    //do nothing
  }
  printSerial(1);

  //I2C Initialization
  Wire.begin(boardAddress); //slave address 1
  //  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  //IR Remote
  irrecv.enableIRIn();
  irrecv.blink13(true);
}

void loop() {
  time0 = millis();
  checkIR();
}

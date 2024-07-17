/* *****Clock Cluster Main Board*****
    Peripherals:
        RTC Module
        Relay
        Shift Register for 7-Seg Display
        I2C LCD Display (16x2)
        I2C Bus for communication w/ Individual Clock Boards, other Clock Cluster Boards
*/

/*  IR Remote Used
   [1]   [2]   [3]    //numerals for setting time
   [4]   [5]   [6]
   [7]   [8]   [9]
   [*]   [0]   [#]    //[*]: toggle lighting, [#]: enter/exit clock setting (handled by board 51)
         [W]          //[W]: switch between analog mode and digital mode
   [A]   [=]   [D]    //[A]: select segment to the left, [D]: select segment to the right; [A] and [D] used for setting clock time only
         [S]          //[S]: freeze/unfreeze clock
*/

//**************UNIQUE SETTINGS FOR THIS BOARD**************
#define boardAddress    52    //address in decimal
#define initDelay       5     //time in seconds given to Individual Clock Boards to home steppers before status check
#define irRegDelay      500   //we'll see
#define maxInitCnt      2     //how many times initialization can be attempted before fatal error #01
#define clockCheckDelay 450   //time in milliseconds between RTC checks, should be less than 1 second for optimal operation
#define irCheckDelay    20    //time in milliseconds that system rests before requesting an IR signal update from Board 51
#define overrideInitChk true  //for debugging purposes; if true, program ignores errors reported by Individual Clock Boards, SHOULD BE FALSE FOR NORMAL OPERATION
#define forceClockSet   false //for debugging purposes; program will only program RTC; this must be then set to false and reuploaded, see "Forced Time Set" section below
//**********************************************************

//################Forced Time Set################
int newSetTime[] = {2024, 7, 10, 4, 16, 5, 0}; //year, month, day, weekday#, hour, min, sec; Sunday is weekday #1; takes about 10 seconds to compile & upload
//###############################################

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

//LCD Display
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); //LCD address 0x27, dec 39

byte snowflake[] = {
  B10001,
  B01010,
  B00100,
  B11111,
  B00100,
  B01010,
  B10001,
  B00000
};

//RTC Module
#include <I2C_RTC.h>
static DS3231 RTC;
int year, month, day;
int weekday0, weekday1;
int hour0, hour1;
int minute0, minute1;
int second0, second1;

//Shift Register for 7-Seg Display
byte seven_seg_digits[10] = { B11111100,  // = 0
                              B01100000,  // = 1
                              B11011010,  // = 2
                              B11110010,  // = 3
                              B01100110,  // = 4
                              B10110110,  // = 5
                              B10111110,  // = 6
                              B11100000,  // = 7
                              B11111110,  // = 8
                              B11100110   // = 9
                            };

const int latchPin = 3;
const int clockPin = 4;
const int dataPin = 2;

//Relay
const int relayPin = 7;

//Buzzer
#include <toneAC.h>
//Buzzer must be wired to pins 11 and 12 on Mega

//also using built-in LED

//Variables
volatile int clockStatus[24] = {};     //Array storing statuses of each Individual Clock Board
unsigned long time0 = 0;      //millis raw value
unsigned long time1 = 0;      //value for use in if statements
unsigned long time2 = 0;      //value for use in other features
int timeRemaining = 0;        //timer value
int initCount = 0;            //counts how many times initialization has been attempted
volatile int tempStorage = 0; //used to ensure initCount variable is accurate b/c initCount variable is being a prissy litte bitch and I don't fucking know why it's so screwy fml
bool errorsPresent;           //used during system init, false only if all Individual Clock Boards report no errors at init
char irSignal;                //stores the value of an IR signal received by the Clock_Cluster_IR board
bool keepLoop = false;        //for use in while loops that incorporate switch case operations, for simplicity
bool analogMode = false;      //indicates which display mode Individual Clock Boards should use; false=analog, true=digital
bool frozen = false;          //freezes or unfreezes clock updates

int selectedToEdit;           //used for time setting only; selectedToEdit=0 means hours are edited, selectedToEdit=1 means minutes are edited
int loadedVal;                //used to store value for by how much the hour or minute value is to be modified in time setting mode

int errorCode;                //for use in reporting fatal errors
/* Error Codes
   01: Number of system initializations exceeds allowed (error likely due to stepper homing failure on 1+ Individual Clock Boards
   02: RTC not running (likely due to poor connection between I2C bus and RTC module, maybe RTC battery died)
   03: Unknown system mode (programming bug)
*/

int mode = 0;
/* Mode Variable
  -1: Error State (manual reset required)
   0: Initialization Delay (10 sec)
   1: Checking Clock Statuses
   2: Normal operation (includes switching between analog/digital and un/freezing clock)
   3: Setting time
*/


void setup() {
  //Serial monitor (debug)
  Serial.begin(9600);
  while (!Serial) {
    //do nothing
  }
  printSerial(1);

  //I2C Initialization
  Wire.begin(boardAddress);

  //IR Remote
  irrecv.enableIRIn();
  irrecv.blink13(false);

  //RTC Module
  RTC.begin();
  RTC.startClock();
  RTC.setHourMode(CLOCK_H24); //24Hr time instead of 12Hr time (CLOCK_H12)
  delay(10);
  printTimeSerial(getTime());

  //LCD Display
  lcd.init();  //initialize the lcd
  lcd.createChar(0, snowflake);
  lcd.backlight();  //open the backlight
  printLCD(0);

  //Shift Register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  //Other Peripherals
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(relayPin, OUTPUT);

  //Checking overrides
  if (forceClockSet) {
    mode = 3;
  }

  delay(100);
}



void loop() {
  time0 = millis();

  switch (mode) {
    case -1:
      Serial.print("[+] Mode -1: Fatal Error, Manual Reset Required");
      printLCD(-1);
      while (1) {
        // do nothing
      }
      break;
    case 0:
      initCount = tempStorage;
      initCount = initCount + 1;
      tempStorage = initCount;
      delay(10);

      Serial.print("[+] Mode 0: Initialization Delay");
      Serial.print("\n\t[#] Init Count: ");
      Serial.print(initCount);
      Serial.print("\tmaxInitCnt: ");
      Serial.print(maxInitCnt);
      runCountdown(initDelay); //duration defined in board settings
      Serial.print("\n");
      mode = 1;
      break;

    case 1:
      Serial.print("[+] Mode 1: Checking Clock Statuses");
      getClockStatuses();

      Serial.print("\n\t[%] Errors present: ");
      initCount = tempStorage;

      if (overrideInitChk == true) { //debugging override accessible in settings; overrides checking all Individual Clock Boards for reported errors
        errorsPresent = false;
        Serial.print("[OVERRIDDEN] ");
      }

      switch (errorsPresent) {
        case true:
          Serial.print("True");
          if (initCount >= maxInitCnt) { //if initCnt has met or exceeded maxInitCnt
            Serial.print("\n\t[!] Fatal Error: initCnt >= maxInitCnt\n");
            Serial.print("\tError code: 1\n\n");
            errorCode = 1;
            delay(50);
            mode = -1;
          }
          else {
            Serial.print("\n\tinitCnt: ");
            Serial.print(initCount);
            Serial.print("\tmaxInitCnt: ");
            Serial.print(maxInitCnt);
            Serial.print("\tinitCnt < maxInitCnt, reattempting init\n\n");

            mode = 0;
          }
          break;
        case false:
          Serial.print("False\n");
          mode = 2;
          break;
      }
      break;

    case 2:
      Serial.print("[+] Mode 2: Normal Standby Mode (Analog/Digital, Freeze/Unfreeze)");
      printLCD(1);
      keepLoop = true;

      for (int i = 1; i <= 5; i = i + 1) {
        printTimeLCD(i);
        delay(150);
      }

      while (keepLoop) {
        checkRTC();
        irSignal = decodeIR();

        if ( (irSignal != '?') && (irSignal != ' ') ) {
          switch (irSignal) { //checks if new IR signal has been received
            case '*': //toggle lighting
              Serial.print("\n\tMode 2, IR action '*': Toggle lighting");
              toggleRelay(); //toggleRelay() contains a call to playTone()
              break;
            case '#': //go to clock setting mode
              mode = 3;
              playTone(1000, 250);
              keepLoop = false;
              break;
            case 'w':  //toggle between digital and analog modes
              Serial.print("\n\tMode 2, IR action 'w': Toggle between Analog / Digital Setting");
              toggleDigitalAnalog();
              playTone(500, 500); //duration in ms, freq in Hz
              break;
            case 's': //freeze clock
              Serial.print("\n\tMode 2, IR action 's': Freeze clock");
              toggleFrozen(); //toggleFrozen() contains calls to playTone()
              break;
          }
          irSignal = '?';
        }
      }
      break;

    case 3:
      Serial.print("[+] Mode 3: Time Setting");

      if (forceClockSet) { //Code here will only execute if the time set override in settings is true
        Serial.print(" [forceClockSet OVERRIDE ACTIVE]");
        printLCD(3);

        RTC.setYear(newSetTime[0]);
        RTC.setMonth(newSetTime[1]);
        RTC.setDay(newSetTime[2]);
        RTC.setWeek(newSetTime[3]);
        RTC.setHours(newSetTime[4]);
        RTC.setMinutes(newSetTime[5]);
        RTC.setSeconds(newSetTime[6]);

        if (getTime()) {
          Serial.print("\n");
          printTimeSerial(true);
          Serial.print("\t[!] Forced Time Set Successful; deactivate override and reprogram");
          while (1) {
            //wait for reprogram
          }
        }
      }

      //The below code will execute in normal operation
      Serial.print(" [Normal Operation]");

      printLCD(1);
      keepLoop = true;

      activeSeg = 'a';
      while (keepLoop) {
        irSignal = decodeIR();

        if ( (irSignal != '?') && (irSignal != ' ') ) {
          switch (irSignal) { //checks if new IR signal has been received
            case '*': //toggle lighting
              Serial.print("\n\tMode 3, IR action '*': Toggle lighting");
              toggleRelay(); //toggleRelay() contains a call to playTone()
              break;

            case '#': //exit clock setting mode
              mode = 2;
              playTone(1000, 250);
              keepLoop = false;
              break;

            case 'a': //choose to modify hour value
              Serial.print("\n\t[&] Hours selected to be modified");
              selectedToEdit = 0;
              loadedVal = 0;
              break;

            case 'd': //choose to modify minute value
              Serial.print("\n\t[&] Minutes selected to be modified");
              selectedToEdit = 1;
              loadedVal = 0;
              break;

            case '=': //confirm change to hour or minute value
              getTime();

              Serial.print("\n\t[!] Confirmed time change; added <");
              Serial.print(loadedVal);
              Serial.print("> to ");
              if (selectedToEdit == 0) {
                Serial.print("<hour value>");
                hour1 = hour0 + loadedVal;
                RTC.setHours(hour1);
              }
              else {
                Serial.print("<minute value>");
                minute1 = minute0 + loadedVal;
                RTC.setMinutes(minute1);
              }

              mode = 2;
              keepLoop = false;
              break;

            default:
              int z = irSignal - '0'; //converts value stored in irSignal to an int

              if ((z >= 0) && (z <= 9)) { //only executes code if an integer value of 0 <= x <= 9 is stored in z
                loadedVal = z;
                Serial.print("\n\t[&] Add <");
                Serial.print(loadedVal);
                Serial.print("> to ");

                if (selectedToEdit == 0) {
                  Serial.print("<hour> value; awaiting confirmation");
                }
                else {
                  Serial.print("<minute> value; awaiting confirmation");
                }
              }
              break;

          }
          irSignal = '?';
        }
      }

      break;

    default:
      Serial.print("\n\t[!] Fatal Error: Unknown system mode\tInput Mode: [");
      Serial.print(mode);
      Serial.print("]");
      Serial.print("\n\tError code: 3\n\n");
      errorCode = 3;
      mode = -1;
      break;
  }
}

void runCountdown(int duration) {
  timeRemaining = duration;
  time1 = time0 + (duration * 1000);
  time2 = time0 + 1000;

  lcdShowTimer(1); //prints the brackets
  Serial.print("\n\t[*] Countdown Started\n");
  Serial.print("\t[*] Duration: ");
  Serial.print(duration);
  Serial.print("\t\t");
  Serial.print("Start Time (time0) = ");
  Serial.print(time0);
  Serial.print("\t");
  Serial.print("End Time (time1) = ");
  Serial.print(time1);

  Serial.print("\n\t[*] Time Remaining: ");

  while (time1 >= time0) {
    time0 = millis();

    if (time0 >= time2) {
      timeRemaining = timeRemaining - 1;
      Serial.print(timeRemaining);
      Serial.print("  ");
      time2 = time0 + 1000;
      lcdShowTimer(2); //updates timeRemaining on the LCD
    }
  }
  Serial.print("Complete\n");
}

/*void shortCountdown(int duration) {
  timeRemaining = duration;
  time1 = time0 + (duration * 1000);
  time2 = time0 + 1000;

  Serial.print("\n\t[*] Short Countdown");
  Serial.print("\t[*] Duration: ");
  Serial.print(duration);
  Serial.print("\t\t");
  Serial.print("Start Time (time0) = ");
  Serial.print(time0);
  Serial.print("\t");
  Serial.print("End Time (time1) = ");
  Serial.print(time1);

  Serial.print("\t[*] Time Remaining: ");

  while (time1 >= time0) {
    time0 = millis();

    if (time0 >= time2) {
      timeRemaining = timeRemaining - 1;
      Serial.print(timeRemaining);
      Serial.print("  ");
      time2 = time0 + 1000;
    }
  }
  Serial.print("\tComplete\n");
  }*/

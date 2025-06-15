/* *****Single Clock Testing*****
  This program is used to test the functionality of a Single Clock Board.
  THIS PROGRAM SHOULD NOT BE USED FOR NORMAL SYSTEM OPERATION.
*/

/*  Orientation Chart: (in current code, this is calculated automatically from boardAddress)
        [0   1] <--Clock segment
        [2   3]
        [4   5]
*/

//**************UNIQUE SETTINGS FOR EACH BOARD**************
#define boardAddress     34   //address in decimal
//**********************************************************

//~~~~~~~~~~~~~~~~~~Constants~~~~~~~~~~~~~~~~~~
#define stepsInRev       2048 //number of steps in a full rotation of 28BYJ-48 Steppers
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//I2C
#include <Wire.h>
volatile byte i2c_message = 1;  //not sure if I'm going to keep this variable
char receivedMessage[7] = {'5', '_', '5', '5', '5', '5'}; //includes extra space for '\0' str terminator

//Board Position
char seg; //stores which segment this board operates in
int pos;  //stores this board's position in its segment

//System Operation
int receivedDigit;              //stores the value received from the receiveEvent I2C function
bool newTimeValue = false;      //set to true if the receiveEvent I2C function receives a new valid time value
bool analogMode = false;        //indicates which display mode Individual Clock Boards should use; false=analog, true=digital
bool frozen = false;            //indicates clock freeze status; frozen=true, freeze; frozen=false, unfreeze

bool homingComplete = false;    //used to track if homing sequence was completed successfully

int step_nums[2] = {0, 0};      //NOT A COUNT OF TOTAL STEPPER STEPS! Used to count in fours to determine next appropriate step type
int step_counts[2] = {0, 0};    //Used to track position of steppers, should be reset to 0 during homing sequence

int newAngle[2];                //updated by getNewAngle function
int currentPos[2] = {0, 0};     //stores where steppers currently are
int newPos[2] = {0, 0};           //stores where steppers must be moved to

//These capitalized variables are used in the stepOnce function
int STEP_NUMBER;
int STEPPER_PIN[4] = {0, 0, 0, 0};

//Pin Assignments
int step_1_poles[] = {5, 4, 3, 2}; //pins for poles for Stepper 1
int step_2_poles[] = {9, 8, 7, 6}; //pins for poles for Stepper 2
int hall_sensors[] = {10, 11};     //pins for Hall Effect Sensors: 1, 2

void setup() {
  //Serial monitor initialization
  Serial.begin(9600);
  while (!Serial) {
    //do nothing
  }
  //Serial.print("\n\n\n\n\n\n");
  //Serial.print("**********Initializing Single Clock Board**********\n");

  //I2C Initialization
  //Wire.begin(boardAddress); //slave address 1
  //Wire.onReceive(receiveEvent);
  //Wire.onRequest(requestEvent);

  //pinMode initialization
  pinMode(step_1_poles[0], OUTPUT);
  pinMode(step_1_poles[1], OUTPUT);
  pinMode(step_1_poles[2], OUTPUT);
  pinMode(step_1_poles[3], OUTPUT);

  pinMode(step_2_poles[0], OUTPUT);
  pinMode(step_2_poles[1], OUTPUT);
  pinMode(step_2_poles[2], OUTPUT);
  pinMode(step_2_poles[3], OUTPUT);

  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(hall_sensors[0], INPUT);
  pinMode(hall_sensors[1], INPUT);

  //Determine board segment and position
  //getBoardLocation();

  //Stepper homing
  //homeSteppers();
}

void loop() {

  // Step 'm' stepper 500 times
  for (int i = 0; i < 500; i++) {
    stepOnce(true, 'm', 2);
  }
  
  delay(100);
  quitMotor(false, 'm');

  delay(100);

  // Step 'h' stepper 500 times
  for (int i = 0; i < 500; i++) {
    stepOnce(true, 'h', 2);
  }

  delay(100);
  quitMotor(false, 'h');

  delay(100);
}

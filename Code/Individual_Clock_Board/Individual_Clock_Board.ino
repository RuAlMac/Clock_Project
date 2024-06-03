/* *****Individual Clock Board*****
    Peripherals:
        Hall Effect Sensors (2x) INPUT
        Stepper Controllers (2x) OUTPUT
        I2C Bus for communication w/ Clock Cluster boards

    Reference document below for appropriate I2C device addresses:
      https://docs.google.com/document/d/1MbWG_syWLBZlh941dtcajAVfe2LQ0b-RFf0K8S4ZPrs/edit?usp=sharing

    Reference link below for 28BYJ-48 Stepper tutorial
      https://github.com/NikodemBartnik/ArduinoTutorials/tree/master
*/

//I2C
#include <Wire.h>
volatile int i2c_message = 0;

//Stepper Motor Characteristics
int stepsInRev = 2048;
int STEP_NUMBER = 0;
int STEPPER_PIN_1 = 0;
int STEPPER_PIN_2 = 0;
int STEPPER_PIN_3 = 0;
int STEPPER_PIN_4 = 0;

int step1_num = 0;
int step2_num = 0;

//Pin Assignments
int stepper_1_in1 = 2;
int stepper_1_in2 = 3;
int stepper_1_in3 = 4;
int stepper_1_in4 = 5;

int stepper_2_in1 = 9;
int stepper_2_in2 = 8;
int stepper_2_in3 = 7;
int stepper_2_in4 = 6;

int hall_1 = 10;
int hall_2 = 11;

void setup() {
  //Serial monitor initialization
  Serial.begin(9600);
  while (!Serial) {
    //do nothing
  }
  printMessage(1);

  //I2C Initialization
  Wire.begin(0xA1); //slave address A1

  //pinMode initialization
  pinMode(stepper_1_in1, OUTPUT);
  pinMode(stepper_1_in2, OUTPUT);
  pinMode(stepper_1_in3, OUTPUT);
  pinMode(stepper_1_in4, OUTPUT);

  pinMode(stepper_2_in1, OUTPUT);
  pinMode(stepper_2_in2, OUTPUT);
  pinMode(stepper_2_in3, OUTPUT);
  pinMode(stepper_2_in4, OUTPUT);

  pinMode(hall_1, INPUT);
  pinMode(hall_2, INPUT);

  //Begin setup code
  
  homeSteppers();
}

void loop() {
  //
}

//NOTE: all this code is only used when clock is in DIGITAL display mode; ANALOG display mode code handled elsewhere

/*  Orientation Chart:
        [0   1] <--Clock segment
        [2   3]
        [4   5]
*/

/*       0     <-- Angles
          8
    275     15
      38
        30
*/

/*

void moveSteppers_digital(int digit, int movementScheme) {

  //*****Declaring local variables for this function*****
  float stepsInRev_float = static_cast<float>(stepsInRev); //stores stepsInRev as a float value
  int toMove[2] = {0, 0};                                  //stores the number of steps (+) or (-) that either stepper will need to move
  bool stepDir[2];                                         //direction: true=clockwise, false=counterclockwise
  int mx, hx;                                              //temporary index variables, count number of remaining steps for either stepper
  int dx[4] = {0, 0, 0, 0};                                //temporary index variable, stores number of steps needed to move stepper clockwise, or counter-clockwise (compares to determine direction)

  //*****Determining step direction and step distance for each stepper*****
  getNewAngle(digit);

  Serial.print("\n\n\t[!] moveSteppers_digital function called");
  Serial.print("\n\t\t[*] New digit: <");
  Serial.print(digit);
  Serial.print(">");

  for (int i = 0; i < 2; i = i + 1) {                   //~~~~~~~~~~~~~~~~~~~~~~~Code below will be executed twice (once for each stepper)

    Serial.print("\n\t\t[*] ");

    if (i == 0) {
      Serial.print("Minute");
    }
    else {
      Serial.print("Hour");
    }

    Serial.print(" Stepper");

    //Determine new step position using floating point math
    newPos[i] = static_cast<float>(newAngle[i]);        //convert int to float
    newPos[i] = ( (newPos[i] / 60.0) * stepsInRev_float);  //float-point arithmatic, converts given base-60 angle to a usable step value
    newPos[i] = static_cast<int>(newPos[i]);          //convert float to int, truncates decimal part of answer

    Serial.print("\n\t\t\t\tnewAngle: ");
    Serial.print(newAngle[i]);
    Serial.print("\n\t\t\t\tcurrentPos[i]: ");
    Serial.print(currentPos[i]);
    Serial.print("\n\t\t\t\tnewPos[i]: ");
    Serial.print(newPos[i]);

    dx[0] = currentPos[i];      //store current step pos in dx[0]
    dx[1] = newPos[i];          //store new step pos in dx[1]
    dx[2] = 0;                  //stores the number of steps needed for stepper to move to newPos CLOCKWISE
    dx[3] = 0;                  //stores the number of steps needed for stepper to move to newPos COUNTERCLOCKWISE

    //CLOCKWISE (# of steps needed to reach newPos CW)
    while (dx[0] != dx[1]) {
      if (dx[1] == 2048) {  //if dx[1] is in overflow
        dx[1] = 0; //reset to 0
        Serial.print("\n\t\t\t\tClockwise overflow, reset to 0");
      }
      else { //if dx[1] is not in overflow
        dx[1] = dx[1] + 1;
        dx[3] = dx[3] + 1;
      }
    }

    //Reset between calculations
    dx[0] = currentPos[i];      //store current step pos in dx[0]
    dx[1] = newPos[i];          //store new step pos in dx[1]
    //dx[3] = 0;                  //stores the number of steps needed for stepper to move to newPos COUNTERCLOCKWISE

    //COUNTERCLOCKWISE (# of steps needed to reach newPos CCW)
    while (dx[0] != dx[1]) {
      if (dx[1] == 0) {  //if dx[1] is in overflow
        dx[1] = 2048; //reset to 0
        Serial.print("\n\t\t\t\tCounterclockwise overflow, reset to 2048");
      }
      else { //if dx[1] is not in overflow
        dx[1] = dx[1] - 1;
        dx[2] = dx[2] + 1;
      }
    }

    Serial.print("\n\t\t\t\tdx[2]: ");
    Serial.print(dx[2]);
    Serial.print("  dx[3]: ");
    Serial.print(dx[3]);

    Serial.print("\n\t\t\t\tDirection: ");

    //Compare distances
    if (dx[2] < dx[3]) {
      stepDir[i] = true; //move clockwise
      toMove[i] = dx[2];
      Serial.print("Clockwise (dx[2] < dx[3]");
    }
    else if (dx[3] < dx[2]) {
      stepDir[i] = false; //move counterclockwise
      toMove[i] = dx[3];
      Serial.print("Counterclockwise (dx[3] < dx[2]");
    }
    else {
      stepDir[i] = true; //move clockwise
      toMove[i] = dx[2];
      Serial.print("Clockwise (dx[2] == dx[3]");
    }

    //Update current position
    currentPos[i] = newPos[i];

  } //~~~~~~~~~~~~~~~~~~~~~~~~~~end of for loop

  //Updating movement indexes
  mx = (toMove[0]);
  hx = (toMove[1]);

  moveSteppers(mx, hx, stepDir[0], stepDir[1], movementScheme); //arguments: mx, hx, stepDirM, stepDirH, movementScheme
}

void getNewAngle(int digit) { //this function returns where a stepper should point in terms of minute poisitions (angles, but in base-60)

  switch (digit) {

    case 0:
      switch (pos) {
        case 0:
          newAngle[0] = 30; //minutes
          newAngle[1] = 15; //hours
          break;
        case 1:
          newAngle[0] = 45; //minutes
          newAngle[1] = 30; //hours
          break;
        case 2:
          newAngle[0] = 30; //minutes
          newAngle[1] = 0; //hours
          break;
        case 3:
          newAngle[0] = 0; //minutes
          newAngle[1] = 30; //hours
          break;
        case 4:
          newAngle[0] = 15; //minutes
          newAngle[1] = 0; //hours
          break;
        case 5:
          newAngle[0] = 0; //minutes
          newAngle[1] = 45; //hours
          break;
      }
      break;

    case 1:
      switch (pos) {
        case 0:
          newAngle[0] = 38; //minutes
          newAngle[1] = 38; //hours
          break;
        case 1:
          newAngle[0] = 30; //minutes
          newAngle[1] = 30; //hours
          break;
        case 2:
          newAngle[0] = 38; //minutes
          newAngle[1] = 38; //hours
          break;
        case 3:
          newAngle[0] = 0; //minutes
          newAngle[1] = 30; //hours
          break;
        case 4:
          newAngle[0] = 38; //minutes
          newAngle[1] = 38; //hours
          break;
        case 5:
          newAngle[0] = 0; //minutes
          newAngle[1] = 0; //hours
          break;
      }
      break;

    case 2:
      switch (pos) {
        case 0:
          newAngle[0] = 15; //minutes
          newAngle[1] = 15; //hours
          break;
        case 1:
          newAngle[0] = 30; //minutes
          newAngle[1] = 45; //hours
          break;
        case 2:
          newAngle[0] = 30; //minutes
          newAngle[1] = 15; //hours
          break;
        case 3:
          newAngle[0] = 45; //minutes
          newAngle[1] = 0; //hours
          break;
        case 4:
          newAngle[0] = 15; //minutes
          newAngle[1] = 0; //hours
          break;
        case 5:
          newAngle[0] = 45; //minutes
          newAngle[1] = 45; //hours
          break;
      }
      break;

    case 3:
      switch (pos) {
        case 0:
          newAngle[0] = 15; //minutes
          newAngle[1] = 15; //hours
          break;
        case 1:
          newAngle[0] = 30; //minutes
          newAngle[1] = 45; //hours
          break;
        case 2:
          newAngle[0] = 15; //minutes
          newAngle[1] = 15; //hours
          break;
        case 3:
          newAngle[0] = 30; //minutes
          newAngle[1] = 45; //hours
          break;
        case 4:
          newAngle[0] = 15; //minutes
          newAngle[1] = 15; //hours
          break;
        case 5:
          newAngle[0] = 45; //minutes
          newAngle[1] = 0; //hours
          break;
      }
      break;

    case 4:
      switch (pos) {
        case 0:
          newAngle[0] = 30; //minutes
          newAngle[1] = 30; //hours
          break;
        case 1:
          newAngle[0] = 30; //minutes
          newAngle[1] = 30; //hours
          break;
        case 2:
          newAngle[0] = 15; //minutes
          newAngle[1] = 0; //hours
          break;
        case 3:
          newAngle[0] = 0; //minutes
          newAngle[1] = 30; //hours
          break;
        case 4:
          newAngle[0] = 38; //minutes
          newAngle[1] = 38; //hours
          break;
        case 5:
          newAngle[0] = 0; //minutes
          newAngle[1] = 0; //hours
          break;
      }
      break;

    case 5:
      switch (pos) {
        case 0:
          newAngle[0] = 30; //minutes
          newAngle[1] = 15; //hours
          break;
        case 1:
          newAngle[0] = 45; //minutes
          newAngle[1] = 45; //hours
          break;
        case 2:
          newAngle[0] = 15; //minutes
          newAngle[1] = 0; //hours
          break;
        case 3:
          newAngle[0] = 30; //minutes
          newAngle[1] = 45; //hours
          break;
        case 4:
          newAngle[0] = 15; //minutes
          newAngle[1] = 15; //hours
          break;
        case 5:
          newAngle[0] = 45; //minutes
          newAngle[1] = 0; //hours
          break;
      }
      break;

    case 6:
      switch (pos) {
        case 0:
          newAngle[0] = 30; //minutes
          newAngle[1] = 15; //hours
          break;
        case 1:
          newAngle[0] = 45; //minutes
          newAngle[1] = 45; //hours
          break;
        case 2:
          newAngle[0] = 30; //minutes
          newAngle[1] = 0; //hours
          break;
        case 3:
          newAngle[0] = 45; //minutes
          newAngle[1] = 30; //hours
          break;
        case 4:
          newAngle[0] = 15; //minutes
          newAngle[1] = 0; //hours
          break;
        case 5:
          newAngle[0] = 0; //minutes
          newAngle[1] = 45; //hours
          break;
      }
      break;

    case 7:
      switch (pos) {
        case 0:
          newAngle[0] = 15; //minutes
          newAngle[1] = 15; //hours
          break;
        case 1:
          newAngle[0] = 30; //minutes
          newAngle[1] = 45; //hours
          break;
        case 2:
          newAngle[0] = 38; //minutes
          newAngle[1] = 15; //hours
          break;
        case 3:
          newAngle[0] = 0; //minutes
          newAngle[1] = 30; //hours
          break;
        case 4:
          newAngle[0] = 38; //minutes
          newAngle[1] = 38; //hours
          break;
        case 5:
          newAngle[0] = 0; //minutes
          newAngle[1] = 0; //hours
          break;
      }
      break;

    case 8:
      switch (pos) {
        case 0:
          newAngle[0] = 30; //minutes
          newAngle[1] = 15; //hours
          break;
        case 1:
          newAngle[0] = 30; //minutes
          newAngle[1] = 45; //hours
          break;
        case 2:
          newAngle[0] = 30; //minutes
          newAngle[1] = 15; //hours
          break;
        case 3:
          newAngle[0] = 45; //minutes
          newAngle[1] = 30; //hours
          break;
        case 4:
          newAngle[0] = 15; //minutes
          newAngle[1] = 0; //hours
          break;
        case 5:
          newAngle[0] = 0; //minutes
          newAngle[1] = 45; //hours
          break;
      }
      break;

    case 9:
      switch (pos) {
        case 0:
          newAngle[0] = 15; //minutes
          newAngle[1] = 30; //hours
          break;
        case 1:
          newAngle[0] = 30; //minutes
          newAngle[1] = 45; //hours
          break;
        case 2:
          newAngle[0] = 0; //minutes
          newAngle[1] = 15; //hours
          break;
        case 3:
          newAngle[0] = 45; //minutes
          newAngle[1] = 30; //hours
          break;
        case 4:
          newAngle[0] = 38; //minutes
          newAngle[1] = 38; //hours
          break;
        case 5:
          newAngle[0] = 0; //minutes
          newAngle[1] = 0; //hours
          break;
      }
      break;
  }
}

*/

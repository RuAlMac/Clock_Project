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

void moveSteppers_digital(int digit, int movementScheme) {

  //*****Declaring local variables for this function*****
  int tempAngle[2];           //used to update oldStepAngle after steppers have been moved, preserves original value that comes out of getNewAngle function
  int selectedAlgorithm = 0;  //used to indicate which algorithm is to be used to determine the direction a stepper will move
  int distances[2];           //used in Direction Algorithm 2, stores added distance to 0 from start and end angle and added distance to 30 from start and end angle, respectively
  int x[2] = {0, 0};          //used as intermediary variables in arithmetic operations

  float tempPos[2];
  float stepsInRev_float = static_cast<float>(stepsInRev); //stores stepsInRev as a float value
  int toMove[2] = {0, 0}; //stores the number of steps (+) or (-) that either stepper will need to move
  bool stepDir[2]; //direction: true=clockwise, false=counterclockwise
  int mx, hx; //temporary index variables, count number of remaining steps for either stepper


  //Debug Serial Monitor output NEEDS TO BE EDITED
  Serial.print("\n\n\t[!] DIGITAL moveSteppers function called");
  Serial.print("\n\t\t[*] Input digit: <");
  Serial.print(digit);
  Serial.print(">");

  //Core algorithmic operations
  getNewAngle(digit); //updates endStepAngle variable based on received digit

  for (int i = 0; i < 2; i = i + 1) { //begin massive for loop to encompass all algorithmic operations

    tempAngle[i] = endStepAngle[i]; //used to update startStepAngle after steppers have been moved

    if (i == 0) {
      Serial.print("\n\t\t[*] Minute");
    }
    else {
      Serial.print("\n\t\t[*] Hour");
    }
    Serial.print("Stepper");

    if (startStepAngle[i] == endStepAngle[i]) { //if this stepper does not need to move for this new digit
      Serial.print("\n\t\t\t-");
      Serial.print(" Stepper not moving for this digit change");
      continue;
    }

    Serial.print("\n\t\t\t-startStepAngle: <");
    Serial.print(startStepAngle[i]);
    Serial.print("> endStepAngle: <");
    Serial.print(endStepAngle[i]);
    Serial.print(">");

    //*****Algorithm to determine in which direction a stepper should move*****
    if (( (startStepAngle[i] >= 0) && (startStepAngle[i] <= 30) ) && ( (endStepAngle[i] >= 0) && (endStepAngle[i] <= 30) )) { //if both start and end angle are in Hemisphere 1
      selectedAlgorithm = 1;
    }

    else if (( (startStepAngle[i] > 30) && (startStepAngle[i] <= 59) ) && ( (endStepAngle[i] > 30) && (endStepAngle[i] <= 59) )) { //if both start and end angle are in Hemisphere 2
      selectedAlgorithm = 1;
    }

    else {
      selectedAlgorithm = 2;
    }

    Serial.print("\n\t\t\t-Movement Direction Algorithm:");
    Serial.print("\n\t\t\t\t~Algorithm used: <");
    Serial.print(selectedAlgorithm);
    Serial.print(">");


    switch (selectedAlgorithm) {
      case 1: //If both start and end angles are in the same hemisphere, whether Hemisphere 1 or Hemisphere 2
        if ((startStepAngle[i] - endStepAngle[i]) > 0) { //if angle0 - angle1 = (+)
          stepDir[i] = false; //move counterclockwise
        }
        else { //if angle0 - angle1 = (-)
          stepDir[i] = true; //move clockwise
        }
        break;

      case 2: //If both start and end angles are NOT in the same hemisphere

        //Calculate added distance to 0
        x[0] = abs(60 - startStepAngle[i]);
        x[1] = abs(60 - endStepAngle[i]);

        while (x[0] > 30) { //ensures proper distance to 0deg / 60deg is calculated
          x[0] = x[0] - 30;
        }

        while (x[1] > 30) {
          x[1] = x[1] - 30;
        }

        distances[0] = startStepAngle[i] + endStepAngle[i];

        Serial.print("\n\t\t\t\t~Calculated added distance to 0: <");
        Serial.print(distances[0]);
        Serial.print(">");

        //Calculate added distance to 30
        x[0] = abs(30 - startStepAngle[i]);
        x[1] = abs(30 - endStepAngle[i]);

        distances[1] = x[0] + x[1];
        Serial.print("\n\t\t\t\t~Calculated added distance to 30: <");
        Serial.print(distances[0]);
        Serial.print(">");

        //Compare distances
        if (distances[0] < distances[1]) { //if (added distance to 0) > (added distance to 30)
          stepDir[i] = true; //move clockwise
        }
        else if (distances[0] > distances[1]) { //if (added distance to 0) < (added distance to 30)
          stepDir[i] = false; //move counterclockwise
        }
        else {
          stepDir[i] = true; //if distances are equal, default to clockwise
        }
        break;
    }

    Serial.print("\n\t\t\t\t~Determined direction: ");
    if (stepDir[i] == true) {
      Serial.print("Clockwise");
    }
    else {
      Serial.print("Counterclockwise");
    }

    //*****Algorithm to determine where a stepper must end up pointing*****
    Serial.print("\n\t\t\t-Movement Direction Algorithm:");

    endStepAngle[i] = tempAngle[i]; //restore endStepAngle to original value

    startStepAngle[i] = static_cast<float>(startStepAngle[i]); //converts integer in endStepAngle to a float for this operation
    startStepAngle[i] = (startStepAngle[i] / 60.0) *  stepsInRev_float; //floating point division and multiplication
    startStepAngle[i] = static_cast<int>(startStepAngle[i]); //converting float values to integers, truncates any decimal parts

    endStepAngle[i] = static_cast<float>(endStepAngle[i]); //converts integer in endStepAngle to a float for this operation
    endStepAngle[i] = (endStepAngle[i] / 60.0) *  stepsInRev_float; //floating point division and multiplication
    endStepAngle[i] = static_cast<int>(endStepAngle[i]); //converting float values to integers, truncates any decimal parts

    toMove[i] = startStepAngle[i] - endStepAngle[i];
    Serial.print("\n\t\t\t\t~To step <");
    Serial.print(abs(toMove[i]));
    Serial.print("> times");

    //*****Updating startStepAngle*****
    startStepAngle[i] = tempAngle[i];
  }

  //Setting values for mx (steps remaining for min step) and hx (steps remaining for hour step)
  mx = abs(toMove[0]);
  hx = abs(toMove[1]);

  Serial.print("\n\t\t[*] Movement Scheme: <");
  Serial.print(movementScheme);
  Serial.print("> ");


  //*****Movement schemes*****
  switch (movementScheme) {
    case 1: //Moving synchronously (minute steps once, then hour, then min, etc.)
      Serial.print("(Synchronous Movement)");
      Serial.print("\nmx = ");
      Serial.print(mx);
      Serial.print("\thx = ");
      Serial.print(hx);

      while ((mx != 0) || (hx != 0)) { //while steps remain for either minute or hour stepper

        if (mx != 0) { //if steps remain for minute stepper
          stepOnce(!stepDir[0], 'm', 2); //arguments: direction, stepper, delay in ms; Note that the stepDir for the minutes stepper is not'ted b/c min step is rotated 180 deg to hour step in the mech baseplate
          mx = mx - 1;
        }

        if (hx != 0) { //if steps remain for hour stepper
          stepOnce(stepDir[1], 'h', 2);
          hx = hx - 1;
        }
      }
      break;

    case 2: //Moving asychnronously (minute does all its stepping, then hour does its stepping)
      Serial.print("(Asynchronous Movement)");
      while (mx != 0) {
        stepOnce(stepDir[0], 'm', 2); //arguments: direction, stepper, delay in ms
        mx = mx - 1;
      }

      while (hx != 0) {
        stepOnce(stepDir[1], 'h', 2); //arguments: direction, stepper, delay in ms
        hx = hx - 1;
      }
      break;
  }

  quitMotor(true, 'm');
  quitMotor(true, 'h');
}

void getNewAngle(int digit) { //this function returns where a stepper should point in terms of minute poisitions (angles, but in base-60)

  switch (digit) {

    case 0:
      switch (pos) {
        case 0:
          endStepAngle[0] = 30; //minutes
          endStepAngle[1] = 15; //hours
          break;
        case 1:
          endStepAngle[0] = 45; //minutes
          endStepAngle[1] = 30; //hours
          break;
        case 2:
          endStepAngle[0] = 30; //minutes
          endStepAngle[1] = 0; //hours
          break;
        case 3:
          endStepAngle[0] = 0; //minutes
          endStepAngle[1] = 30; //hours
          break;
        case 4:
          endStepAngle[0] = 15; //minutes
          endStepAngle[1] = 0; //hours
          break;
        case 5:
          endStepAngle[0] = 0; //minutes
          endStepAngle[1] = 45; //hours
          break;
      }
      break;

    case 1:
      switch (pos) {
        case 0:
          endStepAngle[0] = 38; //minutes
          endStepAngle[1] = 38; //hours
          break;
        case 1:
          endStepAngle[0] = 30; //minutes
          endStepAngle[1] = 30; //hours
          break;
        case 2:
          endStepAngle[0] = 38; //minutes
          endStepAngle[1] = 38; //hours
          break;
        case 3:
          endStepAngle[0] = 0; //minutes
          endStepAngle[1] = 30; //hours
          break;
        case 4:
          endStepAngle[0] = 38; //minutes
          endStepAngle[1] = 38; //hours
          break;
        case 5:
          endStepAngle[0] = 0; //minutes
          endStepAngle[1] = 0; //hours
          break;
      }
      break;

    case 2:
      switch (pos) {
        case 0:
          endStepAngle[0] = 15; //minutes
          endStepAngle[1] = 15; //hours
          break;
        case 1:
          endStepAngle[0] = 30; //minutes
          endStepAngle[1] = 45; //hours
          break;
        case 2:
          endStepAngle[0] = 30; //minutes
          endStepAngle[1] = 15; //hours
          break;
        case 3:
          endStepAngle[0] = 45; //minutes
          endStepAngle[1] = 0; //hours
          break;
        case 4:
          endStepAngle[0] = 15; //minutes
          endStepAngle[1] = 0; //hours
          break;
        case 5:
          endStepAngle[0] = 45; //minutes
          endStepAngle[1] = 45; //hours
          break;
      }
      break;

    case 3:
      switch (pos) {
        case 0:
          endStepAngle[0] = 15; //minutes
          endStepAngle[1] = 15; //hours
          break;
        case 1:
          endStepAngle[0] = 30; //minutes
          endStepAngle[1] = 45; //hours
          break;
        case 2:
          endStepAngle[0] = 15; //minutes
          endStepAngle[1] = 15; //hours
          break;
        case 3:
          endStepAngle[0] = 30; //minutes
          endStepAngle[1] = 45; //hours
          break;
        case 4:
          endStepAngle[0] = 15; //minutes
          endStepAngle[1] = 15; //hours
          break;
        case 5:
          endStepAngle[0] = 45; //minutes
          endStepAngle[1] = 0; //hours
          break;
      }
      break;

    case 4:
      switch (pos) {
        case 0:
          endStepAngle[0] = 30; //minutes
          endStepAngle[1] = 30; //hours
          break;
        case 1:
          endStepAngle[0] = 30; //minutes
          endStepAngle[1] = 30; //hours
          break;
        case 2:
          endStepAngle[0] = 15; //minutes
          endStepAngle[1] = 0; //hours
          break;
        case 3:
          endStepAngle[0] = 0; //minutes
          endStepAngle[1] = 30; //hours
          break;
        case 4:
          endStepAngle[0] = 38; //minutes
          endStepAngle[1] = 38; //hours
          break;
        case 5:
          endStepAngle[0] = 0; //minutes
          endStepAngle[1] = 0; //hours
          break;
      }
      break;

    case 5:
      switch (pos) {
        case 0:
          endStepAngle[0] = 30; //minutes
          endStepAngle[1] = 15; //hours
          break;
        case 1:
          endStepAngle[0] = 45; //minutes
          endStepAngle[1] = 45; //hours
          break;
        case 2:
          endStepAngle[0] = 15; //minutes
          endStepAngle[1] = 0; //hours
          break;
        case 3:
          endStepAngle[0] = 30; //minutes
          endStepAngle[1] = 45; //hours
          break;
        case 4:
          endStepAngle[0] = 15; //minutes
          endStepAngle[1] = 15; //hours
          break;
        case 5:
          endStepAngle[0] = 45; //minutes
          endStepAngle[1] = 0; //hours
          break;
      }
      break;

    case 6:
      switch (pos) {
        case 0:
          endStepAngle[0] = 30; //minutes
          endStepAngle[1] = 15; //hours
          break;
        case 1:
          endStepAngle[0] = 45; //minutes
          endStepAngle[1] = 45; //hours
          break;
        case 2:
          endStepAngle[0] = 30; //minutes
          endStepAngle[1] = 0; //hours
          break;
        case 3:
          endStepAngle[0] = 45; //minutes
          endStepAngle[1] = 30; //hours
          break;
        case 4:
          endStepAngle[0] = 15; //minutes
          endStepAngle[1] = 0; //hours
          break;
        case 5:
          endStepAngle[0] = 0; //minutes
          endStepAngle[1] = 45; //hours
          break;
      }
      break;

    case 7:
      switch (pos) {
        case 0:
          endStepAngle[0] = 15; //minutes
          endStepAngle[1] = 15; //hours
          break;
        case 1:
          endStepAngle[0] = 30; //minutes
          endStepAngle[1] = 45; //hours
          break;
        case 2:
          endStepAngle[0] = 38; //minutes
          endStepAngle[1] = 15; //hours
          break;
        case 3:
          endStepAngle[0] = 0; //minutes
          endStepAngle[1] = 30; //hours
          break;
        case 4:
          endStepAngle[0] = 38; //minutes
          endStepAngle[1] = 38; //hours
          break;
        case 5:
          endStepAngle[0] = 0; //minutes
          endStepAngle[1] = 0; //hours
          break;
      }
      break;

    case 8:
      switch (pos) {
        case 0:
          endStepAngle[0] = 30; //minutes
          endStepAngle[1] = 15; //hours
          break;
        case 1:
          endStepAngle[0] = 30; //minutes
          endStepAngle[1] = 45; //hours
          break;
        case 2:
          endStepAngle[0] = 30; //minutes
          endStepAngle[1] = 15; //hours
          break;
        case 3:
          endStepAngle[0] = 45; //minutes
          endStepAngle[1] = 30; //hours
          break;
        case 4:
          endStepAngle[0] = 15; //minutes
          endStepAngle[1] = 0; //hours
          break;
        case 5:
          endStepAngle[0] = 0; //minutes
          endStepAngle[1] = 45; //hours
          break;
      }
      break;

    case 9:
      switch (pos) {
        case 0:
          endStepAngle[0] = 15; //minutes
          endStepAngle[1] = 30; //hours
          break;
        case 1:
          endStepAngle[0] = 30; //minutes
          endStepAngle[1] = 45; //hours
          break;
        case 2:
          endStepAngle[0] = 0; //minutes
          endStepAngle[1] = 15; //hours
          break;
        case 3:
          endStepAngle[0] = 45; //minutes
          endStepAngle[1] = 30; //hours
          break;
        case 4:
          endStepAngle[0] = 38; //minutes
          endStepAngle[1] = 38; //hours
          break;
        case 5:
          endStepAngle[0] = 0; //minutes
          endStepAngle[1] = 0; //hours
          break;
      }
      break;
  }
}

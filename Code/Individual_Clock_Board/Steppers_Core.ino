void homeSteppers() {
  Serial.print("\n\t[!] Running stepper homing sequence");

  Serial.print("\n\t\t[*] Homing Stepper 1 (minute stepper)");
  while (digitalRead(hall_sensors[0]) != LOW) { //hall effect sensors return 'low' if there IS a magnet nearby
    stepOnce(false, 'm', 2);
  }
  step_nums[0] = 0;     //Used in stepOnce function
  step_counts[0] = 0;   //Used in stepOnce function
  currentPos[0] = 0;    //Used in digital display mode functions
  minPosition[0] = 0;   //Used in analog display mode functions
  quitMotor(true, 'm');
  Serial.print("\n\t\t[*] Stepper 1 homed.");

  Serial.print("\n\t\t[*] Homing Stepper 2 (hour stepper)");
  while (digitalRead(hall_sensors[1]) != LOW) {
    stepOnce(false, 'h', 2);
  }
  step_nums[1] = 0;     //Used in stepOnce function
  step_counts[1] = 0;   //Used in stepOnce function
  currentPos[1] = 0;    //Used in digital display mode functions
  hourPosition[0] = 0;  //Used in analog display mode functions
  quitMotor(true, 'h');
  Serial.print("\n\t\t[*] Stepper 2 homed.");

  homingComplete = true;
}

void quitMotor(bool flash, char motor) { //should be called when a certain motor is done being used
  switch (motor) {
    case 'm':
      digitalWrite(step_1_poles[0], LOW);
      digitalWrite(step_1_poles[1], LOW);
      digitalWrite(step_1_poles[2], LOW);
      digitalWrite(step_1_poles[3], LOW);

      if (flash == true) {
        digitalWrite(step_1_poles[2], HIGH);
        delay(500);
        digitalWrite(step_1_poles[2], LOW);
      }
      break;
    case 'h':
      digitalWrite(step_2_poles[0], LOW);
      digitalWrite(step_2_poles[1], LOW);
      digitalWrite(step_2_poles[2], LOW);
      digitalWrite(step_2_poles[3], LOW);

      if (flash == true) {
        digitalWrite(step_2_poles[2], HIGH);
        delay(500);
        digitalWrite(step_2_poles[2], LOW);
      }
      break;
  }
}

void moveSteppers(int mx, int hx, int stepDirM, int stepDirH, int movementScheme) { //used for both Steppers_Analog and Steppers_Digital functions
  Serial.print("\n\t\t[*] Movement Scheme: ");
  //*****Movement schemes*****
  switch (movementScheme) {
    case 1: //Moving synchronously (minute steps once, then hour, then min, etc.)
      Serial.print("Synchronous");
      Serial.print("\n\t\t\tmx = ");
      Serial.print(mx);
      Serial.print("\thx = ");
      Serial.print(hx);

      while ((mx != 0) || (hx != 0)) { //while steps remain for either minute or hour stepper

        if (mx != 0) { //if steps remain for minute stepper
          stepOnce(!stepDirM, 'm', 2); //arguments: direction, stepper, delay in ms; Note that the stepDir for the minutes stepper is noted b/c min step is rotated 180 deg to hour step in the mech baseplate
          mx = mx - 1;
        }

        if (hx != 0) { //if steps remain for hour stepper
          stepOnce(stepDirH, 'h', 2);
          hx = hx - 1;
        }
      }
      break;

    case 2: //Moving asychnronously (minute does all its stepping, then hour does its stepping)
      Serial.print("(Asynchronous Movement)");
      while (mx != 0) {
        stepOnce(stepDirM, 'm', 2); //arguments: direction, stepper, delay in ms
        mx = mx - 1;
      }

      while (hx != 0) {
        stepOnce(stepDirH, 'h', 2); //arguments: direction, stepper, delay in ms
        hx = hx - 1;
      }
      break;
  }

  quitMotor(true, 'm');
  quitMotor(true, 'h');
}

void stepOnce(bool dir, char selectedStepper, int stepDelay) {
  //if dir = true, then rotate CLOCKWISE; dir = false, then rotate COUNTERCLOCKWISE
  //stepDelay is time in ms between steps if stepOnce is called more than once in a row, minimum and default is 2ms

  switch (selectedStepper) {
    case 'm':
      STEP_NUMBER = step_nums[0];
      STEPPER_PIN[0] = step_1_poles[0];
      STEPPER_PIN[1] = step_1_poles[1];
      STEPPER_PIN[2] = step_1_poles[2];
      STEPPER_PIN[3] = step_1_poles[3];
      break;
    case 'h':
      STEP_NUMBER = step_nums[1];
      STEPPER_PIN[0] = step_2_poles[0];
      STEPPER_PIN[1] = step_2_poles[1];
      STEPPER_PIN[2] = step_2_poles[2];
      STEPPER_PIN[3] = step_2_poles[3];
      break;
    default:
      Serial.println("error");
      break;
  }

  if (dir) {
    switch (STEP_NUMBER) {
      case 0:
        digitalWrite(STEPPER_PIN[0], HIGH);
        digitalWrite(STEPPER_PIN[1], LOW);
        digitalWrite(STEPPER_PIN[2], LOW);
        digitalWrite(STEPPER_PIN[3], LOW);
        break;
      case 1:
        digitalWrite(STEPPER_PIN[0], LOW);
        digitalWrite(STEPPER_PIN[1], HIGH);
        digitalWrite(STEPPER_PIN[2], LOW);
        digitalWrite(STEPPER_PIN[3], LOW);
        break;
      case 2:
        digitalWrite(STEPPER_PIN[0], LOW);
        digitalWrite(STEPPER_PIN[1], LOW);
        digitalWrite(STEPPER_PIN[2], HIGH);
        digitalWrite(STEPPER_PIN[3], LOW);
        break;
      case 3:
        digitalWrite(STEPPER_PIN[0], LOW);
        digitalWrite(STEPPER_PIN[1], LOW);
        digitalWrite(STEPPER_PIN[2], LOW);
        digitalWrite(STEPPER_PIN[3], HIGH);
        break;
    }
  } else {
    switch (STEP_NUMBER) {
      case 0:
        digitalWrite(STEPPER_PIN[0], LOW);
        digitalWrite(STEPPER_PIN[1], LOW);
        digitalWrite(STEPPER_PIN[2], LOW);
        digitalWrite(STEPPER_PIN[3], HIGH);
        break;
      case 1:
        digitalWrite(STEPPER_PIN[0], LOW);
        digitalWrite(STEPPER_PIN[1], LOW);
        digitalWrite(STEPPER_PIN[2], HIGH);
        digitalWrite(STEPPER_PIN[3], LOW);
        break;
      case 2:
        digitalWrite(STEPPER_PIN[0], LOW);
        digitalWrite(STEPPER_PIN[1], HIGH);
        digitalWrite(STEPPER_PIN[2], LOW);
        digitalWrite(STEPPER_PIN[3], LOW);
        break;
      case 3:
        digitalWrite(STEPPER_PIN[0], HIGH);
        digitalWrite(STEPPER_PIN[1], LOW);
        digitalWrite(STEPPER_PIN[2], LOW);
        digitalWrite(STEPPER_PIN[3], LOW);
    }
  }

  STEP_NUMBER++;
  if (STEP_NUMBER > 3) {
    STEP_NUMBER = 0;
  }

  switch (selectedStepper) {  //updating stepper numbers and stepper counts
    case 'm':
      step_nums[0] = STEP_NUMBER;

      if (dir) { //Clockwise, meaning INCREMENT count
        step_counts[0] = step_counts[0] + 1;
      }
      else { //Counterclockwise, meaning DECREMENT count
        step_counts[0] = step_counts[0] - 1;
      }

      break;

    case 'h':
      step_nums[1] = STEP_NUMBER;

      if (dir) { //Clockwise, meaning INCREMENT count
        step_counts[1] = step_counts[1] + 1;
      }
      else { //Counterclockwise, meaning DECREMENT count
        step_counts[1] = step_counts[1] - 1;
      }

      break;

    default:
      Serial.println("error");
      break;
  }

  delay(stepDelay);
}

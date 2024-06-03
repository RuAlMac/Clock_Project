void homeSteppers() {
  printMessage(2);
  while (hall_1 != LOW) {
    stepOnce(false, 1);
  }
  printMessage(3);

  printMessage(4);
  while (hall_2 != LOW) {
    stepOnce(false, 2);
  }
  printMessage(5);
}

void stepOnce(bool dir, int selectedStepper) { //dir = true, then rotate CLOCKWISE; dir = false, then rotate COUNTERCLOCKWISE
  switch (selectedStepper) {
    case 1:
      STEP_NUMBER = step1_num;
      STEPPER_PIN_1 = stepper_1_in1;
      STEPPER_PIN_2 = stepper_1_in2;
      STEPPER_PIN_3 = stepper_1_in3;
      STEPPER_PIN_4 = stepper_1_in4;
      break;
    case 2:
      STEP_NUMBER = step2_num;
      STEPPER_PIN_1 = stepper_2_in1;
      STEPPER_PIN_2 = stepper_2_in2;
      STEPPER_PIN_3 = stepper_2_in3;
      STEPPER_PIN_4 = stepper_2_in4;
      break;
    default:
      Serial.println("error");
      break;
  }

  if (dir) {
    switch (STEP_NUMBER) {
      case 0:
        digitalWrite(STEPPER_PIN_1, HIGH);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, LOW);
        break;
      case 1:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, HIGH);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, LOW);
        break;
      case 2:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, HIGH);
        digitalWrite(STEPPER_PIN_4, LOW);
        break;
      case 3:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, HIGH);
        break;
    }
  } else {
    switch (STEP_NUMBER) {
      case 0:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, HIGH);
        break;
      case 1:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, HIGH);
        digitalWrite(STEPPER_PIN_4, LOW);
        break;
      case 2:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, HIGH);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, LOW);
        break;
      case 3:
        digitalWrite(STEPPER_PIN_1, HIGH);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, LOW);
    }
  }

  STEP_NUMBER++;
  if (STEP_NUMBER > 3) {
    STEP_NUMBER = 0;
  }

  switch (selectedStepper) {
    case 1:
      step1_num = STEP_NUMBER;
      break;
    case 2:
      step2_num = STEP_NUMBER;
      break;
    default:
      Serial.println("error");
      break;
  }

  delay(2);
}

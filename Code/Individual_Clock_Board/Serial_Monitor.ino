void printMessage(int message) {
  switch (message) {
    case 1:
      Serial.print("\n\n\n\n\n\n");
      Serial.print("**********Initializing Single Clock Board**********\n");
      break;
    case 2:
      Serial.print("Homing Stepper 1 (minute stepper)\n");
      break;
    case 3:
      Serial.print("Stepper 1 homed.");
      break;
    case 4:
      Serial.print("Homing Stepper 2 (hour stepper)\n");
      break;
    case 5:
      Serial.print("Stepper 2 homed.");
      break;
  }
  Serial.print("");
}

/* LCD Screens:
   -1: Fatal Error
    0: System Init
    1: Normal Standby Operation (digital)
    2: forceClockSet Override Mode
    3: Clock Set Mode
*/

void printLCD(int message) {
  lcd.clear();
  switch (message) {
    case -1:
      lcd.print("[-1]: Fatal Err");

      lcd.setCursor(0, 1);
      lcd.print("Error Code: [  ]");
      lcd.setCursor(13, 1);

      if (errorCode <= 9) {
        lcd.print("0");
      }
      lcd.print(errorCode);
      break;

    case 0:
      lcd.print(" [0]: Sys Init  ");
      break;

    case 1:
      lcd.print("[2]: Normal <  >"); //Format: "[2]: Normal <AF>"
      lcd.setCursor(0, 1);
      lcd.print("         :  :   "); //Format: "  Mon  00:00:00 ", " Thurs 00:00:00 "

      updateSysStatuses();
      break;

    case 2:
      lcd.print("*forceClockSet* ");
      lcd.setCursor(0, 1);
      lcd.print("No Normal Operat");
      break;

    case 3:
      lcd.print(" [3]: Set Time  ");
      lcd.setCursor(0, 1);
      lcd.print("<    > < > <  > "); //Format: "<Hour> <+> <05> ", "<Min > <-> <10> "
      break;
  }
}

void lcdShowTimer(int function) {
  switch (function) {
    case 1: //case 1 just prints the brackets once (as a setup)
      lcd.setCursor(0, 1);
      lcd.print("Sys Timer: [  ]");
      break;
    case 2: //case 2 updates the time remaining
      if (timeRemaining > 9) {
        lcd.setCursor(12, 1);
      }
      else {
        lcd.setCursor(12, 1);
        lcd.print(0);
      }
      lcd.print(timeRemaining);
      break;
  }
}

void updateSysStatuses() { //updates LCD display on Mode 2 to indicate that digital or analog mode is active
  lcd.setCursor(13, 0);

  switch (analogMode) {
    case true:
      lcd.print("A");
      break;
    case false:
      lcd.print("D");
      break;
  }

  switch (frozen) {
    case true:
      lcd.write(0);   //prints a snowflake to indicate frozen
      break;
    case false:
      lcd.print("-");
      break;
  }
}

void updateEditPlan() { //updates LCD display on Mode 3 to indicate what will happen once user confirms time change plan

  //Hours or Minutes?
  if (selectedToEdit == 0) {
    lcd.setCursor(1, 1);
    lcd.print("Hour");
  }
  else {
    lcd.setCursor(1, 1);
    lcd.print("Min ");
  }

  //Addition or Subtraction?
  if (selectedOperation == 0) {
    lcd.setCursor(8, 1);
    lcd.print("-");
  }
  else {
    lcd.setCursor(8, 1);
    lcd.print("+");
  }

  //By how much?
  lcd.setCursor(12, 1);
  if (loadedVal[0] != '?') {
    lcd.print(loadedVal[0] - '0');  //print the value in loadedVal[0] as an integer
  }
  else {
    lcd.print(" ");
  }
  lcd.setCursor(13, 1);
  if (loadedVal[1] != '?') {
    lcd.print(loadedVal[1] - '0');  //print the value in loadedVal[1] as an integer
  }
  else {
    lcd.print(" ");
  }
}

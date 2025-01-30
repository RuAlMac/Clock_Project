void getClockStatuses() {
  int checkClock = 0;
  int x = 0;

  while (checkClock <= 37) {
    Wire.requestFrom(checkClock, 1); //Request from the selected clock one byte of data (one int uses 2 bytes)
    while (Wire.available()) {
      x = Wire.read();
      clockStatus[checkClock] = x;
    }
    delay(10);
    checkClock = checkClock + 1;
    switch (checkClock) {
      case 8:
        checkClock = 10;
        break;
      case 18:
        checkClock = 20;
        break;
      case 28:
        checkClock = 30;
        break;
    }
  }
  printClockStatuses();
}

int printClockStatuses() {
  errorsPresent = false;

  int i = 0;
  while (i <= 37) {
    Serial.print("\n\t");
    Serial.print("Clock ");
    Serial.print(i);
    if (i < 10) { //this just makes things look nicer in the Serial monitor
      Serial.print(" ");
    }
    Serial.print(" ....... ");
    if (clockStatus[i] == 1) {
      Serial.print("OK");
    }
    else {
      Serial.print("ERROR [");
      Serial.print(clockStatus[i]);
      Serial.print("]");
      errorsPresent = true;
    }
    i = i + 1;
    switch (i) {
      case 8:
        i = 10;
        Serial.print("\n");
        break;
      case 18:
        i = 20;
        Serial.print("\n");
        break;
      case 28:
        i = 30;
        Serial.print("\n");
        break;
    }
  }
  Serial.print("\n");
}

void sendSysUpdate(int sysUpdateType) { //This function will handle all transmissions to Individual Clock Boards and the other Clock Cluster Boards, except for retrieving status information during init

  Serial.print("\n\t[!] sendSysUpdate function called");
  Serial.print("\n\t\t[*] Update type: ");

  //Creating local variables used in this function
  int newTime[] = {0, 0, 0, 0};

  char message[7] = {'5', '_', '5', '5', '5', '5'}; //a string of characters, with an extra value for the '\0' string terminator
  //Format: "0_1624" (0=update type, 1624=new time); "1_0000" (1=update type, 0000=digital mode); "1_1000" (1=update type, 1000=analog mode); "2_0000" (2=update type, 0000=unfrozen); etc.

  switch (sysUpdateType) { //prepare message string to be sent based on sysUpdateType
    case 0: //time update
      Serial.print("(0) Time update");
      //get new time values
      newTime[0] = hour0 / 10;   //Seg A
      newTime[1] = hour0 % 10;   //Seg B
      newTime[2] = minute0 / 10; //Seg C
      newTime[3] = minute0 % 10; //Seg D

      message[0] = '0'; //set message type
      message[2] = newTime[0] + '0'; //converts value in newTime to a character
      message[3] = newTime[1] + '0';
      message[4] = newTime[2] + '0';
      message[5] = newTime[3] + '0';
      break;

    case 1: //display mode update
      message[0] = '1'; //set message type
      Serial.print("(1) Display mode (digital or analog)");

      switch (analogMode) {//toggles existing value first
        case false:
          Serial.print("\n\t\t[$] New clock mode: Analog");
          analogMode = true;    //Switching to analog mode
          message[2] = '1'; //display mode: analog
          break;
        case true:
          Serial.print("\n\t\t[$] New clock mode: Digital");
          analogMode = false;   //Switching to digital mode
          message[2] = '0'; //display mode: analog
          break;
      }

      message[3] = '0';
      message[4] = '0';
      message[5] = '0';
      
      updateSysStatuses();
      break;

    case 2: //frozen mode update
      message[0] = '2'; //set message type
      Serial.print("(2) Frozen/Unfrozen");

      if (frozen) {
        frozen = false;
        Serial.print("\n\t\t[$] Clock unfrozen");
        message[2] = '0'; //indicates UNFROZEN
        playTone(100, 500); //duration in ms, freq in Hz
        delay(100);
        playTone(100, 500); //duration in ms, freq in Hz
      }
      else {
        frozen = true;
        Serial.print("\n\t\t[$] Clock frozen");
        message[2] = '1'; //indicates FROZEN
        playTone(250, 1000); //duration in ms, freq in Hz
        delay(100);
        playTone(250, 1000); //duration in ms, freq in Hz
      }

      message[3] = '0';
      message[4] = '0';
      message[5] = '0';
      
      updateSysStatuses();
      break;
  }

  Serial.print("\n\t\t[*] Message Array: ");
  Serial.print(message);

  //Sending message
  int i = 0; //Target Individual Clock Boards first
  while (i < 38) {
    Wire.beginTransmission(i);
    Wire.write(message);
    Wire.endTransmission();

    i = i + 1;
  }

  i = 50; //Then target Clock Cluster Boards
  while (i < 54) {
    Wire.beginTransmission(i);
    Wire.write(message);
    Wire.endTransmission();

    i = i + 1;

    if (i == 52) { //skip Board 52 (is this board)
      i = 53;
    }

  }

  Serial.print("\n\t\t[!] Message sent\n");
}

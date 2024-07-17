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

void sendClockUpdate() {
  int timeSend[] = {0, 0, 0, 0};

  timeSend[0] = hour0 / 10;   //Seg A
  timeSend[1] = hour0 % 10;   //Seg B
  timeSend[2] = minute0 / 10; //Seg C
  timeSend[3] = minute0 % 10; //Seg D


  if (frozen == false) {

    //Send to Clock Cluster Boards (except to itself)
    int i = 50;
    while (i != 0) {
      Wire.beginTransmission(i);
      switch (i) {
        case 50:
          Wire.write(timeSend[0]);
          i = 51;
          break;
        case 51:
          Wire.write(timeSend[1]);
          i = 53;
          break;
        case 53:
          Wire.write(timeSend[3]);
          i = 0;
          break;
      }
      Wire.endTransmission();
    }

    //Send to all Individual Clock Boards
    i = 0;
    while (i <= 37) {
      Wire.beginTransmission(i);

      if (i < 10) {
        Wire.write(timeSend[0]);
      }
      else if (i < 20) {
        Wire.write(timeSend[1]);
      }
      else if (i < 30) {
        Wire.write(timeSend[2]);
      }
      else {
        Wire.write(timeSend[3]);
      }

      Wire.endTransmission();

      i = i + 1;
      switch (i) {
        case 8:
          i = 10;
          break;
        case 18:
          i = 20;
          break;
        case 28:
          i = 30;
          break;
      }
    }
  }
}

void toggleDigitalAnalog() {
  int i = 0;
  int m = 0;

  switch (analogMode) {
    case false:
      Serial.print("\n\t\t[$] New clock mode: Analog");
      analogMode = true;    //Switching to analog mode
      m = 40;               //m==40 indicates analog mode
      break;
    case 1:
      Serial.print("\n\t\t[$] New clock mode: Digital");
      analogMode = false;   //Switching to digital mode
      m = 30;               //m==30 indicates digital mode
      break;
  }

  updateSysStatuses();  //LCD function, prints "D" or "A" depending on analogMode

  //Send to all Individual Clock Boards
  Serial.print("\n\t\t[$] Sending new clock mode to all Individual Clock Boards");
  i = 0;
  while (i <= 37) {
    Wire.beginTransmission(i);
    Wire.write(m);
    Wire.endTransmission();

    i = i + 1;
    switch (i) {
      case 8:
        i = 10;
        break;
      case 18:
        i = 20;
        break;
      case 28:
        i = 30;
        break;
    }
  }
}

void toggleFrozen() {
  if (frozen) {
    frozen = false;
    playTone(250, 500); //duration in ms, freq in Hz
    delay(100);
    playTone(250, 500); //duration in ms, freq in Hz
  }
  else {
    frozen = true;
    playTone(250, 1000); //duration in ms, freq in Hz
    delay(100);
    playTone(250, 1000); //duration in ms, freq in Hz
  }

  updateSysStatuses();
}

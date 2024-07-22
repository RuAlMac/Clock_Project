void receiveEvent() {
  int i = 0;

  Serial.print("\n\n\t[@] Local receiveEvent triggered:");
  Serial.print("\n\t\t[*] Board <");
  Serial.print(boardAddress);
  Serial.print("> on Segment <");
  Serial.print(seg);
  Serial.print("> at Position <");
  Serial.print(pos);
  Serial.print(">");

  while (Wire.available()) {
    receivedMessage[i] = Wire.read();
    i = i + 1;
  }

  Serial.print("\n\t\t[>] Received message: ");
  Serial.print(receivedMessage);
  Serial.print("\n\t\t[>] Message type: ");

  switch (receivedMessage[0]) { //Actions based off of update type (indicated by receivedMessage[0])
    case '0': //Time update
      Serial.print("Time update");

      switch (seg) {
        case 'A':
          receivedDigit = (receivedMessage[2] - '0'); //0_1000, converts char to int
          break;
        case 'B':
          receivedDigit = (receivedMessage[3] - '0'); //0_0100
          break;
        case 'C':
          receivedDigit = (receivedMessage[4] - '0'); //0_0010
          break;
        case 'D':
          receivedDigit = (receivedMessage[5] - '0'); //0_0001
          break;
      }
      newTimeValue = true;
      break;

    case '1': //Display mode update (digital or analog)
      Serial.print("Display mode update\n\t\t");

      switch (receivedMessage[2]) { //0_1000
        case '0': //digital
          analogMode = false;
          Serial.print("[*] Digital");
          break;
        case '1': //frozen
          analogMode = true;
          Serial.print("[*] Analog");
          break;
      }

      break;

    case '2': //Freeze update (unfrozen or frozen)
      Serial.print("Freeze update\n\t\t");

      switch (receivedMessage[2]) { //0_1000
        case '0': //unfrozen
          frozen = false;
          Serial.print("[*] Unfrozen");
          break;
        case '1': //frozen
          frozen = true;
          Serial.print("[*] Frozen");
          break;
      }

      break;
  }

}

void requestEvent() {
  Wire.write(i2c_message); //0 means an error occurred, 1 means all good
}

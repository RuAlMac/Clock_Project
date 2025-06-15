/*

int awaitSerialInput() {
  while (Serial.available() == 0) { //halt program until input received from Serial monitor
  }

  int serialInput = Serial.parseInt();

  return serialInput;
}

void getBoardLocation() {
  int x1 = boardAddress / 10; //integer division discards decimal portion of answer, functionally floor division
  int x0 = boardAddress % 10;

  switch (x1) { //assigning board segment
    case 0:
      seg = 'A';
      break;
    case 1:
      seg = 'B';
      break;
    case 2:
      seg = 'C';
      break;
    case 3:
      seg = 'D';
      break;
  }

  pos = x0; //assigning board position

  Serial.print("\t[*] Board position determined: Segment ");
  Serial.print(seg);
  Serial.print(", Position ");
  Serial.print(pos);
  Serial.print("\n");
}

*/

void moveSteppers_analog() {
  int hourPosition[2]; //[0] is current, [1] is new
  int minPosition[2];
  int hour;
  int minute;
  int mx, hx;         //used to store number of steps each stepper should move
  float stepsInRev_float = static_cast<float>(stepsInRev);

  //*****Hour value*****
  hour = ( ((receivedMessage[2] - '0') * 10) + (receivedMessage[3] - '0') ); //0_1100

  if (hour > 12) { //subtract 12 for display on 12-hour analog clocks
    hour = hour - 12;
  }

  //*****Minute value*****
  minute = ( ((receivedMessage[4] - '0') * 10) + (receivedMessage[5] - '0') ); //0_0011

  //*****Convert to usable step value*****
  minPosition[1] = static_cast<float>(minute);                       //convert int to float
  minPosition[1] = ( (minPosition[1] / 60.0) * stepsInRev_float);   //float-point arithmatic, converts given base-60 angle to a usable step value
  minPosition[1] = static_cast<int>(minPosition[1]);                //convert float to int, truncates decimal part of answer

  hourPosition[1] = static_cast<float>(hour);                         //convert int to float
  hourPosition[1] = ( (hourPosition[1] / 60.0) * stepsInRev_float);   //float-point arithmatic, converts given base-60 angle to a usable step value
  hourPosition[1] = static_cast<int>(hourPosition[1]);                //convert float to int, truncates decimal part of answer

  //*****Calculate mx and hx*****
  mx = abs(minPosition[1] - minPosition[0]);
  if (mx == 59) { //if a new hour begins
    mx = 1;
  }

  hx = abs(hourPosition[1] - hourPosition[0]);
  if (hx == 11) { //if a new hour begins
    hx = 1;
  }

  //*****Serial Monitor output*****
  Serial.print("\n\n\t[!] moveSteppers_digital function called");
  Serial.print("\n\t\t[*] Time: ");
  Serial.print(hour);
  Serial.print(":");
  Serial.print(minute);

  Serial.print("\n\t\t[*] New step positions:");
  Serial.print("\n\t\t\t-Hour Hand: ");
  Serial.print(hourPosition[1]);
  Serial.print("\n\t\t\t-Minute Hand: ");
  Serial.print(minPosition[1]);

  Serial.print("\n\t\t[*] Step clockwise by:");
  Serial.print("\n\t\t\t-hx: ");
  Serial.print(hx);
  Serial.print("\n\t\t\t-mx: ");
  Serial.print(mx);

  //*****Move steppers*****
  moveSteppers(mx, hx, true, true, 1); //arguments: mx, hx, stepDirM (true=clockwise), stepDirH (true=clockwise), movementScheme (1=simultaneous movement)

  //*****Update minPosition[0] and hourPosition[0]*****
  minPosition[0] = minPosition[1];
  hourPosition[0] = hourPosition[0];
}

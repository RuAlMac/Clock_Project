bool getTime() {
  bool clockRunning;

  if (RTC.isRunning()) {
    clockRunning = true;
  }
  else {
    clockRunning = false;
    Serial.print("\n\t[!] Fatal Error: RTC not running\n");
    errorCode = 1;
    mode = -1; //allow code to continue until this function is done, no return placed here, helpful for serial monitor output
  }

  year = RTC.getYear();
  month = RTC.getMonth();
  day = RTC.getDay();
  weekday0 = RTC.getWeek();
  hour0 = RTC.getHours();
  minute0 = RTC.getMinutes();
  second0 = RTC.getSeconds();

  return clockRunning;
}

String getWeekdayStr() {
  String weekday_str;

  switch (weekday0)
  {
    case 1:
      weekday_str = "Sun";
      break;
    case 2:
      weekday_str = "Mon";
      break;
    case 3:
      weekday_str = "Tues";
      break;
    case 4:
      weekday_str = "Wed";
      break;
    case 5:
      weekday_str = "Thur";
      break;
    case 6:
      weekday_str = "Fri";
      break;
    case 7:
      weekday_str = "Sat";
      break;
  }
  return weekday_str;
}

void printTimeSerial(bool clockRunning) { //prints RTC data to Serial Monitor (debug)
  Serial.print("\tRTC DS3231 Data: ");

  if (clockRunning) {
    Serial.print(getWeekdayStr());
    Serial.print(" ");

    if (month <= 9) {
      Serial.print("0");
    }

    Serial.print(month);
    Serial.print("/");

    if (day <= 9) {
      Serial.print("0");
    }

    Serial.print(day);
    Serial.print("/");
    Serial.print(year);
    Serial.print(" ");

    if (hour0 <= 9) {
      Serial.print("0");
    }

    Serial.print(hour0);
    Serial.print(":");

    if (minute0 <= 9) {
      Serial.print("0");
    }

    Serial.print(minute0);
    Serial.print(":");

    if (second0 <= 9) {
      Serial.print("0");
    }

    Serial.print(second0);
  }
  else {
    Serial.print("Warning: RTC not running, inaccurate data/time");
  }
  Serial.print("\n");
}

void checkRTC() {
  weekday1 = weekday0;
  hour1 = hour0;
  minute1 = minute0;
  second1 = second0;

  weekday0 = RTC.getWeek();
  hour0 = RTC.getHours();
  minute0 = RTC.getMinutes();
  second0 = RTC.getSeconds();

  //**Weekday Updates**
  if (weekday1 != weekday0) {
    printTimeLCD(1);
  }

  //**Hour Updates**
  if (hour1 != hour0) {
    printTimeLCD(2);
  }

  //**Minute Updates**
  if (minute1 != minute0) {
    printTimeLCD(3);
    sendSysUpdate(0); //update type 0: time update, no need to include a timeUpdate for changes in hours because a minute change will already cover it (ex. change from 18:59 to 19:00 includes a minute change and an hour change)
  }

  //**Second Updates**
  if (second1 != second0) {
    printTimeLCD(4);
    blinkSeconds();
  }

  delay(clockCheckDelay); //built-in delay to not over-exhaust I2C bus or RTC
}

void blinkSeconds() {
  if (second0 % 2 == 0) { //only visual effect of a second update is blinking onboard LED, reducing unnecessarily verbose Serial monitor output
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else {
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void printTimeLCD(int x) {
  switch (x) {
    case 1: //print weekday_str
      lcd.setCursor(1, 1);
      lcd.print("    ");
      lcd.setCursor(1, 1);

      lcd.print(getWeekdayStr());
      break;
    case 2: //print hour
      lcd.setCursor(7, 1);
      if (hour0 <= 9) {
        lcd.print("0");
      }
      lcd.print(hour0);
      break;
    case 3: //print minute
      lcd.setCursor(10, 1);
      if (minute0 <= 9) {
        lcd.print("0");
      }
      lcd.print(minute0);
      break;
    case 4: //print second
      lcd.setCursor(13, 1);
      if (second0 <= 9) {
        lcd.print("0");
      }
      lcd.print(second0);
      break;
  }
}

void checkIfValid(int timeAspect) { //timeAspect==0 is hour, timeAspect==1 is minute
  switch (timeAspect) {
    case 0:
      while (1) {
        if (hour1 >= 24) {
          hour1 = hour1 - 24;
          day = day + 1;
        }

        if (hour1 < 0) {
          hour1 = hour1 + 24;
          day = day - 1;
        }

        if ( (hour1 >= 0) && (hour1 < 24) ) { //valid hour
          break;
        }
      }
      break;

    case 1:
      while (1) {
        if (minute1 > 59) {
          minute1 = minute1 - 59;
          hour1 = hour1 + 1;
        }

        if (minute1 < 0) {
          minute1 = minute1 + 59;
          hour1 = hour1 - 1;
        }

        if ( (minute1 >= 0) && (minute1 <= 59) ) { //valid minute
          break;
        }
      }
      break;
  }
}

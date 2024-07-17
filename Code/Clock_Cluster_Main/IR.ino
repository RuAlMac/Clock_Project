char decodeIR() {
  if (irrecv.decode(&results)) {

    if (results.value == 0XFFFFFFFF) {
      results.value = key_value;
    }

    switch (results.value) {
      case 0xFFA25D:
        receivedVal0 = '1';
        break;
      case 0xFF629D:
        receivedVal0 = '2';
        break;
      case 0xFFE21D:
        receivedVal0 = '3';
        break;
      case 0xFF22DD:
        receivedVal0 = '4';
        break;
      case 0xFF02FD:
        receivedVal0 = '5';
        break ;
      case 0xFFC23D:
        receivedVal0 = '6';
        break ;
      case 0xFFE01F:
        receivedVal0 = '7';
        break ;
      case 0xFFA857:
        receivedVal0 = '8';
        break ;
      case 0xFF906F:
        receivedVal0 = '9';
        break ;
      case 0xFF9867:
        receivedVal0 = '0';
        break ;
      case 0xFF6897:
        receivedVal0 = '*';
        break ;
      case 0xFFB04F:
        receivedVal0 = '#';
        break ;
      case 0xFF18E7:
        receivedVal0 = 'w';
        break ;
      case 0xFF10EF:
        receivedVal0 = 'a';
        break ;
      case 0xFF4AB5:
        receivedVal0 = 's';
        break ;
      case 0xFF5AA5:
        receivedVal0 = 'd';
        break ;
      case 0xFF38C7:
        receivedVal0 = '=';
        break ;
      default:
        receivedVal0 = '?';
        break;
    }

    key_value = results.value;
    irrecv.resume();

    //The following code determines if an input signal is considered valid

    if (receivedVal0 == '?') { //Check 1: Bypass following checks if an unidentified signal is received.
      return '?';
    }

    if (receivedVal0 != receivedVal1) { //Check 2: Check if a new input is different from the previous input. Valid if different.
      Serial.print("\n\t[>] IR Signal Received: ");
      Serial.print("[");
      Serial.print(receivedVal0);
      Serial.print("]");

      receivedVal1 = receivedVal0;
    }
    else { //Check 3: If same input, freeze program for time defined by irRegDelay. Delete value in receivedVal1, then allow program to keep running. A repeated input will then be accepted once. If received twice, this will apply again.
      time1 = time0 + irRegDelay;
      Serial.print("\n\t[!] Repeat IR Signal; program frozen for ");
      Serial.print(irRegDelay);
      Serial.print("ms. receivedVal1 value deleted.");
      while (time0 < time1) {
        time0 = millis();
        //do nothing
      }
      receivedVal1 = '?';
    }
    return receivedVal0;  //return receivedVal0 if a valid IR signal is received
  }
  return '?';   //return '?' if no IR signal is received
}

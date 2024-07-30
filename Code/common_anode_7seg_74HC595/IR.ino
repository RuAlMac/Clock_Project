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

    //The following code determines if an input signal is considered valid

    if (receivedVal0 == '?') { //Check 1: Bypass following checks if an unidentified signal is received.
      irrecv.resume();
      return '?';
    }

    if (millis() - lastPressTime > irDebounceDelay) { //Check 2: debounce delay

      while (irrecv.decode(&results)) { //do nothing until no new IR signal received
        delay(10);
        irrecv.resume();
      }

      lastPressTime = millis(); // Update the time when the button was last pressed

      irrecv.resume();
      return receivedVal0;  //return receivedVal0 if an IR signal is received after an appropriate debounce time
    }
  }

  return '?';   //return '?' if no IR signal is received
}

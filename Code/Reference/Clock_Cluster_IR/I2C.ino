void requestEvent() {
  //Clock Cluster Main board will request update from this board to see if any IR values have been input
  if (irUpdate) { //if there is a new IR signal to be sent
    Wire.write(receivedVal0);
    Serial.print("\n\t[*] Sending to main: ");
    Serial.print(receivedVal0);
    
    irUpdate = false;
  }
  else { //if no new IR signal is to be sent, this board returns '?'
    Wire.write('?');
  }
}

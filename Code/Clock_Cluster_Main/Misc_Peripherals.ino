void toggleRelay() {
  playTone(250, 500); //duration in ms, freq in Hz
  
  if (digitalRead(7) == LOW) {
    digitalWrite(7, HIGH);
  }
  else {
    digitalWrite(7, LOW);
  }
}

void playTone(unsigned long duration, int freq) {
  //freq in Hz, freq limits span from ?? to 2.66 MHz; I like 500Hz
  int vol = 10;   //from 0=mute to 10=max
  toneAC(freq, vol, duration);
}

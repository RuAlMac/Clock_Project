void toggleRelay() {
  playTone(100, 500); //duration in ms, freq in Hz
  
  if (digitalRead(7) == LOW) {
    digitalWrite(7, HIGH);
  }
  else {
    digitalWrite(7, LOW);
  }
}

void playTone(unsigned long duration, int freq0) {
  //freq in Hz, freq limits span from 100Hz to 15KHz; I like 500Hz
  int vol = 10;   //from 0=mute to 10=max
  int freq1 = freq0 + 0; //optional increase frequency, helpful if switching between tone libraries
  TimerFreeTone(buzzerPin, freq1, duration, vol);
}

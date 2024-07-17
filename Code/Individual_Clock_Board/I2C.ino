void receiveEvent(){
  //
}

void requestEvent(){
  Wire.write(i2c_message); //0 means an error occurred, 1 means all good
}

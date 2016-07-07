void serial_transmit(){
  Serial.print("Temp: ");
  Serial.print(get_temp());
  Serial.print("    Mass: ");
  Serial.print(get_mass());
  for(int i = 0; i < NO_NFC_READERS; i++){
    Serial.print("    NFC");
    Serial.print(i);
    Serial.print(":  ");
    Serial.print(get_nfc(i));
  }
  Serial.println();
}


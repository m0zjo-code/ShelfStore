
  PN532_I2C pn532i2c(Wire);
  PN532 nfc(pn532i2c);
  
  #define TCA_ADDRESS 0x71
  
  boolean success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  
  const int NO_NFC_READERS = 4;

void nfc_select(uint8_t i) {
  if (i > 7) return;
 
  Wire.beginTransmission(TCA_ADDRESS);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

  
int nfc_init(int channel) {

  nfc_select(channel);

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  //Uncomment below once all readers are implemented
  //It will return an error if it fails
  //if (! versiondata) {
  //  return 0;
  //}

  // Set the max number of retry attempts to read from a card
  // This prevents us from waiting forever for a card, which is
  // the default behaviour of the PN532.
  nfc.setPassiveActivationRetries(0xFF);
  
  // configure board to read RFID tags
  nfc.SAMConfig();
  
  return 1;

}

String get_nfc(int channel) {
	
  nfc_select(channel);
  
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);
  
  if (success) {
    Serial.println("Found a card!");
    Serial.print("UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("UID Value: ");
    String uid_string = "";
    for (uint8_t i=0; i < uidLength; i++) 
    {
      uid_string = String(uid[i]); 
    }
    return uid_string;
  }
  return "";
  
}

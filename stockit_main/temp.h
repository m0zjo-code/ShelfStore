// Create the MCP9808 temperature sensor object
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

int temp_init() {
  // Make sure the sensor is found, you can also pass in a different i2c
  // address with tempsensor.begin(0x19) for example
  if (!tempsensor.begin()) {
    //error
    return 0;
  }
  //ok
  return 1;
}

float get_temp() {

  tempsensor.shutdown_wake(0);   // Wake sensor
  float temp = tempsensor.readTempC();
  tempsensor.shutdown_wake(1); // shutdown MSP9808 - power consumption ~0.1 mikro Ampere
  return temp;
}

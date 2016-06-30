Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */

const int NO_CAL_READ = 256; 
const int MASS_CHANGE_THRESHOLD = 20;

int16_t start_mass = 0;
int16_t previous_mass = 0;

int mass_init(void);
int16_t get_mass(void);

int mass_init(void) 
{
  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
  ads.begin();
  
  unsigned long start_mass_sum = 0;
  //Make a lot of reading here for initial calibration
  for(int i; i <NO_CAL_READ; i++){
  	start_mass_sum += get_mass();
  }
  start_mass = start_mass_sum / NO_CAL_READ;
  return 1;
  //if error make return 0
}

int16_t get_mass(void) 
{
  int16_t mass;
  mass = ads.readADC_SingleEnded(0); //argument is channel for reading
  int16_t adjusted_mass = mass - start_mass;
  return adjusted_mass;
  //scale mass to grams
  
}

boolean mass_changed(void){
	int16_t mass = ads.readADC_SingleEnded(0); //argument is channel for reading
	if(mass - previous_mass > MASS_CHANGE_THRESHOLD || previous_mass - mass > MASS_CHANGE_THRESHOLD){
		return 1;
	}
	return 0;
}

//Drives 4 sets of RGB LEDS using PCA9685

//Use default I2C address of 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

//Define routine for state machine
const int ROUTINE_OFF  =        0;
const int ROUTINE_FLASH =       1;
const int ROUTINE_FADE_DOWN =   2;
const int ROUTINE_FADE_UP =     3;
const int ROUTINE_FADE_ACROSS = 4;
const int ROUTINE_SLEEP =       5;
const int ROUTINE_STEADY =      6;
const int ROUTINE_BREATHE_IN =  7;
const int ROUTINE_BREATHE_OUT = 8;

//Define system parameters
const int NO_OF_RGB =           4;

const long MINOR_ERROR_COLOUR =  0xFF0000;
const long MAJOR_ERROR_COLOUR =  0xFF0000;
const long ITEM_PLACED_COLOUR =  0x00FF00;
const long ITEM_REMOVED_COLOUR =  0xFFFF00;
const long LOCATE_ITEM_COLOUR =  0xFFFFFF;

const int MINOR_ERROR_DURATION = 2400;
const int MAJOR_ERROR_DURATION = 15000;
const int ITEM_PLACED_DURATION = 1000;
const int ITEM_REMOVED_DURATION = 1000;
const int LOCATE_ITEM_DURATION = 3000;

const int MINOR_ERROR_NO_FLASHES =  12;
const int MAJOR_ERROR_NO_FLASHES =  50;
const int LOCATE_ITEM_NO_FLASHES =  30;

const int DECAY_CONSTANT = 4;
const int GAIN_CONSTANT = 4;
const int FADE_DOWN_STEPS = 10;
const int FADE_UP_STEPS = 10;

const int BREATHE_DECAY_CONSTANT = 4;
const int BREATHE_GAIN_CONSTANT = 4;

const int IDLE_OFF = 0;
const int IDLE_STEADY = 1;
const int IDLE_BREATHE = 2;



class rgb{

  //Initialise privite variables
  int state;
  int current_routine;
  int red_channel;
  int green_channel;
  int blue_channel;
  int red_intensity;
  int green_intensity;
  int blue_intensity;
  int period;
  int duration;
  int idle_behaviour;
  unsigned long start_time;
  unsigned long previous_millis;
 

  public:
  //Class constructor: set everything as off
  rgb(){
    state = LOW;
    current_routine = ROUTINE_OFF;
    idle_behaviour = IDLE_OFF;
    red_intensity = 0;
    green_intensity = 0;
    blue_intensity = 0;
    period = 0;
    duration = 0;
    start_time = 0;
    previous_millis = 0;
    }

  void set_flash(long colour, int duration, int no_of_flashes);
  void set_fade_down(long colour, int duration);
  void set_fade_up(long colour, int duration);
  void convert_hex(long colour);
  void initialise(int channel);
  void sleep();
  void update();
  void on();
  void off();

  
};

//instantiate all the rgb leds
rgb led[NO_OF_RGB];


int led_init() {
  pwm.begin();
  pwm.setPWMFreq(1600);  // This is the maximum PWM frequency
  for(int channel = 0; channel < NO_OF_RGB; channel++){
    led[channel].initialise(channel);
  }
  return 1;
}

void rgb::on() {
  state = LOW;
  red_intensity = constrain(red_intensity,0, 4095);
    if (red_intensity == 0) {
      // Special value for signal fully off.
      pwm.setPWM(red_channel, 4096, 0);
    }
    else if (red_intensity == 4095) {
      // Special value for signal fully on.
      pwm.setPWM(red_channel, 0, 4096);
      state = HIGH;
    }
    else {
      pwm.setPWM(red_channel, 0, 4095-red_intensity);
      state = HIGH;
    }
  green_intensity = constrain(green_intensity,0, 4095);
    if (green_intensity == 0) {
      // Special value for signal fully off.
      pwm.setPWM(green_channel, 4096, 0);
    }
    else if (green_intensity == 4095) {
      // Special value for signal fully on.
      pwm.setPWM(green_channel, 0, 4096);
      state = HIGH;
    }
    else {
      pwm.setPWM(green_channel, 0, 4095-green_intensity);
      state = HIGH;
    }
  blue_intensity = constrain(blue_intensity,0, 4095);
    if (blue_intensity == 0) {
      // Special value for signal fully off.
      pwm.setPWM(blue_channel, 4096, 0);
    }
    else if (blue_intensity == 4095) {
      // Special value for signal fully on.
      pwm.setPWM(blue_channel, 0, 4096);
      state = HIGH;
    }
    else {
      pwm.setPWM(blue_channel, 0, 4095-blue_intensity);
      state = HIGH;
    }

}

void rgb::off(){
  pwm.setPWM(red_channel, 4096, 0);
  pwm.setPWM(green_channel, 4096, 0);
  pwm.setPWM(blue_channel, 4096, 0);
  state = LOW;
}

void rgb::initialise(int channel){

  channel *= 4;
  red_channel = channel;
  green_channel = channel+1;
  blue_channel = channel+2;

}

void rgb::sleep(){
  off();
  current_routine = ROUTINE_OFF;
}

void rgb::convert_hex(long colour){
  red_intensity = colour >> 16;
  red_intensity = red_intensity & 0x00FF;
  red_intensity *= 16;
  green_intensity = colour >> 8;
  green_intensity = green_intensity & 0x00FF;
  green_intensity *= 16;
  blue_intensity = colour & 0x00FF;
  blue_intensity *= 16;
}


//Flashes the led attached to this instance of rgb
//Goes between the hex colour selected and off for as long as duration (milliseconds) the number of times specified
//This function should be called through one of the av set commands
void rgb::set_flash(long colour, int duration, int no_of_flashes){
  start_time = millis();
  previous_millis = millis();
  this->convert_hex(colour);
  current_routine = ROUTINE_FLASH;
  period = duration/no_of_flashes;
  this->duration = duration;
}

//Fades the led attached to this instance of rgb from full power down to off
//Goes from the hex colour specified 
void rgb::set_fade_down(long colour, int duration){
  start_time = millis();
  previous_millis = millis();
  this->convert_hex(colour);
  current_routine = ROUTINE_FADE_DOWN;
  period = duration / FADE_DOWN_STEPS;
  this->duration = duration;
}

void rgb::set_fade_up(long colour, int duration){
  start_time = millis();
  previous_millis = millis();
  this->convert_hex(colour);
  for(int i = 0; i<6 ; i++){
  	red_intensity /= GAIN_CONSTANT;
  	blue_intensity /= GAIN_CONSTANT;
  	green_intensity /= GAIN_CONSTANT;
  }
  current_routine = ROUTINE_FADE_UP;
  period = duration / FADE_UP_STEPS;
  this->duration=duration;
}

void rgb::update(){
  unsigned long current_millis = millis();
  if(current_routine == ROUTINE_OFF){
    //do nothing
  }
  else{
    if(current_millis - start_time <= duration){
      if(current_millis - previous_millis >= period){
        switch(current_routine){
          case  ROUTINE_FLASH:
            if(state == LOW){
              previous_millis = current_millis;
              on();
            }
            else if (state == HIGH){
              previous_millis = current_millis;
              off();
            }
          break;
    
          case  ROUTINE_FADE_DOWN:

              previous_millis = current_millis;
              on();
              red_intensity /= DECAY_CONSTANT;  
              green_intensity /= DECAY_CONSTANT;  
              blue_intensity /= DECAY_CONSTANT;   
          break;
  
          case  ROUTINE_FADE_UP:
              previous_millis = current_millis;
              on();
              red_intensity *= GAIN_CONSTANT;  
              green_intensity *= GAIN_CONSTANT;  
              blue_intensity *= GAIN_CONSTANT; 
          //foo
          //bar
  
          break;
  
          case  ROUTINE_FADE_ACROSS:
  
          //foo
          //bar
  
          break;
  
          case  ROUTINE_SLEEP:
          sleep();
          break;
          
          case  ROUTINE_STEADY:
          on();
          current_routine = ROUTINE_OFF;
          break;
          
          case  ROUTINE_BREATHE_OUT:

              previous_millis = current_millis;
              on();
              red_intensity -= BREATHE_DECAY_CONSTANT;  
              green_intensity -= BREATHE_DECAY_CONSTANT;  
              blue_intensity -= BREATHE_DECAY_CONSTANT;   
          break;
  
          case  ROUTINE_BREATHE_IN:
              previous_millis = current_millis;
              on();
              red_intensity += BREATHE_GAIN_CONSTANT;  
              green_intensity += BREATHE_GAIN_CONSTANT;  
              blue_intensity += BREATHE_GAIN_CONSTANT; 
          //foo
          //bar
  
          default:
          break;
          
        }
      }
    }
    else{
      switch(idle_behaviour){
      
      	case IDLE_OFF:
      		current_routine = ROUTINE_SLEEP;
      	break;
      	
      	case IDLE_STEADY:
      		current_routine = ROUTINE_STEADY;
      	break;
      	
      	case IDLE_BREATHE:
      		if(current_routine = ROUTINE_BREATHE_OUT){
      			current_routine = ROUTINE_BREATHE_IN;
      		}
      		else{
      			current_routine = ROUTINE_BREATHE_OUT;
      		}
      	
      }
      
    }
  }
}



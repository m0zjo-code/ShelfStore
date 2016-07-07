/*
stock.it - Code by Rob Arrow 2016
Smartshelf firmware
*/
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_ADS1015.h>
#include <Adafruit_MCP9808.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>
//#include <Tone.h>
//#include <ESP8266WiFi.h>
//#include <WiFiClient.h>
//#include <ESP8266WebServer.h>
//#include <ESP8266mDNS.h>
//#include <ESP8266HTTPUpdateServer.h>

#include "led.h"
#include "mass.h"
#include "nfc.h"
#include "temp.h"
#include "serial_uplink.h"
//#include "uplink.h"
//#include "downlink.h"
#include "av.h"



#define BUILTIN_LED 2


//ESP8266WebServer httpServer(80);
//ESP8266HTTPUpdateServer httpUpdater;

const char* host = "esp8266-webupdate";
const char* ssid = "Stock-It-Server";
const char* password = "stockit123";

const char* dest = "192.168.0.100";
const int port = 8950;

const int MAX_REPORT_INTERVAL = 5000;
//const int SHELF_ID = 43345;
//const int HK_PACKET_ID = 05;
//const int REG_PACKET_ID = 06;

String packet = "";
String uid_string = "";

unsigned long last_report = 0;

void setup()
{
  Serial.begin(115200);
  Wire.begin(4,5);
  Wire.setClockStretchLimit(2000);
	if(!led_init())
	{
		//led initialise error;
	}
	if(!temp_init())
	{
		//temp initialise error;
	}
	if(!mass_init()){
		//mass initialise error;
	}
	if(!nfc_init(1)){
		//nfc initialise error;
	}
 /*
	if(!wifi_init()){
    //wifi initialise error;
  }
  */
  /*
	for(int i = 0; i < NO_NFC_READERS; i++){
		if(!nfc_init(i)){
			Serial.print("NFC ");
      Serial.print(i);
      Serial.println("  Error!");
		}
	}
 */

  nfc_init(1);

	
	//av_minor_error(1);
	//av_item_placed(3);

	
}

void loop()
{
	//httpServer.handleClient();
	av_update();
	//led_wave();
	if(mass_changed() || millis() - last_report> MAX_REPORT_INTERVAL){
		last_report = millis();
		serial_transmit();
	}
  //Serial.println(millis());

}

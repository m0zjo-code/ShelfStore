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
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>
  


#include "led.h"
#include "mass.h"
#include "nfc.h"
#include "temp.h"
#include "uplink.h"
#include "downlink.h"
#include "av.h"



#define BUILTIN_LED 2


ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

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
#ifdef ESP8266
	Wire.pins(2, 14);   // ESP8266 can use any two pins, such as SDA to #2 and SCL to #14
#endif

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
	for(int i = 0; i < NO_NFC_READERS; i++){
		if(!nfc_init(i)){
			//nfc initialise error;
		}
	}
	*/
	Serial.begin(115200);
	
	av_minor_error(1);
	av_item_placed(3);

	//Encapsulate the below :)	
  Serial.println();
  Serial.println("Booting Sketch...");
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);

  while(WiFi.waitForConnectResult() != WL_CONNECTED){
    WiFi.begin(ssid, password);
    Serial.println("WiFi failed, retrying.");
  }

  MDNS.begin(host);

  httpUpdater.setup(&httpServer);
  httpServer.begin();

  MDNS.addService("http", "tcp", 80);
  Serial.printf("HTTPUpdateServer ready! Open http://%s.local/update in your browser\n", host);
  Serial.print("My IP is: ");
  Serial.println(WiFi.localIP());


	//End of the wifi init
	
}

void loop()
{
	httpServer.handleClient();
	av_update();
	//led_wave();
	if(mass_changed() || millis() - last_report> MAX_REPORT_INTERVAL){
		last_report = millis();
		transmit();
	}
	Serial.print("Temp: ");
  Serial.println(get_temp());



}

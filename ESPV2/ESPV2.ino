/*
  To upload through terminal you can use: curl -F "image=@firmware.bin" esp8266-webupdate.local/update
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>

const char* host = "esp8266-webupdate";
const char* ssid = "Stock-It-Server";
const char* password = "stockit123";

const char* dest = "192.168.0.100";
const int port = 8950;

#define BUILTIN_LED 2

unsigned long previousMillis = 0;
unsigned long previousMillis2 = 0;
const long interval = 1000;
const long interval2 = 9000;

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

void setup(void){

  Serial.begin(115200);
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
}

int xmit_Data(String data){

  Serial.print("Connecting to : ");
  Serial.println(dest);

  WiFiClient client;

  if (!client.connect(dest, port)){
    Serial.println("TCP Failed");
    return 99;
  }

  client.print(data);
  String req = client.readStringUntil(64);
  Serial.println(req[0]); 
  if (req[0] == 'L'){
    Serial.println("LED CHANGE");
    bool val = digitalRead(BUILTIN_LED);
    digitalWrite(BUILTIN_LED, !val);
  }

  return 0;
}

void loop(void){
  httpServer.handleClient();
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;   
    xmit_Data("05:43345:21.432:123.33"); //PktIdent:ShelfID:Temp:MassReading
    bool val = digitalRead(BUILTIN_LED);
    digitalWrite(BUILTIN_LED, !val);
  }

  if(currentMillis - previousMillis2 >= interval2) {
    previousMillis2 = currentMillis;   
    xmit_Data("06:43345:5:765237"); //PktIdent:ShelfID:Location:TadID
    bool val = digitalRead(BUILTIN_LED);
    digitalWrite(BUILTIN_LED, !val);
    Serial.println("Sent NFC Pkt");
  }
  
  
}



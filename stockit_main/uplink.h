const String HK_PACKET_ID  = "05";
const String REG_PACKET_ID = "06";
const String SHELF_ID = "76346";
int xmit_Data(String data);
String  construct_housekeeping_packet(){
	String packet = "";
	packet += String(HK_PACKET_ID);
	packet += String(':');
	packet += String(SHELF_ID);
	packet += String(':');
	packet += String(get_temp(), 2);
	packet += String(':');
  Serial.print("Temp: ");
  Serial.println(get_temp());
  Serial.print("Mass: ");
  Serial.println(get_mass());
	packet += String(get_mass());
  return packet;
}

String  construct_regular_packet(){
	String packet = "";
	packet += String(REG_PACKET_ID);
	packet += String(':');
	packet += String(SHELF_ID);

	for(int i = 0; i < NO_NFC_READERS; i++){
		packet += String(':');
		packet += String(i);
		packet += String(':');
		packet += get_nfc(i);
	}
  return packet;
}

void wifi_init(){
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

  return 1;
}

void transmit(){
	
	String packet = construct_housekeeping_packet();
	//xmit_Data("05:43345:21.432:123.33"); //PktIdent:ShelfID:Temp:MassReading
	xmit_Data(packet);
	
	packet = construct_regular_packet();
	//xmit_Data("06:43345:5:765237"); //PktIdent:ShelfID:Location:TadID
	xmit_Data(packet);
	
	//Check downlink crap and pass to av handler
}

/*
  To upload through terminal you can use: curl -F "image=@firmware.bin" esp8266-webupdate.local/update
*/


int xmit_Data(String data){
  const char* host = "esp8266-webupdate";
  const char* ssid = "Stock-It-Server";
  const char* password = "stockit123";

  const char* dest = "192.168.0.100";
  const int port = 8950;
  Serial.print("Connecting to : ");
  Serial.println(dest);

  WiFiClient client;

  if (!client.connect(dest, port)){
    Serial.println("TCP Failed");
    return 99;
  }

  client.print(data);
  String reply = client.readStringUntil(64);
  Serial.println(reply[0]); 
  if (reply[0] == 'L'){
    //return pointer and length of reply
  }

  return 0;
}




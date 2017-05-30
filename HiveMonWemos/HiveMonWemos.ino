/*
 Name:		HiveMonWemos.ino
 Author:	Mario Nitchev
*/

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>

#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

const char* ssid = "Onetouch Idol 3 (5.5)";
const char* password = "12345678";

ESP8266WebServer server(80);

const char* centralServiceUsername = "admin";
const char* centralServicePassword = "esp8266";

const char LOCK = '0';
const char UNLOCK = '1';
const char GET_DATA = '2';

void setup() {
	Serial.begin(9600);
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
	if (WiFi.waitForConnectResult() != WL_CONNECTED) {
		delay(1000);
		ESP.restart();
	}
	ArduinoOTA.begin();

	server.on("/sensors", handleCollectDataRequest);
	server.on("/lock", handleLockRequest);
	server.on("/unlock", handleUnlockRequest);
  server.on("/update", handleUpdateSoftware);
	server.begin();
}

void checkAuthentication() {
	if (!server.authenticate(centralServiceUsername, centralServicePassword)) {
		return server.requestAuthentication();
	}
}

void handleCollectDataRequest() {
	if (!server.authenticate(centralServiceUsername, centralServicePassword)) {
		return server.requestAuthentication();
	}
  Serial.print(GET_DATA);
	String jsonResponse = Serial.readStringUntil('\0');
	server.send(200, "application/json", jsonResponse);
}

void handleLockRequest() {
	Serial.print(LOCK);
	server.send(200);
}

void handleUnlockRequest() {
	Serial.print(UNLOCK);
	server.send(200);
}

void handleUpdateSoftware(){
  t_httpUpdate_return ret = ESPhttpUpdate.update("http://192.168.43.100/public/HiveMonWemos.bin");
  String errorCode;
        switch(ret) {
            case HTTP_UPDATE_FAILED:
                errorCode = String("HTTP_UPDATE_FAILD Error " 
                        + String(ESPhttpUpdate.getLastError()) 
                        + " : " 
                        + ESPhttpUpdate.getLastErrorString());
                server.send(400, "text/plain", errorCode);
                break;

            case HTTP_UPDATE_NO_UPDATES:
                server.send(404);
                break;

            case HTTP_UPDATE_OK:
                server.send(200);
                break;
        }
}

void loop() {
	ArduinoOTA.handle();
	server.handleClient();
}

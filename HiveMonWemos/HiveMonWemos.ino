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

const char* ssid = "Linksys22621";
const char* password = "qgdrq7w0mm";

ESP8266WebServer server(80);

const char* centralServiceUsername = "admin";
const char* centralServicePassword = "esp8266";

const byte LOCK = 0;
const byte UNLOCK = 1;
const byte GET_DATA = 2;


void setup() {
	Serial.begin(115200);
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
	if (WiFi.waitForConnectResult() != WL_CONNECTED) {
		Serial.println("WiFi Connect Failed! Rebooting...");
		delay(1000);
		ESP.restart();
	}
	ArduinoOTA.begin();

	server.on("/sensors", handleCollectDataRequest);
	server.on("/lock", handleLockRequest);
	server.on("/unlock", handleUnlockRequest);
	server.begin();

	Serial.print("Open http://");
	Serial.print(WiFi.localIP());
	Serial.println("/ in your browser to see it working");
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
	byte command = GET_DATA;
	Serial.print(command);
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

void loop() {
	ArduinoOTA.handle();
	server.handleClient();
}

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK  "your-password"
#endif

const char* ssid = "yourssidhere";
const char* password = "yourpasswordhere";

ESP8266WebServer server(80);

#define relayPin 5
#define relayPin2 4
#define lightPin 3
#define alarmPin 12

const char *webpage =
#include "webPage.h"
;

void handleRoot() {
  //digitalWrite(lightPin, 1);
  server.send(200, "text/html", webpage);
  //digitalWrite(lightPin, 0);
}

void handleNotFound() {
  digitalWrite(lightPin, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(lightPin, 0);
}

void setup(void) {
  pinMode(lightPin, OUTPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  pinMode(alarmPin, OUTPUT);
  digitalWrite(lightPin, LOW);
  digitalWrite(relayPin, HIGH);
  digitalWrite(relayPin2, HIGH);
  digitalWrite(alarmPin, LOW);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  int alarmVar = 0;  //Initialize the alarmVar variable for the alarm loop

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("garage")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/maindoor", []() {

    digitalWrite(alarmPin, HIGH);
    delay(1000);
    digitalWrite(alarmPin, LOW);
    alarmVar = 0;
    while(alarmVar < 3){
      // play alarm
      alarmVar++;
      digitalWrite(alarmPin, HIGH);
      delay(500);
      digitalWrite(alarmPin, LOW);
      delay(500);
    }
    alarmVar = 0;
    while(alarmVar < 6){
      // play alarm
      alarmVar++;
      digitalWrite(alarmPin, HIGH);
      delay(100);
      digitalWrite(alarmPin, LOW);
      delay(100);
    }    
     
    //the code to actually open the door 
    digitalWrite(relayPin, LOW);
    delay(250);
    digitalWrite(relayPin, HIGH);
    delay(50); //may not be needed
    server.send(200, "text/plain", "maindoor");
  });
  
  server.on("/smalldoor", []() {

    digitalWrite(alarmPin, HIGH);
    delay(1000);
    digitalWrite(alarmPin, LOW);
    alarmVar = 0;
    while(alarmVar < 3){
      // play alarm
      alarmVar++;
      digitalWrite(alarmPin, HIGH);
      delay(500);
      digitalWrite(alarmPin, LOW);
      delay(500);
    }
    alarmVar = 0;
    while(alarmVar < 6){
      // play alarm
      alarmVar++;
      digitalWrite(alarmPin, HIGH);
      delay(100);
      digitalWrite(alarmPin, LOW);
      delay(100);
    }    
     
    //the code to actually open the door 
    digitalWrite(relayPin2, LOW);
    delay(250);
    digitalWrite(relayPin2, HIGH);
    delay(50); //may not be needed
    server.send(200, "text/plain", "smalldoor");
  });

  server.on("/lightOn", []() {
    digitalWrite(lightPin, HIGH);
    server.send(200, "text/plain", "lightOn");
  });

  server.on("/lightOff", []() {
    digitalWrite(lightPin, LOW);
    server.send(200, "text/plain", "lightOff");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

}

void loop() {
  
  // NOTIFICATION LIGHT
  if (millis()%30000==0) { // every 30 seconds
    digitalWrite(notificationPin1, HIGH);
    delay(250);
    digitalWrite(notificationPin1, LOW);
    delay(200);
    digitalWrite(notificationPin1, HIGH);
    delay(250);
    digitalWrite(notificationPin1, LOW);
  }
  
  server.handleClient();
  MDNS.update();
}

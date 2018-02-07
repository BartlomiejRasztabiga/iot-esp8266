#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "WLANConfig.h"

#define TOKEN "tFjV2SeIJiWfe5psqrss"

char thingsboardServer[] = "35.158.167.56";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

int status = WL_IDLE_STATUS;
unsigned long lastSend;

void sendData();
void reconnect();
void setupWlan();

void setup() {
  Serial.begin(115200);
  delay(10);
  setupWlan();

  client.setServer(thingsboardServer, 1883);
  lastSend = 0;
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  if (millis() - lastSend > 5000) { // Update every 5 sec
    sendData();
    lastSend = millis();
  }

  client.loop();
}

void sendData() {
  IPAddress me (192, 168, 1, 147); // xiaomi redmi note 3
  IPAddress mother (192, 168, 1, 197); // mother
  IPAddress father (192, 168, 1, 177); // father

  // Prepare a JSON payload string
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["BR"] = Ping.ping(me);
  root["MR"] = Ping.ping(mother);
  root["TR"] = Ping.ping(father);


  String payload;
  root.printTo(payload);

  // Send payload
  char attributes[100];
  payload.toCharArray(attributes, 100);
  client.publish("v1/devices/me/telemetry", attributes);
}

void reconnect() {
  while (!client.connected()) {
    if (!client.connect("ESP8266 Device", TOKEN, NULL)) {
      delay(5000); // wait and try again
    }
  }
}

void setupWlan() {
  Serial.print("Connecting to ");
  Serial.println(WLANConfig::ssid);

  WiFi.mode(WIFI_STA); //We don't want the ESP to act as an AP
  WiFi.begin(WLANConfig::ssid, WLANConfig::password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

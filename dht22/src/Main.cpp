#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <Adafruit_Sensor.h>
#include <PubSubClient.h>
#include <DHT.h>

#include "WLANConfig.h"

#define TOKEN "EzA4JcD1wjaKaFsmllhU"
#define DHTPIN 5
#define DHTTYPE DHT22

char thingsboardServer[] = "35.158.167.56";

DHT dht(DHTPIN, DHTTYPE);

WiFiClient wifiClient;
PubSubClient client(wifiClient);

int status = WL_IDLE_STATUS;
unsigned long lastSend;

void sendData();
void reconnect();
void setupWlan();

void setup() {
  Serial.begin(115200);
  dht.begin();
  delay(10);
  setupWlan();

  client.setServer( thingsboardServer, 1883 );
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
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    return; // invalid data
  }

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" *C ");

  // Prepare a JSON payload string
  String payload = "{";
  payload += "\"temperature\":"; payload += temperature; payload += ",";
  payload += "\"humidity\":"; payload += humidity;
  payload += "}";

  // Send payload
  char attributes[100];
  payload.toCharArray(attributes, 100);
  client.publish("v1/devices/me/telemetry", attributes);
}

void reconnect() {
  while (!client.connected()) {
    status = WiFi.status();
    if (status != WL_CONNECTED) {
      setupWlan();
    }
    if (client.connect("ESP8266 Device", TOKEN, NULL)) {
      // connected
    } else {
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

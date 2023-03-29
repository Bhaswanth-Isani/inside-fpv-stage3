#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <RFID.h>

int SS_PIN = 4;   // D2
int RST_PIN = 5;  // D1

RFID rfid(SS_PIN, RST_PIN);
String rfidcard;

const char* ssid = "Venky";          //Your network SSID
const char* password = "987654321";  //Your Network password

String serverName = "https://inside-fpv-server-production.up.railway.app";

void setup() {
  Serial.begin(9600);
  Serial.println("Starting the RFID Reader...");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
  }
  Serial.println("Connected to Wi-Fi");

  SPI.begin();
  rfid.init();
}

void loop() {
  if (rfid.isCard()) {
    if (rfid.readCardSerial()) {
      rfidcard = String(rfid.serNum[0]) + " " + String(rfid.serNum[1]) + " " + String(rfid.serNum[2]) + " " + String(rfid.serNum[3]);
      Serial.println("RFID detected: " + rfidcard);
    }
    rfid.halt();
    delay(100);


    WiFiClientSecure client;
    HTTPClient http;
    client.setFingerprint("7A 19 06 B0 69 72 A3 77 A4 92 FE A2 29 59 D2 87 76 F7 77 FE");
    client.setTimeout(15000);

    client.connect(serverName, 443); 
      Serial.println("Connected to server");
      String postJson = "{\"rfid\":\"" + rfidcard + "\"}";
      http.begin(client, serverName + "/stage3");
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST(postJson);
      Serial.println("HTTP response code: " + String(httpResponseCode));
      if (httpResponseCode > 0) {
        Serial.println("HTTP response code: " + String(httpResponseCode));
        String response = http.getString();
        Serial.println("Response: " + response);
      } else {
        Serial.println("Error on HTTP request");
      }
      http.end();
  

    delay(1000);
  }
}
/*
  Complete project details: https://RandomNerdTutorials.com/esp8266-nodemcu-https-requests/ 
  Based on the BasicHTTPSClient.ino Created on: 20.08.2018 (ESP8266 examples)
*/

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

#define LED D0  

// Replace with your network credentials
const char* ssid = "<WIFI_NAME>";
const char* password = "<WIFI_PASSWORD>";

const char* AZURE_FUNCTION_URL = "<AZURE_FUNCTION_URL>";
const int RETRY_TIMEOUT_MS = 3000;

void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  Serial.begin(115200);

  Serial.println();
  Serial.println();
  Serial.println();

  //Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
}

void loop() {
  // wait for WiFi connection
  if ((WiFi.status() == WL_CONNECTED)) {

    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    // Ignore SSL certificate validation
    client->setInsecure();
    
    //create an HTTPClient instance
    HTTPClient https;
    
    //Initializing an HTTPS communication using the secure client
    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, AZURE_FUNCTION_URL)) {  // HTTPS
      Serial.print("[HTTPS] GET...\n");
      https.addHeader("Content-Type", "application/json");
      // start connection and send HTTP header
      int httpCode = https.POST("{\"token\":\"<JWT_TOKEN>\",\"applianceId\":\"<APPLIANCE_ID>\"}");
      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();

          if(payload == "1") {
            digitalWrite(LED, LOW);
          }

          if(payload == "0") {
            digitalWrite(LED, HIGH);
          }
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }

      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }
  Serial.println();
  Serial.println("Waiting before the next round...");
  delay(RETRY_TIMEOUT_MS);
}

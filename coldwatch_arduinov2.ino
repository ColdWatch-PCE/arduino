#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2  // Change pin if necessary

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

const char* ssid = "Casa";
const char* password = "girassol897";
const char* host = "api.thingspeak.com";
const char* api_key = "M1NN8ZTWIDRGCPWO";

void setup() {
  Serial.begin(115200);
  sensors.begin();

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);

  if (isnan(tempC)) {
    Serial.println("Failed to read temperature");
    return;
  }

  Serial.print("Temperature: ");
  Serial.println(tempC);

  WiFiClient client;
  if (!client.connect(host, 80)) {
    Serial.println("Connection to ThingSpeak failed");
    return;
  }

  String url = "/update?api_key=" + String(api_key) + "&field1=" + String(tempC);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("Data sent to ThingSpeak");

  // Wait for response and print it
  while (client.connected() || client.available()) {
    if (client.available()) {
      String line = client.readStringUntil('\n');
      Serial.println(line);
    }
  }

  client.stop();
  Serial.println("Connection closed");

  delay(60000); // Wait before sending the next reading
}

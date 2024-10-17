#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <WiFiClient.h>

// WiFi credentials
const char* ssid = "Casa";
const char* senha = "girassol897";

// ThingSpeak settings
const char *host = "api.thingspeak.com";
const int httpPort = 80; 
const String channelID = "2479418";
const String writeApiKey = "M1NN8ZTWIDRGCPWO";

// Sensor setup
OneWire oneWire(4); 
DallasTemperature Sensor(&oneWire);
float leitura; 

// Function to connect to WiFi (with reconnection attempts)
void connectToWiFi() {
  Serial.print("Connecting to WiFi ");
  WiFi.begin(ssid, senha);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// Function to send data to ThingSpeak
void sendDataToThingSpeak(float temperature) {
  // Ensure WiFi connection before sending data
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi(); 
  }

  WiFiClient client;
  if (!client.connect(host, httpPort)) {
    Serial.println("Connection to ThingSpeak failed!");
    return;
  }

  String url = "/update?api_key=";
  url += writeApiKey;
  url += "&field1=";
  url += temperature; 

  client.print("GET " + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  while (client.connected()) {
    String line = client.readStringUntil('\n');
    // Serial.println(line); // Uncomment for debugging 
  }

  client.stop();
}

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(100); } 
  connectToWiFi();
  Sensor.begin(); 
}

void loop() {
  Sensor.requestTemperatures(); 
  leitura = Sensor.getTempCByIndex(0);

  Serial.print("Temperature: ");
  Serial.print(leitura);
  Serial.println(" C");

  sendDataToThingSpeak(leitura); 

  delay(20000); // Wait for 20 seconds before the next reading
}

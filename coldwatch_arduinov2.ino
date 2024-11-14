#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <WiFiClient.h>

#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27 ,16,2);

// WiFi credentials
const char* ssid = "iPhone de Victor";
const char* senha = "21212121";

// ThingSpeak settings
const char *host = "api.thingspeak.com";
const int httpPort = 80; 
const String channelID = "2743925";
const String writeApiKey = "UQYX4V3U494422TX";

// Sensor setup
OneWire oneWire(4); 
DallasTemperature Sensor(&oneWire);
float leitura; 

// Function to send data to ThingSpeak
void sendDataToThingSpeak(float temperature) {
  // Connect to WiFi
  WiFiClient client;
  if (!client.connect(host, httpPort)) {
    Serial.println("Connection to ThingSpeak failed!");
    return;
  }

  // Construct the HTTP request
  String url = "/update?api_key=";
  url += writeApiKey;
  url += "&field1=";
  url += temperature; 

  // Send the HTTP request
  client.print("GET " + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  // Wait for response and print it (for debugging)
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    Serial.println(line); 
  }

  // Close connection
  client.stop();
}

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(100); } 

  lcd.init(); 
  lcd.clear(); 
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Sensor de Temp");

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, senha);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Initialize temperature sensor
  Sensor.begin(); 
}

void loop() {
  Sensor.requestTemperatures(); 
  leitura = Sensor.getTempCByIndex(0);

  if(leitura != DEVICE_DISCONNECTED_C){
    Serial.print("Temperature: ");
    Serial.print(leitura);
    Serial.println(" C");
    sendDataToThingSpeak(leitura); 

    lcd.setCursor(0, 1);
    lcd.print("temp: ");
    lcd.print(leitura, 1);
    lcd.print(" C");
  }
  else{
    Serial.println("Error: Could not read temperature data");
    
    lcd.setCursor(0, 1);
    lcd.print("ERRO AO LER TEMP");
  }


  delay(5000); // Wait for 20 seconds before the next reading 
}

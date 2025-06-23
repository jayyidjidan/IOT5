#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* apiKey = "7f63b60682c92e64b5d13822c92852f0";
const char* city = "Malang";
const char* endpoint = "http://api.openweathermap.org/data/2.5/weather";

LiquidCrystal_I2C lcd(0x27, 16, 2);
unsigned long lastUpdate = 0;
const long interval = 60000;

// ✅ Deklarasi fungsi sebelum digunakan
void getWeatherData();

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  WiFi.begin(ssid, password);
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected");
  delay(1000);
}

void loop() {
  if (millis() - lastUpdate > interval || lastUpdate == 0) {
    getWeatherData();
    lastUpdate = millis();
  }
}

// ✅ Definisi fungsi di bawah
void getWeatherData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(endpoint) + "?q=" + city + "&appid=" + apiKey + "&units=metric";
    http.begin(url);

    int httpCode = http.GET();
    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println(payload);

      DynamicJsonDocument doc(1024);
      DeserializationError error = deserializeJson(doc, payload);

      if (!error) {
        float temp = doc["main"]["temp"];
        const char* desc = doc["weather"][0]["description"];

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Temp: " + String(temp) + "C");
        lcd.setCursor(0, 1);
        lcd.print(desc);
      } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Parse Error");
      }
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("HTTP Error");
    }

    http.end();
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("No WiFi");
  }
}

#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT22.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#define DHTPIN 4
#define DHT_POWER_PIN 2
#define VBATT_PIN 9
#define DHTTYPE DHT22

#define WIFI_SSID "Calcetin con rombos man"
#define WIFI_PASSWORD "dinosaurioanacleto"
#define API_ENDPOINT "https://1ba1-2600-1700-65af-4810-bda1-54b5-ccf6-3dcb.ngrok-free.app"

IPAddress local_IP(192, 168, 1, 108);
IPAddress dns(8, 8, 8, 8);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

DHT22 dht22(DHTPIN);

// --- Forward declarations
void connectToWiFi();
bool readSensorData(float &temperature, float &humidity);
float readBatteryVoltage();
void sendData(float temperature, float humidity, float voltage);
void shutdownAndSleep();

void setup() {
    Serial.begin(115200);
    delay(2500);
    Serial.println("\n[INFO] Starting ESP32-S2 Mini...");

    pinMode(DHTPIN, INPUT);
    pinMode(DHT_POWER_PIN, OUTPUT);

    connectToWiFi();

    if (WiFi.status() == WL_CONNECTED) {
        float temperature, humidity;
        if (readSensorData(temperature, humidity)) {
            float voltage = readBatteryVoltage();
            sendData(temperature, humidity, voltage);
        }
    } else {
        Serial.println("[ERROR] Skipping sensor read due to failed Wi-Fi.");
    }

    shutdownAndSleep();
}

void loop() {
    // Empty, ESP32 stays in deep sleep
}

// --- Wi-Fi Setup
void connectToWiFi() {
    Serial.println("[INFO] Connecting to Wi-Fi...");
    WiFi.config(local_IP, gateway, subnet, dns);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 10) {
        delay(500);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n[INFO] Wi-Fi Connected.");
        Serial.print("[INFO] IP Address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\n[ERROR] Wi-Fi Connection Failed.");
    }
}

// --- DHT22 Sensor Read
bool readSensorData(float &temperature, float &humidity) {
    digitalWrite(DHT_POWER_PIN, HIGH);
    delay(2000);  // Let sensor stabilize

    Serial.println("[INFO] Reading DHT22 sensor...");
    temperature = dht22.getTemperature();
    humidity = dht22.getHumidity();

    if (!isnan(temperature) && !isnan(humidity)) {
        Serial.printf("[INFO] Temperature: %.2f °C | Humidity: %.2f %%\n", temperature, humidity);
        return true;
    } else {
        Serial.println("[ERROR] Failed to read from DHT22 sensor.");
        return false;
    }
}

// --- Battery Voltage Measurement
float readBatteryVoltage() {
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);
    int raw = analogRead(VBATT_PIN);
    float voltage = (raw / 4095.0) * 3.3 * 2.0;
    Serial.printf("[INFO] Battery voltage: %.2f V\n", voltage);
    return voltage;
}

// --- HTTP Data Sending
void sendData(float temperature, float humidity, float voltage) {
    Serial.println("[INFO] Sending data to API...");

    HTTPClient http;
    http.setTimeout(5000);
    http.begin(API_ENDPOINT);
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"temperature\":" + String(temperature) +
                     ",\"humidity\":" + String(humidity) +
                     ",\"voltage\":" + String(voltage, 2) + "}";

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
        Serial.printf("[INFO] Data sent successfully. HTTP code: %d\n", httpResponseCode);
    } else {
        Serial.printf("[ERROR] Failed to send data. HTTP code: %d\n", httpResponseCode);
    }

    http.end();
}

// --- Cleanup & Deep Sleep
void shutdownAndSleep() {
    digitalWrite(DHT_POWER_PIN, LOW);
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    Serial.println("[INFO] Entering deep sleep for 20 minutes...");
    esp_sleep_enable_timer_wakeup(20 * 60 * 1000000);
    esp_deep_sleep_start();
}


#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

// Replace with your network credentials and Telegram bot details
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* botToken = "YOUR_BOT_TOKEN";
const char* chatId = "YOUR_CHAT_ID";

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  scanNetworks();
}

void loop() {
  delay(30000); // Scan every 30 seconds
  scanNetworks();
}

void scanNetworks() {
  Serial.println("🔍 Scanning for nearby devices...");
  int n = WiFi.scanNetworks();
  for (int i = 0; i < n; ++i) {
    String ssid = WiFi.SSID(i);
    String bssid = WiFi.BSSIDstr(i);
    int rssi = WiFi.RSSI(i);

    if (rssi > -70 && !isKnownDevice(bssid)) {
      Serial.printf("Suspicious device: %s (RSSI: %d)
", bssid.c_str(), rssi);
      sendTelegramAlert(bssid, rssi);
    }
  }
  WiFi.scanDelete();
}

bool isKnownDevice(String mac) {
  // Add known MAC addresses to whitelist here
  return false;
}

void sendTelegramAlert(String mac, int rssi) {
  WiFiClientSecure client;
  client.setInsecure(); // Disable SSL verification

  String message = "Suspicious Drone Detected!\nMAC: " + mac + "\nRSSI: " + String(rssi);
  String url = "https://api.telegram.org/bot" + String(botToken) + "/sendMessage?chat_id=" + String(chatId) + "&text=" + message;

  if (!client.connect("api.telegram.org", 443)) {
    Serial.println("Connection to Telegram failed.");
    return;
  }

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: api.telegram.org\r\n" +
               "Connection: close\r\n\r\n");
  delay(500);
  while (client.available()) {
    String line = client.readStringUntil('\n');
    Serial.println(line);
  }
  client.stop();
}

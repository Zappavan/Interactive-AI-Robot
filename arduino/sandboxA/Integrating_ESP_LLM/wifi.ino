#include <WiFi.h>

// --- WiFi config ---
const char* ssid     = "JASUS";
const char* password = "jasus2000";

WiFiServer server(12345);

// --- Incoming message buffer ---
String incoming = "";
String currentMood = "neutral";
String currentText = "";

// --- Setup WiFi ---
void setupWiFi() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

// --- Listener loop ---
void wifiLoop() {
  WiFiClient client = server.available();
  if (client) {
    while (client.connected()) {
      while (client.available()) {
        char c = client.read();
        if (c == '\n') {
          // Full line received -> process
          processMessage(incoming);
          incoming = "";
        } else {
          incoming += c;
        }
      }
      delay(10);
    }
    client.stop();
  }
}

// --- Process LLM message ---
void processMessage(String msg) {
  int sep = msg.indexOf('|');
  if (sep > 0) {
    currentMood = msg.substring(0, sep);
    currentText = msg.substring(sep + 1);
    Serial.print("Received mood: ");
    Serial.println(currentMood);
    Serial.print("Received text: ");
    Serial.println(currentText);
  }
}

// --- Run expression + display ---
void runMood() {
  if (currentMood == "happy") {
    happyState(3000);
  } else if (currentMood == "excited") {
    excitedState(3000);
  } else if (currentMood == "sad") {
    sadState(3000);
  } else if (currentMood == "angry") {
    angryState(3000);
  } else {
    neutralState(3000);
  }
  // After mood, return to neutral
  neutralState(2000);
}

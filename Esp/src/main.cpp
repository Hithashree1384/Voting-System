#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_Fingerprint.h>
#include <HardwareSerial.h>
#include <WebServer.h>

const char* ssid = "Xiaomi";
const char* password = "Hitha@13";
const char* backendUrl = "http://192.168.121.56:3000/vote";  // Update if needed

HardwareSerial mySerial(2);  // GPIO16 (RX), GPIO17 (TX)
Adafruit_Fingerprint finger(&mySerial);
WebServer server(80);

bool hasVoted[128] = {false};
String voterIdMap[128];  // fingerprintID -> voter ID

void sendVoteToServer(String voterId) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(backendUrl);
    http.addHeader("Content-Type", "application/json");
    String payload = "{\"voterId\":\"" + voterId + "\"}";

    int code = http.POST(payload);
    Serial.println("📨 Vote sent. Response code: " + String(code));
    http.end();
  } else {
    Serial.println("❌ Not connected to Wi-Fi.");
  }
}

bool enrollFingerprint(int id) {
  int p;
  Serial.println("🖐️ Place finger for first scan...");
  while ((p = finger.getImage()) != FINGERPRINT_OK);
  if (finger.image2Tz(1) != FINGERPRINT_OK) return false;

  Serial.println("✋ Remove finger...");
  delay(2000);
  while ((p = finger.getImage()) != FINGERPRINT_NOFINGER);

  Serial.println("🖐️ Place same finger again...");
  while ((p = finger.getImage()) != FINGERPRINT_OK);
  if (finger.image2Tz(2) != FINGERPRINT_OK) return false;

  if (finger.createModel() != FINGERPRINT_OK) return false;
  return finger.storeModel(id) == FINGERPRINT_OK;
}

bool matchFingerprint(int &fingerprintID) {
  if (finger.getImage() != FINGERPRINT_OK) return false;
  if (finger.image2Tz() != FINGERPRINT_OK) return false;
  if (finger.fingerSearch() != FINGERPRINT_OK) return false;
  fingerprintID = finger.fingerID;
  return true;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.begin(ssid, password);
  Serial.print("🌐 Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\n✅ Wi-Fi connected!");
  Serial.print("📡 IP: "); Serial.println(WiFi.localIP());

  mySerial.begin(57600, SERIAL_8N1, 16, 17);
  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("✅ Fingerprint sensor ready.");
  } else {
    Serial.println("❌ Fingerprint sensor not detected.");
    while (1) delay(1);
  }

  // ----------- Enroll Fingerprint ----------
  server.on("/enroll", HTTP_GET, []() {
    if (!server.hasArg("voter_id")) {
      server.send(400, "application/json", "{\"error\":\"Missing voter_id\"}");
      return;
    }
    String voterId = server.arg("voter_id");

    int slot = -1;
    for (int i = 1; i < 128; i++) {
      if (voterIdMap[i] == "") { slot = i; break; }
    }

    if (slot == -1) {
      server.send(500, "application/json", "{\"error\":\"Fingerprint DB full\"}");
      return;
    }

    if (enrollFingerprint(slot)) {
      voterIdMap[slot] = voterId;
      server.send(200, "application/json", "{\"message\":\"Fingerprint enrolled\"}");
      Serial.println("✅ Enrolled Voter ID " + voterId + " as template #" + String(slot));
    } else {
      server.send(500, "application/json", "{\"error\":\"Enrollment failed\"}");
    }
  });

  // ----------- First Scan (Match Fingerprint) ----------
  server.on("/match", HTTP_GET, []() {
    int fid;
    if (matchFingerprint(fid)) {
      if (voterIdMap[fid] != "") {
        server.send(200, "application/json", "{\"voterId\":\"" + voterIdMap[fid] + "\"}");
        Serial.println("✅ Fingerprint matched. Voter ID: " + voterIdMap[fid]);
      } else {
        server.send(404, "application/json", "{\"error\":\"No voter ID mapped\"}");
      }
    } else {
      server.send(400, "application/json", "{\"error\":\"Fingerprint not recognized\"}");
    }
  });

  // ----------- Second Scan (Cast Vote) ----------
  server.on("/confirm", HTTP_GET, []() {
    if (!server.hasArg("voter_id")) {
      server.send(400, "application/json", "{\"error\":\"Missing voter_id\"}");
      return;
    }
    String voterId = server.arg("voter_id");

    int fid;
    if (matchFingerprint(fid)) {
      if (voterIdMap[fid] == voterId) {
        if (hasVoted[fid]) {
          server.send(200, "application/json", "{\"message\":\"Already voted\"}");
        } else {
          hasVoted[fid] = true;
          sendVoteToServer(voterId);
          server.send(200, "application/json", "{\"message\":\"Vote cast successfully\"}");
          Serial.println("🗳️ Vote cast for Voter ID: " + voterId);
        }
      } else {
        server.send(403, "application/json", "{\"error\":\"Fingerprint doesn't match Voter ID\"}");
      }
    } else {
      server.send(400, "application/json", "{\"error\":\"Fingerprint not recognized\"}");
    }
  });

  server.begin();
  Serial.println("🚀 HTTP server running on ESP32");
}

void loop() {
  server.handleClient();  // Handle HTTP requests
}

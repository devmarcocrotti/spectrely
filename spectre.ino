#include <WiFiNINA.h>
#include <utility/wifi_drv.h>
#include "Firebase_Arduino_WiFiNINA.h"
#include <string.h>

#define DATABASE_URL ""
#define DATABASE_SECRET ""
#define WIFI_SSID ""
#define WIFI_PASSWORD ""

FirebaseData fbdo;
FirebaseData stream;

String path = "/color";

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println();

  WiFiDrv::pinMode(25, OUTPUT); //define green pin
  WiFiDrv::pinMode(26, OUTPUT); //define red pin
  WiFiDrv::pinMode(27, OUTPUT); //define blue pin

  Serial.print("Connecting to Wi-Fi");
  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED) {
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print(".");
    delay(100);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(DATABASE_URL, DATABASE_SECRET, WIFI_SSID, WIFI_PASSWORD);
  Firebase.reconnectWiFi(true);

  if (!Firebase.beginStream(stream, path))
  {
    Serial.println("Can't connect stream, " + stream.errorReason());
  }
}

void loop() {
  if (!Firebase.readStream(stream)) {
    Serial.println("Can't read stream, " + stream.errorReason());
  }
  if (stream.streamTimeout()) {
    Serial.println("Stream timed out, resuming...");
  }

  if (stream.streamAvailable()) {
    String color = stream.stringData();
    char delimiter[] = ",";

    int i;
    char *p;
    char string[128];
    String colors[3];

    color.toCharArray(string, sizeof(string));
    i = 0;
    p = strtok(string, delimiter);
    while(p && i < 3) {
      colors[i] = p;
      p = strtok(NULL, delimiter);
      ++i;
    }

    WiFiDrv::analogWrite(25, colors[0].toInt());
    WiFiDrv::analogWrite(26, colors[1].toInt());
    WiFiDrv::analogWrite(27, colors[2].toInt());

    stream.clear();
  }
}

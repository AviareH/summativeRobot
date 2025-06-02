#include <WiFi.h>
#include <ESP32Servo.h>
#include "webpage.h"

const char* ssid = "RobotController";
const char* password = "gandalf1";

WiFiServer server(80);

Servo frontRight, backRight, frontLeft, backLeft;

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  Serial.println("AP Started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  server.begin();

  frontRight.setPeriodHertz(50);
  backRight.setPeriodHertz(50);
  frontLeft.setPeriodHertz(50);
  backLeft.setPeriodHertz(50);

  frontRight.attach(18, 1000, 2000);
  backRight.attach(19, 1000, 2000);
  frontLeft.attach(21, 1000, 2000);
  backLeft.attach(22, 1000, 2000);

  frontRight.writeMicroseconds(1500);
  backRight.writeMicroseconds(1500);
  frontLeft.writeMicroseconds(1500);
  backLeft.writeMicroseconds(1500);
}

void loop() {
  WiFiClient client = server.accept();
  if (client) {
    String request = client.readStringUntil('\r');
    Serial.println("Request: " + request);
    client.flush();

    // Parse joystick commands: /joystick?side=l&y=1600 or side=r
    if (request.indexOf("/joystick?") != -1) {
      int sideIndex = request.indexOf("side=");
      int yIndex = request.indexOf("y=");
      if (sideIndex != -1 && yIndex != -1) {
        char side = request.charAt(sideIndex + 5);
        int yVal = request.substring(yIndex + 2).toInt();
        yVal = constrain(yVal, 1000, 2000);

        if (side == 'l') {
          frontLeft.writeMicroseconds(yVal);
          backLeft.writeMicroseconds(yVal;
        } else if (side == 'r') {
          frontRight.writeMicroseconds(map(yVal, 1000, 2000, 2000, 1000));
          backRight.writeMicroseconds(map(yVal, 1000, 2000, 2000, 1000));
        }
      }
    }

    // Send webpage
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.println(index_html);

    delay(1);
    client.stop();
  }
}

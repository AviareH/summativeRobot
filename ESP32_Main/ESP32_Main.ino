#include <WiFi.h>
#include <ESP32Servo.h>
#include "webpage.h"

const char* ssid = "RobotController";
const char* password = "gandalf1";

WiFiServer server(80);

Servo frontRight, backRight, frontLeft, backLeft;

int leftY = 1500;
int rightY = 1500;

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

  applyMotorSpeeds();
}

void loop() {
  WiFiClient client = server.accept();
  if (client) {
    String request = client.readStringUntil('\r');
    Serial.println("Request: " + request);
    client.flush();

    if (request.indexOf("/joystick?") != -1) {
      int sideIndex = request.indexOf("side=");
      int yIndex = request.indexOf("y=");

      if (sideIndex != -1 && yIndex != -1) {
        char side = request.charAt(sideIndex + 5);
        int yVal = request.substring(yIndex + 2).toInt();
        yVal = constrain(yVal, 1000, 2000);
        yVal = applyDeadzone(yVal);  // snap to neutral if within deadzone

        if (side == 'l') {
          leftY = yVal;
        } else if (side == 'r') {
          rightY = yVal;
        }

        applyMotorSpeeds();
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

int applyDeadzone(int val) {
  if (abs(val - 1500) < 25) {
    return 1500;
  }
  return val;
}

void applyMotorSpeeds() {
  // Left side (not inverted)
  frontLeft.writeMicroseconds(leftY);
  backLeft.writeMicroseconds(leftY);

  // Right side (inverted)
  frontRight.writeMicroseconds(map(rightY, 1000, 2000, 2000, 1000));
  backRight.writeMicroseconds(map(rightY, 1000, 2000, 2000, 1000));
}

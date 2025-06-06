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

void applyDrive(int driveVal) {
  frontLeft.writeMicroseconds(driveVal);
  backLeft.writeMicroseconds(driveVal);
  frontRight.writeMicroseconds(map(driveVal, 1000, 2000, 2000, 1000));
  backRight.writeMicroseconds(map(driveVal, 1000, 2000, 2000, 1000));
}

void applyTurn(int turnVal) {
  frontLeft.writeMicroseconds(turnVal);
  backLeft.writeMicroseconds(turnVal);
  frontRight.writeMicroseconds(turnVal);
  backRight.writeMicroseconds(turnVal);
}

void loop() {
  WiFiClient client = server.accept();
  if (client) {
    String request = client.readStringUntil('\r');
    Serial.println("Request: " + request);
    client.flush();

    if (request.indexOf("/drive?") != -1) {
      int yIndex = request.indexOf("y=");
      if (yIndex != -1) {
        int yVal = request.substring(yIndex + 2).toInt();
        yVal = constrain(yVal, 1000, 2000);
        if (abs(yVal - 1500) < 25) yVal = 1500; // Deadzone
        applyDrive(yVal);
      }
    } else if (request.indexOf("/turn?") != -1) {
      int xIndex = request.indexOf("x=");
      if (xIndex != -1) {
        int xVal = request.substring(xIndex + 2).toInt();
        xVal = constrain(xVal, 1000, 2000);
        if (abs(xVal - 1500) < 25) xVal = 1500; // Deadzone
        int left = xVal;
        int right = map(xVal, 1000, 2000, 2000, 1000);
        frontLeft.writeMicroseconds(left);
        backLeft.writeMicroseconds(left);
        frontRight.writeMicroseconds(right);
        backRight.writeMicroseconds(right);
      }
    }

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.println(index_html);

    delay(1);
    client.stop();
  }
}

#include <WiFi.h>
#include <ESP32Servo.h>
#include "webpage.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

const char* ssid = "RobotController";
const char* password = "gandalf1";

float x_acceleration, y_acceleration, z_acceleration;
float tempe;
bool autonomousMode = false;

WiFiServer server(80);

Servo frontRight, backRight, frontLeft, backLeft;

const int trigPinL = 32;
const int echoPinL = 33;
const int trigPinR = 27;
const int echoPinR = 14;

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

  pinMode(trigPinL, OUTPUT);
  pinMode(echoPinL, INPUT);
  pinMode(trigPinR, OUTPUT);
  pinMode(echoPinR, INPUT);

  Wire.begin(25, 26);
  if (!mpu.begin(0x68, &Wire)) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) delay(10);
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

void applyDrive(int driveVal) {
  Serial.print("Drive value: ");
  Serial.println(driveVal);
  int RdriveVal = map(driveVal, 1000, 2000, 2000, 1000);

  frontLeft.writeMicroseconds(driveVal);
  backLeft.writeMicroseconds(driveVal);
  frontRight.writeMicroseconds(RdriveVal);
  backRight.writeMicroseconds(RdriveVal);
}

void applyTurn(int turnVal) {
  Serial.print("Turn value: ");
  Serial.println(turnVal);

  int left = turnVal;
  int right = map(turnVal, 1000, 2000, 2000, 1000);

  frontLeft.writeMicroseconds(right);
  backLeft.writeMicroseconds(right);
  frontRight.writeMicroseconds(right);
  backRight.writeMicroseconds(right);
}

long readDistanceCM(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);
  long distance = duration * 0.034 / 2;
  return distance;
}

void handleAutonomousMode() {
  int driveVal = 1600;
  long distL = readDistanceCM(trigPinL, echoPinL);
  long distR = readDistanceCM(trigPinR, echoPinR);

  Serial.print("L: "); Serial.print(distL);
  Serial.print(" | R: "); Serial.println(distR);

  int turnVal = 1500; 
  if (distL < 10 && distR >= 10) {
    turnVal = 2000; 
  } else if (distR < 10 && distL >= 10) {
    turnVal = 1000; 
  } else if (distL < 10 && distR < 10) {
    turnVal = 2000; 
  }

  applyTurn(turnVal);
  applyDrive(driveVal);
  delay(200);
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
        if (abs(yVal - 1500) < 25) yVal = 1500;
        applyDrive(yVal);
      }
    } else if (request.indexOf("/turn?") != -1) {
      int xIndex = request.indexOf("x=");
      if (xIndex != -1) {
        int xVal = request.substring(xIndex + 2).toInt();
        xVal = constrain(xVal, 1000, 2000);
        if (abs(xVal - 1500) < 25) xVal = 1500;
        applyTurn(xVal);
      }
    } else if (request.indexOf("/auto/on") != -1) {
      autonomousMode = true;
    } else if (request.indexOf("/auto/off") != -1) {
      autonomousMode = false;
      applyDrive(1500);
      applyTurn(1500);
    } else if (request.indexOf("read_Web_MPU6050") != -1) {
      String json = "[" + String(x_acceleration, 2) + "," + String(y_acceleration, 2) + "]";
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: application/json");
      client.println("Connection: close");
      client.println();
      client.println(json);
      client.stop();
      return;
    }

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.println(index_html);
    delay(1);
    client.stop();
  }

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  x_acceleration = a.acceleration.x;
  y_acceleration = a.acceleration.y;
  z_acceleration = a.acceleration.z;
  tempe = temp.temperature;

  if (autonomousMode) {
    handleAutonomousMode();
  }
}

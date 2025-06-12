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

  Wire.begin(25, 26); // SDA = GPIO25, SCL = GPIO26
// Beginning of MPU6050 Accelerometer
  if (!mpu.begin(0x68, &Wire)) { // Default I2C address is 0x68
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

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

void loop() {
  WiFiClient client = server.accept();
  if (client) {
    String request = client.readStringUntil('\r');
    Serial.println("Request: " + request);
    client.flush();

    if (request.indexOf("/drive?") != -1) {
      // your existing drive handling
    } 
    else if (request.indexOf("/turn?") != -1) {
      // your existing turn handling
    } 
    else if (request.indexOf("read_Web_MPU6050") != -1) {
      String json = "[" + String(x_acceleration, 2) + "," + String(y_acceleration, 2) + "]";
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: application/json");
      client.println("Connection: close");
      client.println();
      client.println(json);
      client.stop();
      return;
    }

    // send main HTML
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.println(index_html);

    delay(1);
    client.stop();
  }

  // 🔄 NOW this block is inside the loop, where it belongs:
sensors_event_t a, g, temp;
mpu.getEvent(&a, &g, &temp);

x_acceleration = a.acceleration.x;
y_acceleration = a.acceleration.y;
z_acceleration = a.acceleration.z;
tempe = temp.temperature;

Serial.print("Acceleration X: ");
Serial.print(x_acceleration);
Serial.print(", Y: ");
Serial.print(y_acceleration);
Serial.print(", Z: ");
Serial.print(z_acceleration);
Serial.print(" | Temperature: ");
Serial.print(tempe);
Serial.println(" degC");
}


#include <ESP32Servo.h>

Servo frontRight;
Servo backRight;
Servo frontLeft;
Servo backLeft;

void setup() {
  frontRight.setPeriodHertz(50);           // Standard 50Hz for servos
  frontRight.attach(18, 1000, 2000);       
  frontRight.writeMicroseconds(1500);  

  backRight.setPeriodHertz(50);           // Standard 50Hz for servos
  backRight.attach(19, 1000, 2000);       
  backRight.writeMicroseconds(1500);

  frontLeft.setPeriodHertz(50);           // Standard 50Hz for servos
  frontLeft.attach(20, 1000, 2000);       
  frontLeft.writeMicroseconds(1500);   

  backLeft.setPeriodHertz(50);           // Standard 50Hz for servos
  backLeft.attach(21, 1000, 2000);       
  backLeft.writeMicroseconds(1500);     

}

void loop() {
  // Full forward
  frontRight.writeMicroseconds(2000);
  backRight.writeMicroseconds(1000);
  frontLeft.writeMicroseconds(2000);
  backLeft.writeMicroseconds(1000);
  delay(2000);

  // Neutral
  frontRight.writeMicroseconds(1500);
  backRight.writeMicroseconds(1500);
  frontLeft.writeMicroseconds(1500);
  backLeft.writeMicroseconds(1500);
  delay(2000);
}

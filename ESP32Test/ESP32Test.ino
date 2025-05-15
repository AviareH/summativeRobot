#include <ESP32Servo.h>

Servo motor;

void setup() {
  motor.setPeriodHertz(50);           // Standard 50Hz for servos
  motor.attach(18, 1000, 2000);       
  motor.writeMicroseconds(1500);      
}

void loop() {
  // Full forward
  motor.writeMicroseconds(2000);
  delay(2000);

  // Neutral
  motor.writeMicroseconds(1500);
  delay(2000);

  // Full reverse
  motor.writeMicroseconds(1000);
  delay(2000);
}

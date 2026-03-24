#include <SoftwareSerial.h>
#include <AFMotor.h>
// Motor part
AF_DCMotor MOTOR1(1); // M1
AF_DCMotor MOTOR2(2); // M2
AF_DCMotor MOTOR3(3); // M3
AF_DCMotor MOTOR4(4); // M4

void setup() {
  MOTOR1.setSpeed(255);
  MOTOR2.setSpeed(255);
  MOTOR3.setSpeed(255);
  MOTOR4.setSpeed(255);
  
  MOTOR1.run(RELEASE);
  MOTOR2.run(RELEASE);
  MOTOR3.run(RELEASE);
  MOTOR4.run(RELEASE);
  Serial.begin(9600);
}
void Go_Forward() {
  MOTOR1.run(BACKWARD);
  MOTOR2.run(BACKWARD);
  MOTOR3.run(FORWARD);
  MOTOR4.run(BACKWARD);
  delay(20);
}

void loop() {
  Go_Forward();
  //MOTOR1.run(FORWARD);


}

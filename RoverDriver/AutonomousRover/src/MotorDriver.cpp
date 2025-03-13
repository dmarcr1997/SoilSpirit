#include "MotorDriver.h"

Servo frontLeftServo;
Servo frontRightServo;
Servo backLeftServo;
Servo backRightServo;

void initDriveSystem() {
  // SERVO TIMERS
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  
  // SERVO FREQUENCIES
  frontLeftServo.setPeriodHertz(50);
  frontRightServo.setPeriodHertz(50);
  backLeftServo.setPeriodHertz(50);
  backRightServo.setPeriodHertz(50);
  
  // SERVO ATTACHMENTS
  frontLeftServo.attach(SERVO_FRONT_LEFT_PIN, 500, 2500);
  frontRightServo.attach(SERVO_FRONT_RIGHT_PIN, 500, 2500);
  backLeftServo.attach(SERVO_BACK_LEFT_PIN, 500, 2500);
  backRightServo.attach(SERVO_BACK_RIGHT_PIN, 500, 2500);
  
  Serial.println("[Rover] Servos initialized");
  
  // MOTOR PINMODES
  pinMode(F_MOTOR_LEFT_IN1, OUTPUT);
  pinMode(F_MOTOR_LEFT_IN2, OUTPUT);
  pinMode(F_MOTOR_RIGHT_IN3, OUTPUT);
  pinMode(F_MOTOR_RIGHT_IN4, OUTPUT);

  pinMode(M_MOTOR_LEFT_IN1, OUTPUT);
  pinMode(M_MOTOR_LEFT_IN2, OUTPUT);
  pinMode(M_MOTOR_RIGHT_IN3, OUTPUT);
  pinMode(M_MOTOR_RIGHT_IN4, OUTPUT);

  pinMode(B_MOTOR_LEFT_IN1, OUTPUT);
  pinMode(B_MOTOR_LEFT_IN2, OUTPUT);
  pinMode(B_MOTOR_RIGHT_IN3, OUTPUT);
  pinMode(B_MOTOR_RIGHT_IN4, OUTPUT);
}

void stopMotors() {
  Serial.println("[Rover] Stopping...");
  digitalWrite(F_MOTOR_LEFT_IN1, LOW);
  digitalWrite(F_MOTOR_LEFT_IN2, LOW);
  digitalWrite(F_MOTOR_RIGHT_IN3, LOW);
  digitalWrite(F_MOTOR_RIGHT_IN4, LOW);

  digitalWrite(M_MOTOR_LEFT_IN1, LOW);
  digitalWrite(M_MOTOR_LEFT_IN2, LOW);
  digitalWrite(M_MOTOR_RIGHT_IN3, LOW);
  digitalWrite(M_MOTOR_RIGHT_IN4, LOW);

  digitalWrite(B_MOTOR_LEFT_IN1, LOW);
  digitalWrite(B_MOTOR_LEFT_IN2, LOW);
  digitalWrite(B_MOTOR_RIGHT_IN3, LOW);
  digitalWrite(B_MOTOR_RIGHT_IN4, LOW);
}

void motorsForward() {
  Serial.println("[Rover] Moving Forward");
  // Front motors
  digitalWrite(F_MOTOR_LEFT_IN1, HIGH);
  digitalWrite(F_MOTOR_LEFT_IN2, LOW);
  digitalWrite(F_MOTOR_RIGHT_IN3, HIGH);
  digitalWrite(F_MOTOR_RIGHT_IN4, LOW);

  // Middle motors
  digitalWrite(M_MOTOR_LEFT_IN1, HIGH);
  digitalWrite(M_MOTOR_LEFT_IN2, LOW);
  digitalWrite(M_MOTOR_RIGHT_IN3, HIGH);
  digitalWrite(M_MOTOR_RIGHT_IN4, LOW);

  // Back motors
  digitalWrite(B_MOTOR_LEFT_IN1, HIGH);
  digitalWrite(B_MOTOR_LEFT_IN2, LOW);
  digitalWrite(B_MOTOR_RIGHT_IN3, HIGH);
  digitalWrite(B_MOTOR_RIGHT_IN4, LOW);

  delay(MOVEMENT_DELAY);
}

void motorsBackwards() {
  Serial.println("[Rover] Moving Backward");
  // Front motors
  digitalWrite(F_MOTOR_LEFT_IN1, LOW);
  digitalWrite(F_MOTOR_LEFT_IN2, HIGH);
  digitalWrite(F_MOTOR_RIGHT_IN3, LOW);
  digitalWrite(F_MOTOR_RIGHT_IN4, HIGH);

  // Middle motors
  digitalWrite(M_MOTOR_LEFT_IN1, LOW);
  digitalWrite(M_MOTOR_LEFT_IN2, HIGH);
  digitalWrite(M_MOTOR_RIGHT_IN3, LOW);
  digitalWrite(M_MOTOR_RIGHT_IN4, HIGH);

  // Back motors
  digitalWrite(B_MOTOR_LEFT_IN1, LOW);
  digitalWrite(B_MOTOR_LEFT_IN2, HIGH);
  digitalWrite(B_MOTOR_RIGHT_IN3, LOW);
  digitalWrite(B_MOTOR_RIGHT_IN4, HIGH);
  delay(MOVEMENT_DELAY);
}

void centerWheels() {
  Serial.println("[Rover] Centering");
  frontLeftServo.write(CENTER_ANGLE);
  frontRightServo.write(CENTER_ANGLE);
  backLeftServo.write(CENTER_ANGLE);
  backRightServo.write(CENTER_ANGLE);
  delay(MOVEMENT_DELAY);
}

void leftTurn() {
  Serial.println("[Rover] Turning left");
  frontLeftServo.write(LEFT_ANGLE);
  frontRightServo.write(LEFT_ANGLE);
  backLeftServo.write(RIGHT_ANGLE);
  backRightServo.write(RIGHT_ANGLE);
  delay(MOVEMENT_DELAY);
}

void rightTurn() {
  Serial.println("[Rover] Turning right");
  frontLeftServo.write(RIGHT_ANGLE);
  frontRightServo.write(RIGHT_ANGLE);
  backLeftServo.write(LEFT_ANGLE);
  backRightServo.write(LEFT_ANGLE);
  delay(MOVEMENT_DELAY);
}
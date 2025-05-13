#include "MotorDriver.h"

void initDriveSystem() {
  // MOTOR PINMODES
  pinMode(F_MOTOR_LEFT_IN1, OUTPUT);
  pinMode(F_MOTOR_LEFT_IN2, OUTPUT);
  pinMode(F_MOTOR_RIGHT_IN3, OUTPUT);
  pinMode(F_MOTOR_RIGHT_IN4, OUTPUT);

  pinMode(B_MOTOR_LEFT_IN1, OUTPUT);
  pinMode(B_MOTOR_LEFT_IN2, OUTPUT);
  pinMode(B_MOTOR_RIGHT_IN3, OUTPUT);
  pinMode(B_MOTOR_RIGHT_IN4, OUTPUT);
  stopMotors();
}

void stopMotors() {
  Serial.println("[Rover] Stopping...");
  analogWrite(F_MOTOR_LEFT_IN1, 0);
  analogWrite(F_MOTOR_LEFT_IN2, 0);
  analogWrite(F_MOTOR_RIGHT_IN3, 0);
  analogWrite(F_MOTOR_RIGHT_IN4, 0);

  analogWrite(B_MOTOR_LEFT_IN1, 0);
  analogWrite(B_MOTOR_LEFT_IN2, 0);
  analogWrite(B_MOTOR_RIGHT_IN3, 0);
  analogWrite(B_MOTOR_RIGHT_IN4, 0);
}

void motorsForward() {
  Serial.println("[Rover] Moving Forward");
  // Front motors
  analogWrite(F_MOTOR_LEFT_IN1, 225);
  analogWrite(F_MOTOR_LEFT_IN2, 0);
  analogWrite(F_MOTOR_RIGHT_IN3,  225);
  analogWrite(F_MOTOR_RIGHT_IN4, 0);

  // Back motors
  analogWrite(B_MOTOR_LEFT_IN1, 225);
  analogWrite(B_MOTOR_LEFT_IN2, 0);
  analogWrite(B_MOTOR_RIGHT_IN3,  225);
  analogWrite(B_MOTOR_RIGHT_IN4, 0);

  delay(MOVEMENT_DELAY);
}

void motorsForwardSlow() {
  Serial.println("[Rover] Moving Forward");
  // Front motors
  analogWrite(F_MOTOR_LEFT_IN1, 191);
  analogWrite(F_MOTOR_LEFT_IN2, 0);
  analogWrite(F_MOTOR_RIGHT_IN3,  191);
  analogWrite(F_MOTOR_RIGHT_IN4, 0);

  // Back motors
  analogWrite(B_MOTOR_LEFT_IN1, 191);
  analogWrite(B_MOTOR_LEFT_IN2, 0);
  analogWrite(B_MOTOR_RIGHT_IN3,  191);
  analogWrite(B_MOTOR_RIGHT_IN4, 0);

  delay(MOVEMENT_DELAY);
}

void motorsLeft() {
  analogWrite(F_MOTOR_LEFT_IN1, 225);
  analogWrite(F_MOTOR_LEFT_IN2, 0);
  analogWrite(F_MOTOR_RIGHT_IN3, 225);
  analogWrite(F_MOTOR_RIGHT_IN4, 0);

  // Back motors
  analogWrite(B_MOTOR_LEFT_IN1, 0);
  analogWrite(B_MOTOR_LEFT_IN2, 225);
  analogWrite(B_MOTOR_RIGHT_IN3, 0);
  analogWrite(B_MOTOR_RIGHT_IN4, 225);
  delay(MOVEMENT_DELAY);
}

void motorsRight() {
  analogWrite(F_MOTOR_LEFT_IN1, 0);
  analogWrite(F_MOTOR_LEFT_IN2, 225);
  analogWrite(F_MOTOR_RIGHT_IN3, 0);
  analogWrite(F_MOTOR_RIGHT_IN4, 225);

  // Back motors
  analogWrite(B_MOTOR_LEFT_IN1, 225);
  analogWrite(B_MOTOR_LEFT_IN2, 0);
  analogWrite(B_MOTOR_RIGHT_IN3, 225);
  analogWrite(B_MOTOR_RIGHT_IN4, 0);
}

void motorsBackwards() {
  Serial.println("[Rover] Moving Backward");
  // Front motors
  analogWrite(F_MOTOR_LEFT_IN1, 0);
  analogWrite(F_MOTOR_LEFT_IN2, 225);
  analogWrite(F_MOTOR_RIGHT_IN3, 0);
  analogWrite(F_MOTOR_RIGHT_IN4, 225);

  // Back motors
  analogWrite(B_MOTOR_LEFT_IN1, 0);
  analogWrite(B_MOTOR_LEFT_IN2, 225);
  analogWrite(B_MOTOR_RIGHT_IN3, 0);
  analogWrite(B_MOTOR_RIGHT_IN4, 225);
  delay(MOVEMENT_DELAY);
}
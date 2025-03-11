#include "MotorDriver.h"

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
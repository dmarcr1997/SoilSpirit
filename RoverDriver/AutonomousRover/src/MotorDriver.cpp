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
  stopMotors();
}

void stopMotors() {
  Serial.println("[Rover] Stopping...");
  analogWrite(F_MOTOR_LEFT_IN1, 0);
  analogWrite(F_MOTOR_LEFT_IN2, 0);
  analogWrite(F_MOTOR_RIGHT_IN3, 0);
  analogWrite(F_MOTOR_RIGHT_IN4, 0);

  analogWrite(M_MOTOR_LEFT_IN1, 0);
  analogWrite(M_MOTOR_LEFT_IN2, 0);
  analogWrite(M_MOTOR_RIGHT_IN3, 0);
  analogWrite(M_MOTOR_RIGHT_IN4, 0);

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

  // Middle motors
  analogWrite(M_MOTOR_LEFT_IN1,  225);
  analogWrite(M_MOTOR_LEFT_IN2, 0);
  analogWrite(M_MOTOR_RIGHT_IN3,  225);
  analogWrite(M_MOTOR_RIGHT_IN4, 0);

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

  // Middle motors
  analogWrite(M_MOTOR_LEFT_IN1,  191);
  analogWrite(M_MOTOR_LEFT_IN2, 0);
  analogWrite(M_MOTOR_RIGHT_IN3,  191);
  analogWrite(M_MOTOR_RIGHT_IN4, 0);

  // Back motors
  analogWrite(B_MOTOR_LEFT_IN1, 191);
  analogWrite(B_MOTOR_LEFT_IN2, 0);
  analogWrite(B_MOTOR_RIGHT_IN3,  191);
  analogWrite(B_MOTOR_RIGHT_IN4, 0);

  delay(MOVEMENT_DELAY);
}

void motorsLeft() {
  analogWrite(F_MOTOR_LEFT_IN1, 200);
  analogWrite(F_MOTOR_LEFT_IN2, 0);
  analogWrite(F_MOTOR_RIGHT_IN3, 225);
  analogWrite(F_MOTOR_RIGHT_IN4, 0);

  // Middle motors
  analogWrite(M_MOTOR_LEFT_IN1, 200);
  analogWrite(M_MOTOR_LEFT_IN2, 0);
  analogWrite(M_MOTOR_RIGHT_IN3, 225);
  analogWrite(M_MOTOR_RIGHT_IN4, 0);

  // Back motors
  analogWrite(B_MOTOR_LEFT_IN1, 200);
  analogWrite(B_MOTOR_LEFT_IN2, 0);
  analogWrite(B_MOTOR_RIGHT_IN3, 225);
  analogWrite(B_MOTOR_RIGHT_IN4, 0);
  delay(MOVEMENT_DELAY);
}

void motorsRight() {
  analogWrite(F_MOTOR_LEFT_IN1, 225);
  analogWrite(F_MOTOR_LEFT_IN2, 0);
  analogWrite(F_MOTOR_RIGHT_IN3, 200);
  analogWrite(F_MOTOR_RIGHT_IN4, 0);

  // Middle motors
  analogWrite(M_MOTOR_LEFT_IN1, 225);
  analogWrite(M_MOTOR_LEFT_IN2, 0);
  analogWrite(M_MOTOR_RIGHT_IN3, 200);
  analogWrite(M_MOTOR_RIGHT_IN4, 0);

  // Back motors
  analogWrite(B_MOTOR_LEFT_IN1, 225);
  analogWrite(B_MOTOR_LEFT_IN2, 0);
  analogWrite(B_MOTOR_RIGHT_IN3, 200);
  analogWrite(B_MOTOR_RIGHT_IN4, 0);
}

void motorsBackwards() {
  Serial.println("[Rover] Moving Backward");
  // Front motors
  analogWrite(F_MOTOR_LEFT_IN1, 0);
  analogWrite(F_MOTOR_LEFT_IN2, 225);
  analogWrite(F_MOTOR_RIGHT_IN3, 0);
  analogWrite(F_MOTOR_RIGHT_IN4, 225);

  // Middle motors
  analogWrite(M_MOTOR_LEFT_IN1, 0);
  analogWrite(M_MOTOR_LEFT_IN2, 225);
  analogWrite(M_MOTOR_RIGHT_IN3, 0);
  analogWrite(M_MOTOR_RIGHT_IN4, 225);

  // Back motors
  analogWrite(B_MOTOR_LEFT_IN1, 0);
  analogWrite(B_MOTOR_LEFT_IN2, 225);
  analogWrite(B_MOTOR_RIGHT_IN3, 0);
  analogWrite(B_MOTOR_RIGHT_IN4, 225);
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
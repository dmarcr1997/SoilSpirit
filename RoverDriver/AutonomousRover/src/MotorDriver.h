#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include <ESP32Servo.h>


//SERVO SETUP
extern Servo frontLeftServo;
extern Servo frontRightServo;
extern Servo backLeftServo;
extern Servo backRightServo;

const int SERVO_FRONT_LEFT_PIN = 16;
const int SERVO_FRONT_RIGHT_PIN = 17;
const int SERVO_BACK_LEFT_PIN = 18;
const int SERVO_BACK_RIGHT_PIN = 19;

// L298N MOTOR SETUP
const int F_MOTOR_LEFT_IN1 = 2;   // F-Left motor control pin 1
const int F_MOTOR_LEFT_IN2 = 4;   // F-Left motor control pin 2
const int F_MOTOR_RIGHT_IN3 = 5;  // F-Right motor control pin 1
const int F_MOTOR_RIGHT_IN4 = 13; // F-Right motor control pin 2

const int M_MOTOR_LEFT_IN1 = 14;   // M-Left motor control pin 1
const int M_MOTOR_LEFT_IN2 = 12;   // M-Left motor control pin 2
const int M_MOTOR_RIGHT_IN3 = 15;  // M-Right motor control pin 1
const int M_MOTOR_RIGHT_IN4 = 27; // M-Right motor control pin 2

const int B_MOTOR_LEFT_IN1 = 26;   // B-Left motor control pin 1
const int B_MOTOR_LEFT_IN2 = 25;   // B-Left motor control pin 2
const int B_MOTOR_RIGHT_IN3 = 33;  // B-Right motor control pin 1
const int B_MOTOR_RIGHT_IN4 = 32; // B-Right motor control pin 2

// CONSTANTS
const int CENTER_ANGLE = 90;
const int RIGHT_ANGLE = 30;
const int LEFT_ANGLE = 150; 
const int MOVEMENT_DELAY = 500; 

void initDriveSystem();
void stopMotors();
void motorsForward();
void motorsForwardSlow();
void motorsBackwards();
void motorsLeft();
void motorsRight();
void centerWheels();
void leftTurn();
void rightTurn();

#endif // MOTOR_DRIVER_H
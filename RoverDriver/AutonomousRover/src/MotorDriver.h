#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H
#include "Arduino.h"

// L298N MOTOR SETUP
const int F_MOTOR_LEFT_IN1 = 2;   // F-Left motor control pin 1
const int F_MOTOR_LEFT_IN2 = 4;   // F-Left motor control pin 2
const int F_MOTOR_RIGHT_IN3 = 5;  // F-Right motor control pin 1
const int F_MOTOR_RIGHT_IN4 = 13; // F-Right motor control pin 2

const int B_MOTOR_LEFT_IN1 = 26;   // B-Left motor control pin 1
const int B_MOTOR_LEFT_IN2 = 25;   // B-Left motor control pin 2
const int B_MOTOR_RIGHT_IN3 = 33;  // B-Right motor control pin 1
const int B_MOTOR_RIGHT_IN4 = 32; // B-Right motor control pin 2

const int MOVEMENT_DELAY = 500; 

void initDriveSystem();
void stopMotors();
void motorsForward();
void motorsForwardSlow();
void motorsBackwards();
void motorsLeft();
void motorsRight();

#endif // MOTOR_DRIVER_H
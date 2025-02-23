#include <ESP32Servo.h>

//SERVO SETUP
Servo frontLeftServo;
Servo frontRightServo;
Servo backLeftServo;
Servo backRightServo;

const int FRONT_LEFT_PIN = 26;
const int FRONT_RIGHT_PIN = 33;
const int BACK_LEFT_PIN = 32;
const int BACK_RIGHT_PIN = 25;

// L298N MOTOR SETUP FRONT SET ONLY
const int MOTOR_LEFT_IN1 = 2;   // Left motor control pin 1
const int MOTOR_LEFT_IN2 = 4;   // Left motor control pin 2
const int MOTOR_RIGHT_IN3 = 5;  // Right motor control pin 1
const int MOTOR_RIGHT_IN4 = 13; // Right motor control pin 2

// CONSTANTS
const int CENTER_ANGLE = 90;
const int RIGHT_ANGLE = 120;
const int LEFT_ANGLE = 60; 
const int MOVEMENT_DELAY = 2000;

bool motorsEnabled = false;

void testServos() {
  Serial.println("Testing front left servo");
  frontLeftServo.write(CENTER_ANGLE);
  delay(500);
  
  Serial.println("Testing front right servo");
  frontRightServo.write(CENTER_ANGLE);
  delay(500);
  
  Serial.println("Testing back left servo");
  backLeftServo.write(CENTER_ANGLE);
  delay(500);
  
  Serial.println("Testing back right servo");
  backRightServo.write(CENTER_ANGLE);
  delay(500);
}

void setup() {
  Serial.println("Initialization...");
  
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
  frontLeftServo.attach(FRONT_LEFT_PIN, 500, 2500);
  frontRightServo.attach(FRONT_RIGHT_PIN, 500, 2500);
  backLeftServo.attach(BACK_LEFT_PIN, 500, 2500);
  backRightServo.attach(BACK_RIGHT_PIN, 500, 2500);
  
  Serial.println("Servos initialized");
  
  // MOTOR PINMODES
  pinMode(MOTOR_LEFT_IN1, OUTPUT);
  pinMode(MOTOR_LEFT_IN2, OUTPUT);
  pinMode(MOTOR_RIGHT_IN3, OUTPUT);
  pinMode(MOTOR_RIGHT_IN4, OUTPUT);
  
  // ENSURE ALL MOTORS ARE STOPPED
  digitalWrite(MOTOR_LEFT_IN1, LOW);
  digitalWrite(MOTOR_LEFT_IN2, LOW);
  digitalWrite(MOTOR_RIGHT_IN3, LOW);
  digitalWrite(MOTOR_RIGHT_IN4, LOW);
  
  Serial.println("Motor pins initialized");
  
  // INDIVIDUAL SERVO TESTS
  Serial.println("Testing servos...");
  testServos();
  
  // DISABLE MOTORS FOR SAFETY
  motorsEnabled = false;
  Serial.println("Initialization complete. Motors disabled for safety.");
}

void loop() {
  // Turn servos right
  Serial.println("Turning right");
  frontLeftServo.write(RIGHT_ANGLE);
  frontRightServo.write(RIGHT_ANGLE);
  backLeftServo.write(RIGHT_ANGLE);
  backRightServo.write(RIGHT_ANGLE);
  delay(MOVEMENT_DELAY);
  
  // Center servos
  Serial.println("Centering");
  frontLeftServo.write(CENTER_ANGLE);
  frontRightServo.write(CENTER_ANGLE);
  backLeftServo.write(CENTER_ANGLE);
  backRightServo.write(CENTER_ANGLE);
  delay(MOVEMENT_DELAY);
  
  // Turn servos left
  Serial.println("Turning left");
  frontLeftServo.write(LEFT_ANGLE);
  frontRightServo.write(LEFT_ANGLE);
  backLeftServo.write(LEFT_ANGLE);
  backRightServo.write(LEFT_ANGLE);
  delay(MOVEMENT_DELAY);
  
  // Center servos
  Serial.println("Centering");
  frontLeftServo.write(CENTER_ANGLE);
  frontRightServo.write(CENTER_ANGLE);
  backLeftServo.write(CENTER_ANGLE);
  backRightServo.write(CENTER_ANGLE);
  delay(MOVEMENT_DELAY);
  
  // COMMENT OUT TO DISABLE MOTOR TESTS
  motorsEnabled = true;
  
  if (motorsEnabled) {
    Serial.println("Testing motors forward");
    digitalWrite(MOTOR_LEFT_IN1, HIGH);
    digitalWrite(MOTOR_LEFT_IN2, LOW);
    digitalWrite(MOTOR_RIGHT_IN3, HIGH);
    digitalWrite(MOTOR_RIGHT_IN4, LOW);
    delay(1000);
    
    Serial.println("Stopping motors");
    digitalWrite(MOTOR_LEFT_IN1, LOW);
    digitalWrite(MOTOR_LEFT_IN2, LOW);
    digitalWrite(MOTOR_RIGHT_IN3, LOW);
    digitalWrite(MOTOR_RIGHT_IN4, LOW);
    delay(1000);
  }
}
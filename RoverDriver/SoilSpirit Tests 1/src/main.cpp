#include <ESP32Servo.h>

//SERVO SETUP
Servo frontLeftServo;
Servo frontRightServo;
Servo backLeftServo;
Servo backRightServo;

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
const int RIGHT_ANGLE = 120;
const int LEFT_ANGLE = 60; 
const int MOVEMENT_DELAY = 2000;

bool motorsEnabled = false;
bool servosEnabled = false;
bool fullTestSuite = false;

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

void stopMotors() {
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
}

void setup() {
  Serial.begin(9600);
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
  frontLeftServo.attach(SERVO_FRONT_LEFT_PIN, 500, 2500);
  frontRightServo.attach(SERVO_FRONT_RIGHT_PIN, 500, 2500);
  backLeftServo.attach(SERVO_BACK_LEFT_PIN, 500, 2500);
  backRightServo.attach(SERVO_BACK_RIGHT_PIN, 500, 2500);
  
  Serial.println("Servos initialized");
  
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
  
  // ENSURE ALL MOTORS ARE STOPPED
  stopMotors();
  Serial.println("Motor pins initialized");
  
  // INDIVIDUAL SERVO TESTS
  Serial.println("Testing servos...");
  testServos();
  
  // DISABLE MOTORS FOR SAFETY
  motorsEnabled = false;
  servosEnabled = false;
  fullTestSuite = false;
  Serial.println("Initialization complete. Tests disabled for safety.");
}

void loop() {
  // COMMENT OUT TO ENABLE DIFFERENT TESTS TESTS
  // motorsEnabled = true;
  // servosEnabled = true;
  fullTestSuite = true;
  if(servosEnabled || fullTestSuite) {
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
  }
  if (motorsEnabled || fullTestSuite) {
    Serial.println("Testing motors forward");
    motorsForward();
    delay(1000);
    
    Serial.println("Stopping motors");
    stopMotors();
    delay(2000);
  }
}
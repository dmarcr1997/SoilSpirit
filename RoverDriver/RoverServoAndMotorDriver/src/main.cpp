#include <ESP32Servo.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "secrets.h"

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
const int RETURN_DELAY = 500; 
const int MOVEMENT_DELAY = 2000;
const int HTTP_REQUEST_INTERVAL = 2500; 

//COMMANDS
enum Command {
  LEFT_TURN,
  RIGHT_TURN,
  FORWARD,
  BACKWARD,
  STOP
};

//Global Vars
String lastCommand = "FULL_STOP";
unsigned long lastRequestTime = 0; 

String retrieveCommandFromCamera() {
  HTTPClient http;
  String serverPath = String(serverEndpoint) + "?lastCommand=" + lastCommand;

  http.begin(serverPath.c_str());

  int httpResponseCode = http.GET();
  String command = "STOP";

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println("Response: " + payload);

    if (payload.length() > 0) {
      command = payload;
    }
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
    Serial.println("Failed to get command, using STOP");
  }

  http.end();
  return command;
}

void connectToWiFi() {
  Serial.println("Connecting...");
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
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
  Serial.println("Moving Forward");
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
  Serial.println("Moving Backward");
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
  Serial.println("Centering");
  frontLeftServo.write(CENTER_ANGLE);
  frontRightServo.write(CENTER_ANGLE);
  backLeftServo.write(CENTER_ANGLE);
  backRightServo.write(CENTER_ANGLE);
  delay(MOVEMENT_DELAY);
}

void leftTurn() {
  Serial.println("Turning left");
  frontLeftServo.write(LEFT_ANGLE);
  frontRightServo.write(LEFT_ANGLE);
  backLeftServo.write(LEFT_ANGLE);
  backRightServo.write(LEFT_ANGLE);
  delay(MOVEMENT_DELAY);
}

void rightTurn() {
  Serial.println("Turning right");
  frontLeftServo.write(RIGHT_ANGLE);
  frontRightServo.write(RIGHT_ANGLE);
  backLeftServo.write(RIGHT_ANGLE);
  backRightServo.write(RIGHT_ANGLE);
  delay(MOVEMENT_DELAY);
}

void executeCommand(String command) {
  if (command == "TURN_LEFT"){ 
    leftTurn();
    motorsForward();
    centerWheels();
    lastCommand = "TURN_LEFT";
  } else if (command == "TURN_RIGHT"){
    rightTurn();
    motorsForward();
    centerWheels();
    lastCommand = "TURN_RIGHT";
  } else if (command =="FORWARD") {
    motorsForward();
    lastCommand = "FORWARD";
  } else if (command == "BACKWARD"){
    motorsBackwards();
    lastCommand = "BACKWARD";
  } else {
    centerWheels();
    stopMotors();
    lastCommand = "FULL_STOP";
  }
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
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected. Reconnecting...");
    connectToWiFi();
  }

  unsigned long time = millis();
  if (time - lastRequestTime >= HTTP_REQUEST_INTERVAL) {
    lastRequestTime = time;
    String newCommand = retrieveCommandFromCamera();
    Serial.println("Received command: " + newCommand);
    
    if (newCommand != lastCommand) {
      executeCommand(newCommand);
    }
  }
}
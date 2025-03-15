#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "MotorDriver.h"
#include "secrets.h"

// ULTRA SONIC SENSORS
const int TRIGGER_PIN_C = 18;
const int ECHO_C_PIN = 35;
const int CRIT_PIN = 21;

const int LEFT_THRESH = 0.25;
const int CENTER_THRESH = 0.5;
const int RIGHT_THRESH = 0.25;
const int SAFE_DISTANCE = 80;
const int STOP_DISTANCE = 40;
const int MAX_DISTANCE = 400;

// CONSTANTS
const int HTTP_REQUEST_INTERVAL = 1000;
const int SENSOR_CHECK_INTERVAL = 500;

// Global Vars
String currentCommand = "FULL_STOP";
unsigned long lastRequestTime = 0;
bool isMoving = false;
long durationLeft, durationCenter, durationRight;
float distanceLeft, distanceCenter, distanceRight;
float fusedDistance;
unsigned long lastSensorCheckTime = 0;
bool roverActive = false;

void setupUltrasonicSensors() {
  pinMode(CRIT_PIN, OUTPUT);
  pinMode(TRIGGER_PIN_C, OUTPUT);
  pinMode(ECHO_C_PIN, INPUT);
  digitalWrite(TRIGGER_PIN_C, LOW);
  digitalWrite(CRIT_PIN, LOW);
  Serial.println("[Rover] US Sensors Init...");
}

float triggerSensor(int triggerPin, int echoPin) {
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 23530);
  float distance = duration * 0.034 / 2;
  if (distance == 0 || distance > MAX_DISTANCE) distance = MAX_DISTANCE;
  return distance;
}

String ultraSonicSensorOD() {
  distanceCenter = triggerSensor(TRIGGER_PIN_C, ECHO_C_PIN);
  Serial.print("cm  C:");
  Serial.print(distanceCenter);
  if (distanceCenter <= STOP_DISTANCE) {
    // Critical distance - stop immediately
    digitalWrite(CRIT_PIN, HIGH);
    Serial.print("STOPP");
    Serial.print(distanceCenter);
    return "FULL_STOP";
  } else {
    // All clear, continue with current command
    digitalWrite(CRIT_PIN, LOW);
    return "FORWARD";
  }
}


String retrieveCommandFromServer() {
  HTTPClient http;
  String serverPath = String(serverEndpoint);

  http.begin(serverPath.c_str());

  int httpResponseCode = http.GET();
  String command = "FULL_STOP";

  if (httpResponseCode > 0) {
    Serial.print("[Rover-Server] HTTP Response code: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println("[Rover-Server] Response: " + payload);

    if (payload.length() > 0) {
      DynamicJsonDocument doc(256);
      DeserializationError error = deserializeJson(doc, payload);

      if(!error) {
        command = doc["command"].as<String>();
      } else {
        Serial.println("[Rover-Server] JSON PARSE ERRROR: ");
        Serial.println(error.c_str());
        command = "FULL_STOP";
      }
    }
  } else {
    Serial.print("[Rover-Server]  Error code: ");
    Serial.println(httpResponseCode);
    Serial.println("[Rover-Server] Failed to get command, using STOP");
  }

  http.end();
  return command;
}

void connectToWiFi() {
  Serial.println("[Rover] Connecting...");
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("[Rover] WiFi connected");
}

void executeCommand(String command) {
  if (command == "STOP" || command == "FULL_STOP") {
    stopMotors();
    centerWheels();
    isMoving = false;
    currentCommand = "FULL_STOP";
    Serial.println("[Rover] Stopping...");
    return;
  }
  isMoving = true;
  if (command == "TURN_LEFT"){ 
      leftTurn(); 
      motorsLeft();
      centerWheels();
      currentCommand = "TURN_LEFT";
  } else if (command == "TURN_RIGHT"){
      rightTurn();
      motorsRight();
      centerWheels();
      currentCommand = "TURN_RIGHT";
  } else if (command =="FORWARD") {
      motorsForward();
      currentCommand = "FORWARD";
  } else if (command == "BACKWARD"){
      motorsBackwards();
      currentCommand = "BACKWARD";
  } else {
      centerWheels();
      stopMotors();
      isMoving = false;
      currentCommand = "FULL_STOP";
      Serial.println("[Rover] Stopping...");
      return;
  }
}


void setup() {
  // Serial.begin(9600);
  Serial.println("[Rover] Initialization...");
  
  initDriveSystem();
  setupUltrasonicSensors();

  Serial.println("[Rover] Setup Complete");
}

void loop() {
  unsigned long currentTime = millis();
  if (currentTime - lastSensorCheckTime >= SENSOR_CHECK_INTERVAL) {
    lastSensorCheckTime = currentTime;
    String ultraSonicSensorArrayCommand = ultraSonicSensorOD();
    if (roverActive) {
      Serial.print("WHOAH ");
      Serial.println(currentCommand);
      if (currentCommand == "FORWARD" || currentCommand == "OD") {
        String obstacleCommand = ultraSonicSensorOD();
        if (obstacleCommand == "FULL_STOP") {
          executeCommand("FULL_STOP");
        } else if (currentCommand == "OD" && obstacleCommand == "FORWARD") {
          executeCommand("FORWARD");
        }
      }
    }
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[Rover] WiFi disconnected. Reconnecting...");
    connectToWiFi();
  }

  if (currentTime - lastRequestTime > HTTP_REQUEST_INTERVAL) {
    lastRequestTime = currentTime;
    String newCommand = retrieveCommandFromServer(); //USER COMMAND and START/STOP
    if(newCommand == "OD") {
      currentCommand = "OD";
      roverActive = true;
      Serial.println("[Rover] Received command: " + newCommand);
    } else if(newCommand == "CONTROL_STOP") {
      roverActive = false;
      executeCommand("FULL_STOP");
    } else {
      executeCommand(newCommand);
    }
  } 
}
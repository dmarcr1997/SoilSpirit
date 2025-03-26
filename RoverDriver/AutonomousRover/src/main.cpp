#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "MotorDriver.h"
#include "secrets.h"

// ULTRA SONIC SENSORS
const int TRIGGER_PIN_C = 23;
const int ECHO_C_PIN = 35;
const int CRIT_PIN = 21;

const int LEFT_THRESH = 0.25;
const int CENTER_THRESH = 0.5;
const int RIGHT_THRESH = 0.25;
const int SAFE_DISTANCE = 80;
const int STOP_DISTANCE = 40;
const int MAX_DISTANCE = 400;

// CONSTANTS
const int HTTP_REQUEST_INTERVAL = 500;

// Global Vars
String currentCommand = "FULL_STOP";
String lastCommand = "FULL_STOP";
unsigned long lastRequestTime = 0;
bool isMoving = false;
long durationLeft, durationCenter, durationRight;
float distanceLeft, distanceCenter, distanceRight;
float fusedDistance;
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
    digitalWrite(CRIT_PIN, HIGH);
    Serial.println("[Rover] OBSTACLE DETECTED - STOPPING");
    return "FULL_STOP";
  } else {
    digitalWrite(CRIT_PIN, LOW);
    Serial.println("[Rover] PATH CLEAR");
    return "FORWARD_SLOWLY";
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
  lastCommand = currentCommand;
  if (command == "STOP" || command == "FULL_STOP") {
    stopMotors();
    centerWheels();
    isMoving = false;
    roverActive = false;
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
  } else if (command == "FORWARD_SLOWLY"){
      motorsForwardSlow();
      currentCommand = "FORWARD_SLOWLY";
  } else {
      centerWheels();
      stopMotors();
      isMoving = false;
      roverActive = false;
      currentCommand = "FULL_STOP";
      Serial.println("[Rover] Stopping...");
      return;
  }
}


void setup() {
  Serial.begin(9600);
  Serial.println("[Rover] Initialization...");
  
  initDriveSystem();
  setupUltrasonicSensors();
  connectToWiFi();
  Serial.println("[Rover] Setup Complete");
}

void loop() {
  unsigned long currentTime = millis();

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
      String obstacleCommand = ultraSonicSensorOD();
      executeCommand(obstacleCommand);
    } else if(newCommand == "CONTROL_STOP" || newCommand == "FULL_STOP") {
      roverActive = false;
      executeCommand("FULL_STOP");
    } else {
      executeCommand(newCommand);
    }
  } 
}
#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "MotorDriver.h"
#include "secrets.h"

// ULTRA SONIC SENSORS
// const int TRIGGER_PIN_L = 16;
const int TRIGGER_PIN_C = 15;
// const int TRIGGER_PIN_R = 17;

// const int ECHO_L_PIN = 34;
const int ECHO_C_PIN = 35;
// const int ECHO_R_PIN = 21;
const int LEFT_THRESH = 0.25;
const int CENTER_THRESH = 0.5;
const int RIGHT_THRESH = 0.25;
const int SAFE_DISTANCE = 50;
const int STOP_DISTANCE = 20;
const int MAX_DISTANCE = 400;

// CONSTANTS
const int HTTP_REQUEST_INTERVAL = 1000;
const int SENSOR_CHECK_INTERVAL = 100;

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
  // pinMode(TRIGGER_PIN_L, OUTPUT);
  pinMode(TRIGGER_PIN_C, OUTPUT);
  // pinMode(TRIGGER_PIN_R, OUTPUT);
  
  // pinMode(ECHO_L_PIN, INPUT);
  pinMode(ECHO_C_PIN, INPUT);
  // pinMode(ECHO_R_PIN, INPUT);
  // digitalWrite(TRIGGER_PIN_L, LOW);
  digitalWrite(TRIGGER_PIN_C, LOW);
  // digitalWrite(TRIGGER_PIN_R, LOW);
  
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


void readAllUSSensors() {
  distanceCenter = triggerSensor(TRIGGER_PIN_C, ECHO_C_PIN);
  // distanceLeft = triggerSensor(TRIGGER_PIN_L, ECHO_L_PIN);
  // distanceRight = triggerSensor(TRIGGER_PIN_R, ECHO_R_PIN);
  
  // fusedDistance = (LEFT_THRESH * distanceLeft) + (CENTER_THRESH * distanceCenter) * (RIGHT_THRESH * distanceRight);

  Serial.print("[US Sensors] L:");
  Serial.print(distanceLeft);
  Serial.print("cm  C:");
  Serial.print(distanceCenter);
  Serial.print("cm  R:");
  Serial.print(distanceRight);
  Serial.print("cm  Fused:");
  Serial.print(fusedDistance);
  Serial.println("cm");
}

String ultraSonicSensorOD() {
  readAllUSSensors();
  
  // Decision logic based on sensor readings
  if (distanceCenter < STOP_DISTANCE) {
    // Critical distance - stop immediately
    return "FULL_STOP";
  } 
  // else if (distanceCenter < SAFE_DISTANCE) {
  //   // Center obstacle detected
    
  //   // Determine which direction is clearer
  //   if (distanceLeft > distanceRight && distanceLeft > SAFE_DISTANCE) {
  //     return "TURN_LEFT";
  //   } 
  //   else if (distanceRight > distanceLeft && distanceRight > SAFE_DISTANCE) {
  //     return "TURN_RIGHT";
  //   }
  //   else {
  //     // Both sides are blocked or unsafe
  //     return "BACKWARD";
  //   }
  // }
  // else if (distanceLeft < SAFE_DISTANCE) {
  //   // Left obstacle detected, turn right
  //   return "TURN_RIGHT";
  // }
  // else if (distanceRight < SAFE_DISTANCE) {
  //   // Right obstacle detected, turn left
  //   return "TURN_LEFT";
  // }
  else {
    // All clear, continue with current command
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
      motorsForward();
      currentCommand = "TURN_LEFT";
  } else if (command == "TURN_RIGHT"){
      rightTurn();
      motorsForward();
      currentCommand = "TURN_RIGHT";
  } else if (command =="FORWARD") {
      centerWheels();
      motorsForward();
      currentCommand = "FORWARD";
  } else if (command == "BACKWARD"){
      centerWheels();
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
  Serial.begin(9600);
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
      executeCommand(ultraSonicSensorArrayCommand);
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
      roverActive = true;
      Serial.println("[Rover] Received command: " + newCommand);
      executeCommand(newCommand);
    } else if(newCommand == "CONTROL_STOP") {
      roverActive = false;
    } else {
      executeCommand(newCommand);
    }
  } 
}
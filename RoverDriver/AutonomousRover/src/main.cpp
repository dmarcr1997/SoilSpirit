#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "MotorDriver.h"
#include "secrets.h"
#include <SparkFun_VL53L5CX_Library.h>
const int CRIT_PIN = 21;
const int LEFT_THRESH = 0.25;
const int CENTER_THRESH = 0.5;
const int RIGHT_THRESH = 0.25;
const int SAFE_DISTANCE = 80;
const int STOP_DISTANCE = 300;
const int MAX_DISTANCE = 4000;

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

// TOF INIT
SparkFun_VL53L5CX myTOFSensor;
VL53L5CX_ResultsData measurementData;

void setupTOF() {
  Wire.begin(21, 22);
  if(!myTOFSensor.begin()){
    Serial.println("[ToF] VL53L5CX not detected. Halting.");
    while (1);
  }
  myTOFSensor.setResolution(4 * 4); // 4x4 grid
  myTOFSensor.setRangingFrequency(15); // Hz (max ~15)
  myTOFSensor.startRanging();

  Serial.println("[ToF] VL53L5CX Initialized");
}

String tofSensorOD() {
  if (myTOFSensor.isDataReady()) {
    myTOFSensor.getRangingData(&measurementData);
    uint32_t leftValue = 0;
    uint32_t rightValue = 0;
    uint32_t centerValue = 0;
    int leftCount = 0, rightCount = 0, centerCount = 0;
    for (int i = 0; i < 64; i++) {
      uint16_t distance = measurementData.distance_mm[i]; // get #0-63 rows of TOF sensor 
      int gridColumn = i % 8; // Break up TOF grid into 8 columns 
      if (distance <= 0 || distance >= MAX_DISTANCE) continue; //Ignore out of bounds distances
      if (gridColumn < 3) {
        leftValue += distance;
        leftCount++;
      } else if (gridColumn > 4) {
        rightValue += distance;
        rightCount++;
      } else {
        centerValue += distance;
        centerCount++;
      }
    } 
    uint16_t leftAverage = leftCount ? leftValue / leftCount : 9999;
    uint16_t rightAverage = rightCount ? rightValue / rightCount : 9999;
    uint16_t centerAverage = centerCount ? centerValue / centerCount : 9999;

    Serial.printf("[ToF] Distances ( L: %d | R: %d | C: %d)\n", leftAverage, rightAverage, centerAverage);
    
    if (centerAverage <= STOP_DISTANCE) {
      digitalWrite(CRIT_PIN, HIGH);
      Serial.println("[Rover] CENTER OBSTACLE DETECTED - STOPPING");
      return "FULL_STOP";
    } else if (leftAverage <= STOP_DISTANCE) {
      digitalWrite(CRIT_PIN, HIGH);
      Serial.println("[Rover] LEFT OBSTACLE DETECTED - TURNING RIGHT");
      return "TURN_RIGHT";
    } else if (rightAverage <= STOP_DISTANCE) {
      digitalWrite(CRIT_PIN, HIGH);
      Serial.println("[Rover] RIGHT OBSTACLE DETECTED - TURNING LEFT");
      return "TURN_LEFT";
    } else {
      digitalWrite(CRIT_PIN, LOW);
      Serial.println("[Rover] PATH CLEAR");
      return "FORWARD_SLOWLY";
    }
  }

  // Fallback to last command if no data ready
  return currentCommand;
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
    isMoving = false;
    roverActive = false;
    currentCommand = "FULL_STOP";
    Serial.println("[Rover] Stopping...");
    return;
  }
  isMoving = true;
  if (command == "TURN_LEFT"){ 
      motorsLeft();
      currentCommand = "TURN_LEFT";
  } else if (command == "TURN_RIGHT"){
      motorsRight();
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
      stopMotors();
      isMoving = false;
      roverActive = false;
      currentCommand = "FULL_STOP";
      Serial.println("[Rover] Stopping...");
      return;
  }
}


void setup() {
  Serial.begin(115200);
  Serial.println("[Rover] Initialization...");
  
  initDriveSystem();
  setupTOF();
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
      String obstacleCommand = tofSensorOD();
      executeCommand(obstacleCommand);
    } else if(newCommand == "CONTROL_STOP" || newCommand == "FULL_STOP") {
      roverActive = false;
      executeCommand("FULL_STOP");
    } else {
      executeCommand(newCommand);
    }
  } 
}
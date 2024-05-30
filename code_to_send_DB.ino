#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <WebServer.h>
#include "time.h"

// Insert your network credentials
#define WIFI_SSID "Ettronics Hightech"
#define WIFI_PASSWORD "11111111"

// Insert Firebase project API Key
#define API_KEY "AIzaSyBAgeUvIPwMirPSPywX9YZJZ79htSpKO5c"

// Insert Authorized Email and Corresponding Password
#define USER_EMAIL "test@gmail.com"
#define USER_PASSWORD "test@gmail.com"

// Insert RTDB URL
#define DATABASE_URL "https://wrud-5b418-default-rtdb.firebaseio.com/"

// Define HC-SR04 sensor pins
const int trigPin1 = 5;
const int echoPin1 = 18;
const int trigPin2 = 22;
const int echoPin2 = 23;
const int trigPin3 = 19;
const int echoPin3 = 21;
const int trigPin4 = 4;
const int echoPin4 = 2;

// Define Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Variable to save USER UID
String uid;

// Database main path (to be updated in setup with the user UID)
String databasePath;
// Database child nodes
String temperaturePath = "temperature";
String pulsePath = "pulse";
String heightPath = "height";
String weightPath = "weight";
String timePath = "timestamp";

// Parent Node (to be updated in every loop)
String parentPath;

unsigned long timestamp;
FirebaseJson json;

// Timer variables (send new readings every three minutes)
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 180000; // 3 minutes

WebServer server(80);
String customUserID;

void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println();
  Serial.print("Connected to WiFi with IP: ");
  Serial.println(WiFi.localIP());
}

unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return 0;
  }
  time(&now);
  return now + 3600; //3600 seconds(1 hour) to my epoch time
}

// Function to get local time as a string, GMT+0
String getFormattedTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "Failed to obtain time";
  }

  // Add one hour to the obtained time
  timeinfo.tm_hour += 1;

  // Handle cases where hour exceeds 23 by adjusting the day
  if (timeinfo.tm_hour >= 24) {
    timeinfo.tm_hour -= 24;
    timeinfo.tm_mday += 1; // Increment the day
  }

  char timeString[30];
  strftime(timeString, sizeof(timeString), "%d-%m-%Y %H:%M:%S", &timeinfo);
  return String(timeString);
}

// Function to get distance from HC-SR04 sensor
float getDistance(int trigPin, int echoPin) {
  // Clear the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Trigger the sensor
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echoPin
  long duration = pulseIn(echoPin, HIGH);

  // Calculate the distance
  float distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)

  return distance;
}

void handleRoot() {
  server.send(200, "text/html", R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <title>Enter User ID</title>
      <style>
        body {
          padding-left: 20px;
        }
        
        #userID {
            display: block;
            height: 50px;
            width: 50%;
            background-color: rgba(255,255,255,0.07);
            border-radius: 3px;
            padding: 0 10px;
            margin-top: 8px;
            font-size: 14px;
            font-weight: 300;
        }
        ::placeholder{
            color: #e5e5e5;
        }

        .button{
            margin-top: 10px;
            width: 50%;
            background-color: #0f7a73;
            border: 1px solid #0f7a73;
            color: #fff;
            padding: 10px 0;
            font-size: 18px;
            font-weight: 600;
            border-radius: 10px;
            cursor: pointer;
            transition: .3s;
        }
        .button:hover {
            padding: 15px 0;
            transition: .3s;
        }
      </style>
    </head>
    <body>
      <h1>Enter User ID to Initialize</h1>
      <form action="/submit" method="POST">
        <input type="text" id="userID" name="userID"><br>
        <input class="button" type="submit" value="Submit">
      </form>
    </body>
    </html>
  )rawliteral");
}

void handleSubmit() {
  if (server.hasArg("userID")) {
    customUserID = server.arg("userID");
    customUserID.trim(); // Remove any leading or trailing whitespace
    server.send(200, "text/plain", "User ID received!");
    
    // Immediately update Firebase with the new user ID
    updateFirebaseData();
  } else {
    server.send(400, "text/plain", "User ID not provided");
  }
}

void updateFirebaseData() {
  if (Firebase.ready()) {
    // Get current timestamp
    timestamp = getTime();
    Serial.print("time: ");
    Serial.println(timestamp);

    // Get formatted local time
    String formattedTime = getFormattedTime();
    Serial.print("Formatted time: ");
    Serial.println(formattedTime);

    // Get distance from HC-SR04 sensors
    float temperature = getDistance(trigPin1, echoPin1);
    float pulse = getDistance(trigPin2, echoPin2);
    float height = getDistance(trigPin3, echoPin3);
    float weight = getDistance(trigPin4, echoPin4);

    Serial.print("Temperature: ");
    Serial.println(temperature);
    Serial.print("Pulse: ");
    Serial.println(pulse);
    Serial.print("Height: ");
    Serial.println(height);
    Serial.print("Weight: ");
    Serial.println(weight);

    if (customUserID.length() > 0) {
      parentPath = "/UsersData/" + customUserID + "/readings/" + String(timestamp);

      json.set(temperaturePath, String(temperature));
      json.set(pulsePath, String(pulse));
      json.set(heightPath, String(height));
      json.set(weightPath, String(weight));
      json.set(timePath, formattedTime);
      Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
    } else {
      Serial.println("No custom user ID provided.");
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Initialize HC-SR04 sensor pins
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(trigPin3, OUTPUT);
  pinMode(echoPin3, INPUT);
  pinMode(trigPin4, OUTPUT);
  pinMode(echoPin4, INPUT);

  initWiFi();
  configTime(0, 0, "pool.ntp.org");

  // Assign the api key (required)
  config.api_key = API_KEY;

  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);

  // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);

  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while (auth.token.uid == "") {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);

  // Update database path
  databasePath = "/UsersData/" + uid + "/readings";

  // Initialize web server
  server.on("/", HTTP_GET, handleRoot);
  server.on("/submit", HTTP_POST, handleSubmit);
  server.begin();
}

void loop() {
  server.handleClient();

  // Check if it's time to send new readings to Firebase
  if (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0) {
    sendDataPrevMillis = millis();
    updateFirebaseData();
  }
}





//STABLE WITH WEBPAGE USER INPUT
//#include <Arduino.h>
//#include <WiFi.h>
//#include <Firebase_ESP_Client.h>
//#include <WebServer.h>
//#include "time.h"
//
//// Insert your network credentials
//#define WIFI_SSID "Ettronics Hightech"
//#define WIFI_PASSWORD "11111111"
//
//// Insert Firebase project API Key
//#define API_KEY "AIzaSyBAgeUvIPwMirPSPywX9YZJZ79htSpKO5c"
//
//// Insert Authorized Email and Corresponding Password
//#define USER_EMAIL "test@gmail.com"
//#define USER_PASSWORD "test@gmail.com"
//
//// Insert RTDB URL
//#define DATABASE_URL "https://wrud-5b418-default-rtdb.firebaseio.com/"
//
//// Define HC-SR04 sensor pins
//const int trigPin1 = 5;
//const int echoPin1 = 18;
//const int trigPin2 = 22;
//const int echoPin2 = 23;
//const int trigPin3 = 19;
//const int echoPin3 = 21;
//const int trigPin4 = 4;
//const int echoPin4 = 2;
//
//// Define Firebase objects
//FirebaseData fbdo;
//FirebaseAuth auth;
//FirebaseConfig config;
//
//// Variable to save USER UID
//String uid;
//
//// Database main path (to be updated in setup with the user UID)
//String databasePath;
//// Database child nodes
//String temperaturePath = "temperature";
//String pulsePath = "pulse";
//String heightPath = "height";
//String weightPath = "weight";
//String timePath = "timestamp";
//
//// Parent Node (to be updated in every loop)
//String parentPath;
//
//unsigned long timestamp;
//FirebaseJson json;
//
//// Timer variables (send new readings every three minutes)
//unsigned long sendDataPrevMillis = 0;
//unsigned long timerDelay = 180000; // 3 minutes
//
//WebServer server(80);
//String customUserID;
//
//void initWiFi() {
//  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
//  Serial.print("Connecting to WiFi ..");
//  while (WiFi.status() != WL_CONNECTED) {
//    Serial.print('.');
//    delay(1000);
//  }
//  Serial.println();
//  Serial.print("Connected to WiFi with IP: ");
//  Serial.println(WiFi.localIP());
//}
//
//unsigned long getTime() {
//  time_t now;
//  struct tm timeinfo;
//  if (!getLocalTime(&timeinfo)) {
//    return 0;
//  }
//  time(&now);
//  return now + 3600; //3600 seconds(1 hour) to my epoch time
//}
//
//// Function to get local time as a string, GMT+0
//String getFormattedTime() {
//  struct tm timeinfo;
//  if (!getLocalTime(&timeinfo)) {
//    return "Failed to obtain time";
//  }
//
//  // Add one hour to the obtained time
//  timeinfo.tm_hour += 1;
//
//  // Handle cases where hour exceeds 23 by adjusting the day
//  if (timeinfo.tm_hour >= 24) {
//    timeinfo.tm_hour -= 24;
//    timeinfo.tm_mday += 1; // Increment the day
//  }
//
//  char timeString[30];
//  strftime(timeString, sizeof(timeString), "%d-%m-%Y %H:%M:%S", &timeinfo);
//  return String(timeString);
//}
//
//// Function to get distance from HC-SR04 sensor
//float getDistance(int trigPin, int echoPin) {
//  // Clear the trigPin
//  digitalWrite(trigPin, LOW);
//  delayMicroseconds(2);
//
//  // Trigger the sensor
//  digitalWrite(trigPin, HIGH);
//  delayMicroseconds(10);
//  digitalWrite(trigPin, LOW);
//
//  // Read the echoPin
//  long duration = pulseIn(echoPin, HIGH);
//
//  // Calculate the distance
//  float distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
//
//  return distance;
//}
//
//void handleRoot() {
//  server.send(200, "text/html", R"rawliteral(
//    <!DOCTYPE html>
//    <html>
//    <head>
//      <title>Enter User ID</title>
//      <style>
//        body {
//          padding-left: 20px;
//        }
//        
//        #userID {
//            display: block;
//            height: 50px;
//            width: 50%;
//            background-color: rgba(255,255,255,0.07);
//            border-radius: 3px;
//            padding: 0 10px;
//            margin-top: 8px;
//            font-size: 14px;
//            font-weight: 300;
//        }
//        ::placeholder{
//            color: #e5e5e5;
//        }
//
//        .button{
//            margin-top: 10px;
//            width: 50%;
//            background-color: #0f7a73;
//    border: 1px solid #0f7a73;
//    color: #fff;
//    padding: 10px 0;
//    font-size: 18px;
//    font-weight: 600;
//    border-radius: 10px;
//    cursor: pointer;
//    transition: .3s;
//}
//.button:hover {
//    padding: 15px 0;
//    transition: .3s;
//}
//      </style>
//    </head>
//    <body>
//      <h1>Enter User ID to Initialize</h1>
//      <form action="/submit" method="POST">
//        <input type="text" id="userID" name="userID"><br>
//        <input class="button" type="submit" value="Submit">
//      </form>
//    </body>
//    </html>
//  )rawliteral");
//}
//
//void handleSubmit() {
//  if (server.hasArg("userID")) {
//    customUserID = server.arg("userID");
//    customUserID.trim(); // Remove any leading or trailing whitespace
//    server.send(200, "text/plain", "User ID received!");
//  } else {
//    server.send(400, "text/plain", "User ID not provided");
//  }
//}
//
//void setup() {
//  Serial.begin(115200);
//
//  // Initialize HC-SR04 sensor pins
//  pinMode(trigPin1, OUTPUT);
//  pinMode(echoPin1, INPUT);
//  pinMode(trigPin2, OUTPUT);
//  pinMode(echoPin2, INPUT);
//  pinMode(trigPin3, OUTPUT);
//  pinMode(echoPin3, INPUT);
//  pinMode(trigPin4, OUTPUT);
//  pinMode(echoPin4, INPUT);
//
//  initWiFi();
//  configTime(0, 0, "pool.ntp.org");
//
//  // Assign the api key (required)
//  config.api_key = API_KEY;
//
//  // Assign the user sign in credentials
//  auth.user.email = USER_EMAIL;
//  auth.user.password = USER_PASSWORD;
//
//  // Assign the RTDB URL (required)
//  config.database_url = DATABASE_URL;
//
//  Firebase.reconnectWiFi(true);
//  fbdo.setResponseSize(4096);
//
//  // Initialize the library with the Firebase authen and config
//  Firebase.begin(&config, &auth);
//
//  // Getting the user UID might take a few seconds
//  Serial.println("Getting User UID");
//  while (auth.token.uid == "") {
//    Serial.print('.');
//    delay(1000);
//  }
//  // Print user UID
//  uid = auth.token.uid.c_str();
//  Serial.print("User UID: ");
//  Serial.println(uid);
//
//  // Update database path
//  databasePath = "/UsersData/" + uid + "/readings";
//
//  // Initialize web server
//  server.on("/", HTTP_GET, handleRoot);
//  server.on("/submit", HTTP_POST, handleSubmit);
//  server.begin();
//}
//
//void loop() {
//  server.handleClient();
//
//  // Send new readings to database
//  if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)) {
//    sendDataPrevMillis = millis();
//
//    // Get current timestamp
//    timestamp = getTime();
//    Serial.print("time: ");
//    Serial.println(timestamp);
//
//    // Get formatted local time
//    String formattedTime = getFormattedTime();
//    Serial.print("Formatted time: ");
//    Serial.println(formattedTime);
//
//    // Get distance from HC-SR04 sensors
//    float temperature = getDistance(trigPin1, echoPin1);
//    float pulse = getDistance(trigPin2, echoPin2);
//    float height = getDistance(trigPin3, echoPin3);
//    float weight = getDistance(trigPin4, echoPin4);
//
//    Serial.print("Temperature: ");
//    Serial.println(temperature);
//    Serial.print("Pulse: ");
//    Serial.println(pulse);
//    Serial.print("Height: ");
//    Serial.println(height);
//    Serial.print("Weight: ");
//    Serial.println(weight);
//
//    if (customUserID.length() > 0) {
//      parentPath = "/UsersData/" + customUserID + "/readings/" + String(timestamp);
//
//      json.set(temperaturePath, String(temperature));
//      json.set(pulsePath, String(pulse));
//      json.set(heightPath, String(height));
//      json.set(weightPath, String(weight));
//      json.set(timePath, formattedTime);
//      Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
//    } else {
//      Serial.println("No custom user ID provided.");
//    }
//  }
//}




//// 4-U-SENSORS ASSUMED TO READ TEMPERATURE, PULSE, WEIGHT AND HEIGHT
//
//#include <Arduino.h>
//#include <WiFi.h>
//#include <Firebase_ESP_Client.h>
//#include "time.h"
//
//// Insert your network credentials
//#define WIFI_SSID "Ettronics Hightech"
//#define WIFI_PASSWORD "11111111"
//
//// Insert Firebase project API Key
//#define API_KEY "AIzaSyBAgeUvIPwMirPSPywX9YZJZ79htSpKO5c"
//
//// Insert Authorized Email and Corresponding Password
//#define USER_EMAIL "test@gmail.com"
//#define USER_PASSWORD "test@gmail.com"
//
//// Insert RTDB URL
//#define DATABASE_URL "https://wrud-5b418-default-rtdb.firebaseio.com/"
//
//// Define HC-SR04 sensor pins
//const int trigPin1 = 5;
//const int echoPin1 = 18;
//const int trigPin2 = 22;
//const int echoPin2 = 23;
//const int trigPin3 = 19;
//const int echoPin3 = 21;
//const int trigPin4 = 4;
//const int echoPin4 = 2;
//
//// Define Firebase objects
//FirebaseData fbdo;
//FirebaseAuth auth;
//FirebaseConfig config;
//
//// Variable to save USER UID
//String uid;
//
//// Database main path (to be updated in setup with the user UID)
//String databasePath;
//// Database child nodes
//String temperaturePath = "temperature";
//String pulsePath = "pulse";
//String heightPath = "height";
//String weightPath = "weight";
//String timePath = "timestamp";
//
//// Parent Node (to be updated in every loop)
//String parentPath;
//
//unsigned long timestamp;
//FirebaseJson json;
//
//// Timer variables (send new readings every three minutes)
//unsigned long sendDataPrevMillis = 0;
//unsigned long timerDelay = 180000; // 3 minutes
//
//// Initialize WiFi
//void initWiFi() {
//  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
//  Serial.print("Connecting to WiFi ..");
//  while (WiFi.status() != WL_CONNECTED) {
//    Serial.print('.');
//    delay(1000);
//  }
//  Serial.println();
//  Serial.print("Connected to WiFi with IP: ");
//  Serial.println(WiFi.localIP());
//}
//
//unsigned long getTime() {
//  time_t now;
//  struct tm timeinfo;
//  if (!getLocalTime(&timeinfo)) {
//    return 0;
//  }
//  time(&now);
//  return now + 3600; //3600 seconds(1 hour) to my epoch time
//}
//
//// Function to get local time as a string, GMT+0
////String getFormattedTime() {
////  struct tm timeinfo;
////  if (!getLocalTime(&timeinfo)) {
////    return "Failed to obtain time";
////  }
////  char timeString[30];
////  strftime(timeString, sizeof(timeString), "%d-%m-%Y %H:%M:%S", &timeinfo);
////  return String(timeString);
////}
//
//// algorithm to get time at GMT+1
//String getFormattedTime() {
//  struct tm timeinfo;
//  if (!getLocalTime(&timeinfo)) {
//    return "Failed to obtain time";
//  }
//
//  // Add one hour to the obtained time
//  timeinfo.tm_hour += 1;
//
//  // Handle cases where hour exceeds 23 by adjusting the day
//  if (timeinfo.tm_hour >= 24) {
//    timeinfo.tm_hour -= 24;
//    timeinfo.tm_mday += 1; // Increment the day
//  }
//
//  char timeString[30];
//  strftime(timeString, sizeof(timeString), "%d-%m-%Y %H:%M:%S", &timeinfo);
//  return String(timeString);
//}
//
//// Function to get distance from HC-SR04 sensor
//float getDistance(int trigPin, int echoPin) {
//  // Clear the trigPin
//  digitalWrite(trigPin, LOW);
//  delayMicroseconds(2);
//
//  // Trigger the sensor
//  digitalWrite(trigPin, HIGH);
//  delayMicroseconds(10);
//  digitalWrite(trigPin, LOW);
//
//  // Read the echoPin
//  long duration = pulseIn(echoPin, HIGH);
//
//  // Calculate the distance
//  float distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
//
//  return distance;
//}
//
//void setup() {
//  Serial.begin(115200);
//
//  // Initialize HC-SR04 sensor pins
//  pinMode(trigPin1, OUTPUT);
//  pinMode(echoPin1, INPUT);
//  pinMode(trigPin2, OUTPUT);
//  pinMode(echoPin2, INPUT);
//  pinMode(trigPin3, OUTPUT);
//  pinMode(echoPin3, INPUT);
//  pinMode(trigPin4, OUTPUT);
//  pinMode(echoPin4, INPUT);
//
//  initWiFi();
//  configTime(0, 0, "pool.ntp.org");
//
//  // Assign the api key (required)
//  config.api_key = API_KEY;
//
//  // Assign the user sign in credentials
//  auth.user.email = USER_EMAIL;
//  auth.user.password = USER_PASSWORD;
//
//  // Assign the RTDB URL (required)
//  config.database_url = DATABASE_URL;
//
//  Firebase.reconnectWiFi(true);
//  fbdo.setResponseSize(4096);
//
//  // Initialize the library with the Firebase authen and config
//  Firebase.begin(&config, &auth);
//
//  // Getting the user UID might take a few seconds
//  Serial.println("Getting User UID");
//  while (auth.token.uid == "") {
//    Serial.print('.');
//    delay(1000);
//  }
//  // Print user UID
//  uid = auth.token.uid.c_str();
//  Serial.print("User UID: ");
//  Serial.println(uid);
//
//  // Update database path
//  databasePath = "/UsersData/" + uid + "/readings";
//}
//
//void loop() {
//  // Send new readings to database
//  if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)) {
//    sendDataPrevMillis = millis();
//
//    // Get current timestamp
//    timestamp = getTime();
//    Serial.print("time: ");
//    Serial.println(timestamp);
//
//    // Get formatted local time
//    String formattedTime = getFormattedTime();
//    Serial.print("Formatted time: ");
//    Serial.println(formattedTime);
//
//    // Get distance from HC-SR04 sensors
//    float temperature = getDistance(trigPin1, echoPin1);
//    float pulse = getDistance(trigPin2, echoPin2);
//    float height = getDistance(trigPin3, echoPin3);
//    float weight = getDistance(trigPin4, echoPin4);
//
//    Serial.print("Temperature: ");
//    Serial.println(temperature);
//    Serial.print("Pulse: ");
//    Serial.println(pulse);
//    Serial.print("Height: ");
//    Serial.println(height);
//    Serial.print("Weight: ");
//    Serial.println(weight);
//
//    // Prompt user to enter custom ID
//    Serial.println("Enter a custom user ID: ");
//    while (Serial.available() == 0) {
//      // Wait for user input
//    }
//    String customUserID = Serial.readStringUntil('\n');
//    customUserID.trim();  // Remove any leading or trailing whitespace
//
//    parentPath = "/UsersData/" + customUserID + "/readings/" + String(timestamp);
//
//    json.set(temperaturePath, String(temperature));
//    json.set(pulsePath, String(pulse));
//    json.set(heightPath, String(height));
//    json.set(weightPath, String(weight));
//    json.set(timePath, formattedTime);
//    Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
//  }
//}














//// 4-U-SENSORS ASSUMED TO READ TEMPERATURE, PULSE, WEIGHT AND HEIGHT
//
//#include <Arduino.h>
//#include <WiFi.h>
//#include <Firebase_ESP_Client.h>
//#include "time.h"
//
//// Insert your network credentials
//#define WIFI_SSID "MTN_2.4G_3566C2"
//#define WIFI_PASSWORD "C616799D"
//
//// Insert Firebase project API Key
//#define API_KEY "AIzaSyBAgeUvIPwMirPSPywX9YZJZ79htSpKO5c"
//
//// Insert Authorized Email and Corresponding Password
//#define USER_EMAIL "test@gmail.com"
//#define USER_PASSWORD "test@gmail.com"
//
//// Insert RTDB URL
//#define DATABASE_URL "https://wrud-5b418-default-rtdb.firebaseio.com/"
//
//// Define HC-SR04 sensor pins
//const int trigPin1 = 5;
//const int echoPin1 = 18;
//const int trigPin2 = 22;
//const int echoPin2 = 23;
//const int trigPin3 = 19;
//const int echoPin3 = 21;
//const int trigPin4 = 4;
//const int echoPin4 = 2;
//
//// Define Firebase objects
//FirebaseData fbdo;
//FirebaseAuth auth;
//FirebaseConfig config;
//
//// Variable to save USER UID
//String uid;
//
//// Database main path (to be updated in setup with the user UID)
//String databasePath;
//// Database child nodes
//String temperaturePath = "temperature";
//String pulsePath = "pulse";
//String heightPath = "height";
//String weightPath = "weight";
//String timePath = "timestamp";
//
//// Parent Node (to be updated in every loop)
//String parentPath;
//
//unsigned long timestamp;
//FirebaseJson json;
//
//// Timer variables (send new readings every three minutes)
//unsigned long sendDataPrevMillis = 0;
//unsigned long timerDelay = 180000; // 3 minutes
//
//// Initialize WiFi
//void initWiFi() {
//  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
//  Serial.print("Connecting to WiFi ..");
//  while (WiFi.status() != WL_CONNECTED) {
//    Serial.print('.');
//    delay(1000);
//  }
//  Serial.println();
//  Serial.print("Connected to WiFi with IP: ");
//  Serial.println(WiFi.localIP());
//}
//
//unsigned long getTime() {
//  time_t now;
//  struct tm timeinfo;
//  if (!getLocalTime(&timeinfo)) {
//    return 0;
//  }
//  time(&now);
//  return now + 3600; //3600 seconds(1 hour) to my epoch time
//}
//
//// Function to get local time as a string, GMT+0
////String getFormattedTime() {
////  struct tm timeinfo;
////  if (!getLocalTime(&timeinfo)) {
////    return "Failed to obtain time";
////  }
////  char timeString[30];
////  strftime(timeString, sizeof(timeString), "%d-%m-%Y %H:%M:%S", &timeinfo);
////  return String(timeString);
////}
//
//// algorithm to get time at GMT+1
//String getFormattedTime() {
//  struct tm timeinfo;
//  if (!getLocalTime(&timeinfo)) {
//    return "Failed to obtain time";
//  }
//
//  // Add one hour to the obtained time
//  timeinfo.tm_hour += 1;
//
//  // Handle cases where hour exceeds 23 by adjusting the day
//  if (timeinfo.tm_hour >= 24) {
//    timeinfo.tm_hour -= 24;
//    timeinfo.tm_mday += 1; // Increment the day
//  }
//
//  char timeString[30];
//  strftime(timeString, sizeof(timeString), "%d-%m-%Y %H:%M:%S", &timeinfo);
//  return String(timeString);
//}
//
//// Function to get distance from HC-SR04 sensor
//float getDistance(int trigPin, int echoPin) {
//  // Clear the trigPin
//  digitalWrite(trigPin, LOW);
//  delayMicroseconds(2);
//
//  // Trigger the sensor
//  digitalWrite(trigPin, HIGH);
//  delayMicroseconds(10);
//  digitalWrite(trigPin, LOW);
//
//  // Read the echoPin
//  long duration = pulseIn(echoPin, HIGH);
//
//  // Calculate the distance
//  float distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
//
//  return distance;
//}
//
//void setup() {
//  Serial.begin(115200);
//
//  // Initialize HC-SR04 sensor pins
//  pinMode(trigPin1, OUTPUT);
//  pinMode(echoPin1, INPUT);
//  pinMode(trigPin2, OUTPUT);
//  pinMode(echoPin2, INPUT);
//  pinMode(trigPin3, OUTPUT);
//  pinMode(echoPin3, INPUT);
//  pinMode(trigPin4, OUTPUT);
//  pinMode(echoPin4, INPUT);
//
//  initWiFi();
//  configTime(0, 0, "pool.ntp.org");
//
//  // Assign the api key (required)
//  config.api_key = API_KEY;
//
//  // Assign the user sign in credentials
//  auth.user.email = USER_EMAIL;
//  auth.user.password = USER_PASSWORD;
//
//  // Assign the RTDB URL (required)
//  config.database_url = DATABASE_URL;
//
//  Firebase.reconnectWiFi(true);
//  fbdo.setResponseSize(4096);
//
//  // Initialize the library with the Firebase authen and config
//  Firebase.begin(&config, &auth);
//
//  // Getting the user UID might take a few seconds
//  Serial.println("Getting User UID");
//  while (auth.token.uid == "") {
//    Serial.print('.');
//    delay(1000);
//  }
//  // Print user UID
//  uid = auth.token.uid.c_str();
//  Serial.print("User UID: ");
//  Serial.println(uid);
//
//  // Update database path
//  databasePath = "/UsersData/" + uid + "/readings";
//}
//
//void loop() {
//  // Send new readings to database
//  if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)) {
//    sendDataPrevMillis = millis();
//
//    // Get current timestamp
//    timestamp = getTime();
//    Serial.print("time: ");
//    Serial.println(timestamp);
//
//    // Get formatted local time
//    String formattedTime = getFormattedTime();
//    Serial.print("Formatted time: ");
//    Serial.println(formattedTime);
//
//    // Get distance from HC-SR04 sensors
//    float temperature = getDistance(trigPin1, echoPin1);
//    float pulse = getDistance(trigPin2, echoPin2);
//    float height = getDistance(trigPin3, echoPin3);
//    float weight = getDistance(trigPin4, echoPin4);
//
//    Serial.print("Temperature: ");
//    Serial.println(temperature);
//    Serial.print("Pulse: ");
//    Serial.println(pulse);
//    Serial.print("Height: ");
//    Serial.println(height);
//    Serial.print("Weight: ");
//    Serial.println(weight);
//
//    parentPath = databasePath + "/" + String(timestamp);
//
//    json.set(temperaturePath, String(temperature));
//    json.set(pulsePath, String(pulse));
//    json.set(heightPath, String(height));
//    json.set(weightPath, String(weight));
//    json.set(timePath, formattedTime);
//    Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
//  }
//}






















//SPAGHETI CODES

//2_USENSORS
//#include <Arduino.h>
//#include <WiFi.h>
//#include <Firebase_ESP_Client.h>
//#include "time.h"
//
//// Insert your network credentials
//#define WIFI_SSID "MTN_2.4G_3566C2"
//#define WIFI_PASSWORD "C616799D"
//
//// Insert Firebase project API Key
//#define API_KEY "AIzaSyBAgeUvIPwMirPSPywX9YZJZ79htSpKO5c"
//
//// Insert Authorized Email and Corresponding Password
//#define USER_EMAIL "test@gmail.com"
//#define USER_PASSWORD "test@gmail.com"
//
//// Insert RTDB URL
//#define DATABASE_URL "https://wrud-5b418-default-rtdb.firebaseio.com/"
//
//// Define HC-SR04 sensor pins
//const int trigPin1 = 5;
//const int echoPin1 = 18;
//const int trigPin2 = 22;
//const int echoPin2 = 23;
//
//// Define Firebase objects
//FirebaseData fbdo;
//FirebaseAuth auth;
//FirebaseConfig config;
//
//// Variable to save USER UID
//String uid;
//
//// Database main path (to be updated in setup with the user UID)
//String databasePath;
//// Database child nodes
//String distancePath1 = "distance1";
//String distancePath2 = "distance2";
//String timePath = "timestamp";
//
//// Parent Node (to be updated in every loop)
//String parentPath;
//
//unsigned long timestamp;
//FirebaseJson json;
//
//// Timer variables (send new readings every three minutes)
//unsigned long sendDataPrevMillis = 0;
//unsigned long timerDelay = 180000; // 3 minutes
//
//// Initialize WiFi
//void initWiFi() {
//  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
//  Serial.print("Connecting to WiFi ..");
//  while (WiFi.status() != WL_CONNECTED) {
//    Serial.print('.');
//    delay(1000);
//  }
//  Serial.println();
//  Serial.print("Connected to WiFi with IP: ");
//  Serial.println(WiFi.localIP());
//}
//
//
//unsigned long getTime() {
//  time_t now;
//  struct tm timeinfo;
//  if (!getLocalTime(&timeinfo)) {
//    return 0;
//  }
//  time(&now);
//  return now + 3600; //3600 seconds(1 hour) to my epoch time
//}
//
//// Function to get local time as a string, GMT+0
////String getFormattedTime() {
////  struct tm timeinfo;
////  if (!getLocalTime(&timeinfo)) {
////    return "Failed to obtain time";
////  }
////  char timeString[30];
////  strftime(timeString, sizeof(timeString), "%d-%m-%Y %H:%M:%S", &timeinfo);
////  return String(timeString);
////}
//
//
////algorithm to get time at GMT+1
//
//String getFormattedTime() {
//  struct tm timeinfo;
//  if (!getLocalTime(&timeinfo)) {
//    return "Failed to obtain time";
//  }
//
//  // Add one hour to the obtained time
//  timeinfo.tm_hour += 1;
//  
//  // Handle cases where hour exceeds 23 by adjusting the day
//  if (timeinfo.tm_hour >= 24) {
//    timeinfo.tm_hour -= 24;
//    timeinfo.tm_mday += 1; // Increment the day
//  }
//
//  char timeString[30];
//  strftime(timeString, sizeof(timeString), "%d-%m-%Y %H:%M:%S", &timeinfo);
//  return String(timeString);
//}
//
//
//// Function to get distance from HC-SR04 sensor
//float getDistance(int trigPin, int echoPin) {
//  // Clear the trigPin
//  digitalWrite(trigPin, LOW);
//  delayMicroseconds(2);
//
//  // Trigger the sensor
//  digitalWrite(trigPin, HIGH);
//  delayMicroseconds(10);
//  digitalWrite(trigPin, LOW);
//
//  // Read the echoPin
//  long duration = pulseIn(echoPin, HIGH);
//
//  // Calculate the distance
//  float distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
//
//  return distance;
//}
//
//void setup() {
//  Serial.begin(115200);
//
//  // Initialize HC-SR04 sensor pins
//  pinMode(trigPin1, OUTPUT);
//  pinMode(echoPin1, INPUT);
//  pinMode(trigPin2, OUTPUT);
//  pinMode(echoPin2, INPUT);
//
//  initWiFi();
//  configTime(0, 0, "pool.ntp.org");
//
//  // Assign the api key (required)
//  config.api_key = API_KEY;
//
//  // Assign the user sign in credentials
//  auth.user.email = USER_EMAIL;
//  auth.user.password = USER_PASSWORD;
//
//  // Assign the RTDB URL (required)
//  config.database_url = DATABASE_URL;
//
//  Firebase.reconnectWiFi(true);
//  fbdo.setResponseSize(4096);
//
//  // Initialize the library with the Firebase authen and config
//  Firebase.begin(&config, &auth);
//
//  // Getting the user UID might take a few seconds
//  Serial.println("Getting User UID");
//  while (auth.token.uid == "") {
//    Serial.print('.');
//    delay(1000);
//  }
//  // Print user UID
//  uid = auth.token.uid.c_str();
//  Serial.print("User UID: ");
//  Serial.println(uid);
//
//  // Update database path
//  databasePath = "/UsersData/" + uid + "/readings";
//}
//
//void loop() {
//  // Send new readings to database
//  if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)) {
//    sendDataPrevMillis = millis();
//
//    // Get current timestamp
//    timestamp = getTime();
//    Serial.print("time: ");
//    Serial.println(timestamp);
//
//    // Get formatted local time
//    String formattedTime = getFormattedTime();
//    Serial.print("Formatted time: ");
//    Serial.println(formattedTime );
//
//    // Get distance from HC-SR04 sensors
//    float distance1 = getDistance(trigPin1, echoPin1);
//    float distance2 = getDistance(trigPin2, echoPin2);
//    Serial.print("Distance 1: ");
//    Serial.println(distance1);
//    Serial.print("Distance 2: ");
//    Serial.println(distance2);
//
//    parentPath = databasePath + "/" + String(timestamp);
//
//    json.set(distancePath1, String(distance1));
//    json.set(distancePath2, String(distance2));
//    json.set(timePath, formattedTime);
//    Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
//  }
//}




////4 U-SENSORS
//#include <Arduino.h>
//#include <WiFi.h>
//#include <Firebase_ESP_Client.h>
//#include "time.h"
//
//// Insert your network credentials
//#define WIFI_SSID "MTN_2.4G_3566C2"
//#define WIFI_PASSWORD "C616799D"
//
//// Insert Firebase project API Key
//#define API_KEY "AIzaSyBAgeUvIPwMirPSPywX9YZJZ79htSpKO5c"
//
//// Insert Authorized Email and Corresponding Password
//#define USER_EMAIL "test@gmail.com"
//#define USER_PASSWORD "test@gmail.com"
//
//// Insert RTDB URL
//#define DATABASE_URL "https://wrud-5b418-default-rtdb.firebaseio.com/"
//
//// Define HC-SR04 sensor pins
//const int trigPin1 = 5;
//const int echoPin1 = 18;
//const int trigPin2 = 22;
//const int echoPin2 = 23;
//const int trigPin3 = 19;
//const int echoPin3 = 21;
//const int trigPin4 = 4;
//const int echoPin4 = 2;
//
//// Define Firebase objects
//FirebaseData fbdo;
//FirebaseAuth auth;
//FirebaseConfig config;
//
//// Variable to save USER UID
//String uid;
//
//// Database main path (to be updated in setup with the user UID)
//String databasePath;
//// Database child nodes
//String distancePath1 = "temperature";
//String distancePath2 = "height";
//String distancePath3 = "weight";
//String distancePath4 = "pulse";
//String timePath = "timestamp";
//
//// Parent Node (to be updated in every loop)
//String parentPath;
//
//unsigned long timestamp;
//FirebaseJson json;
//
//// Timer variables (send new readings every three minutes)
//unsigned long sendDataPrevMillis = 0;
//unsigned long timerDelay = 180000; // 3 minutes
//
//// Initialize WiFi
//void initWiFi() {
//  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
//  Serial.print("Connecting to WiFi ..");
//  while (WiFi.status() != WL_CONNECTED) {
//    Serial.print('.');
//    delay(1000);
//  }
//  Serial.println();
//  Serial.print("Connected to WiFi with IP: ");
//  Serial.println(WiFi.localIP());
//}
//
//unsigned long getTime() {
//  time_t now;
//  struct tm timeinfo;
//  if (!getLocalTime(&timeinfo)) {
//    return 0;
//  }
//  time(&now);
//  return now + 3600; //3600 seconds(1 hour) to my epoch time
//}
//
//// Function to get local time as a string, GMT+0
////String getFormattedTime() {
////  struct tm timeinfo;
////  if (!getLocalTime(&timeinfo)) {
////    return "Failed to obtain time";
////  }
////  char timeString[30];
////  strftime(timeString, sizeof(timeString), "%d-%m-%Y %H:%M:%S", &timeinfo);
////  return String(timeString);
////}
//
//// algorithm to get time at GMT+1
//String getFormattedTime() {
//  struct tm timeinfo;
//  if (!getLocalTime(&timeinfo)) {
//    return "Failed to obtain time";
//  }
//
//  // Add one hour to the obtained time
//  timeinfo.tm_hour += 1;
//
//  // Handle cases where hour exceeds 23 by adjusting the day
//  if (timeinfo.tm_hour >= 24) {
//    timeinfo.tm_hour -= 24;
//    timeinfo.tm_mday += 1; // Increment the day
//  }
//
//  char timeString[30];
//  strftime(timeString, sizeof(timeString), "%d-%m-%Y %H:%M:%S", &timeinfo);
//  return String(timeString);
//}
//
//// Function to get distance from HC-SR04 sensor
//float getDistance(int trigPin, int echoPin) {
//  // Clear the trigPin
//  digitalWrite(trigPin, LOW);
//  delayMicroseconds(2);
//
//  // Trigger the sensor
//  digitalWrite(trigPin, HIGH);
//  delayMicroseconds(10);
//  digitalWrite(trigPin, LOW);
//
//  // Read the echoPin
//  long duration = pulseIn(echoPin, HIGH);
//
//  // Calculate the distance
//  float distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
//
//  return distance;
//}
//
//void setup() {
//  Serial.begin(115200);
//
//  // Initialize HC-SR04 sensor pins
//  pinMode(trigPin1, OUTPUT);
//  pinMode(echoPin1, INPUT);
//  pinMode(trigPin2, OUTPUT);
//  pinMode(echoPin2, INPUT);
//  pinMode(trigPin3, OUTPUT);
//  pinMode(echoPin3, INPUT);
//  pinMode(trigPin4, OUTPUT);
//  pinMode(echoPin4, INPUT);
//
//  initWiFi();
//  configTime(0, 0, "pool.ntp.org");
//
//  // Assign the api key (required)
//  config.api_key = API_KEY;
//
//  // Assign the user sign in credentials
//  auth.user.email = USER_EMAIL;
//  auth.user.password = USER_PASSWORD;
//
//  // Assign the RTDB URL (required)
//  config.database_url = DATABASE_URL;
//
//  Firebase.reconnectWiFi(true);
//  fbdo.setResponseSize(4096);
//
//  // Initialize the library with the Firebase authen and config
//  Firebase.begin(&config, &auth);
//
//  // Getting the user UID might take a few seconds
//  Serial.println("Getting User UID");
//  while (auth.token.uid == "") {
//    Serial.print('.');
//    delay(1000);
//  }
//  // Print user UID
//  uid = auth.token.uid.c_str();
//  Serial.print("User UID: ");
//  Serial.println(uid);
//
//  // Update database path
//  databasePath = "/UsersData/" + uid + "/readings";
//}
//
//void loop() {
//  // Send new readings to database
//  if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)) {
//    sendDataPrevMillis = millis();
//
//    // Get current timestamp
//    timestamp = getTime();
//    Serial.print("time: ");
//    Serial.println(timestamp);
//
//    // Get formatted local time
//    String formattedTime = getFormattedTime();
//    Serial.print("Formatted time: ");
//    Serial.println(formattedTime);
//
//    // Get distance from HC-SR04 sensors
//    float distance1 = getDistance(trigPin1, echoPin1);
//    float distance2 = getDistance(trigPin2, echoPin2);
//    float distance3 = getDistance(trigPin3, echoPin3);
//    float distance4 = getDistance(trigPin4, echoPin4);
//
//    Serial.print("Distance 1: ");
//    Serial.println(distance1);
//    Serial.print("Distance 2: ");
//    Serial.println(distance2);
//    Serial.print("Distance 3: ");
//    Serial.println(distance3);
//    Serial.print("Distance 4: ");
//    Serial.println(distance4);
//
//    parentPath = databasePath + "/" + String(timestamp);
//
//    json.set(distancePath1, String(distance1));
//    json.set(distancePath2, String(distance2));
//    json.set(distancePath3, String(distance3));
//    json.set(distancePath4, String(distance4));
//    json.set(timePath, formattedTime);
//    Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
//  }
//}

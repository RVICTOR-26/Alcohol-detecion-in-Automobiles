#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <MPU6050.h>

TinyGPSPlus gps;      // GPS object
MPU6050 mpu;          // MPU6050 object
SoftwareSerial gpsSerial(4, 3);    // RX, TX for GPS
SoftwareSerial gsmSerial(7, 8);    // RX, TX for GSM

const int alcoholPin = A0;        // Alcohol sensor pin
const int buzzerPin = 8;          // Buzzer pin
const int ledPin = 9;             // LED pin
const int in1 = 10;               // Motor driver IN1
const int in2 = 11;               // Motor driver IN2
const int enA = 12;               // Motor driver ENA

void setup() {
  Serial.begin(9600);             // Initialize serial communication
  gpsSerial.begin(9600);          // GPS module
  gsmSerial.begin(9600);          // GSM module

  pinMode(alcoholPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enA, OUTPUT);
  
  digitalWrite(enA, HIGH);        // Enable motor
  
  // MPU6050 Initialization
  Wire.begin();
  mpu.initialize();
  
  // GSM Initialization
  sendSMS("System initialized and ready.");
}

void loop() {
  // Alcohol detection
  int alcoholValue = analogRead(alcoholPin);
  
  // Alcohol detection threshold (adjust as needed)
  if (alcoholValue > 400) {
    Serial.println("Alcohol detected!");
    stopMotor();
    alertDriver();
    sendLocation();  // Send GPS location via SMS
  }
  
  // GPS Data
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
    if (gps.location.isValid()) {
      Serial.print("Latitude: "); Serial.println(gps.location.lat(), 6);
      Serial.print("Longitude: "); Serial.println(gps.location.lng(), 6);
    }
  }
  
  delay(1000);
}

void stopMotor() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
}

void alertDriver() {
  digitalWrite(buzzerPin, HIGH);
  digitalWrite(ledPin, HIGH);
  delay(2000);    // Sound the alarm for 2 seconds
  digitalWrite(buzzerPin, LOW);
  digitalWrite(ledPin, LOW);
}

void sendLocation() {
  if (gps.location.isValid()) {
    String message = "Emergency! Alcohol detected. Location: ";
    message += "Lat: " + String(gps.location.lat(), 6);
    message += ", Lon: " + String(gps.location.lng(), 6);
    sendSMS(message);
  } else {
    sendSMS("GPS signal not available.");
  }
}

void sendSMS(String text) {
  gsmSerial.print("AT+CMGF=1\r");            // Set SMS mode
  delay(1000);
  gsmSerial.print("AT+CMGS=\"+1234567890\"\r");  // Replace with recipient's number
  delay(1000);
  gsmSerial.print(text);    // Message body
  delay(1000);
  gsmSerial.write(26);      // Send Ctrl+Z to indicate end of message
  delay(1000);
}
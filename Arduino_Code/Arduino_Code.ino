/*
 * Multi-Sensor System - Temperature, Humidity, Sound, and Acceleration
 * - DHT11: Temperature & Humidity (Pin 5)
 * - Sound Sensor: Digital Output (Pin 4)
 * - MMA8452: 3-Axis Acceleration (X, Y, Z)
 */

#include <DHT.h>
#include <Wire.h>
#include <SparkFun_MMA8452Q.h>

// ===== PIN CONFIGURATION =====
#define DHTPIN 5              // DHT sensor data pin
#define SOUND_DIGITAL_PIN 4   // Sound sensor digital output
#define LED_PIN 13            // Status LED

// ===== SENSOR CONFIGURATION =====
#define DHTTYPE DHT11         // DHT11 sensor

// ===== SENSOR OBJECTS =====
DHT dht(DHTPIN, DHTTYPE);
MMA8452Q accel;

// ===== SENSOR VARIABLES =====
float temperature = 0.0;
float humidity = 0.0;
int soundDetected = 0;
float accelX = 0.0;
float accelY = 0.0;
float accelZ = 0.0;
float offsetX = 0.0;
float offsetY = 0.0;
float offsetZ = 0.0;

bool accelAvailable = false;

// ===== SETUP =====
void setup() {
  Serial.begin(9600);
  
  Serial.println("\n=== Multi-Sensor System ===\n");
  
  // Initialize pins
  pinMode(SOUND_DIGITAL_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  
  // Initialize DHT11
  Serial.println("Initializing DHT11...");
  dht.begin();
  delay(2000);
  Serial.println("DHT11 Ready!");
  
  // Initialize MMA8452
  Serial.println("\nInitializing MMA8452...");
  Wire.begin();
  
  if (accel.init()) {
    Serial.println("MMA8452 Ready!");
    accel.setScale(SCALE_2G);
    accel.setDataRate(ODR_50);
    accelAvailable = true;
    
    // Calibrate
    Serial.println("\nCalibrating accelerometer...");
    Serial.println("Keep Arduino FLAT and STILL!");
    delay(3000);
    
    calibrateAccelerometer();
    Serial.println("Calibration complete!\n");
    
  } else {
    Serial.println("MMA8452 not found!");
    accelAvailable = false;
  }
  
  Serial.println("=== System Ready ===\n");
  delay(1000);
}

// ===== MAIN LOOP =====
void loop() {
  // Read all sensors
  readDHT();
  readSound();
  if (accelAvailable) {
    readAccelerometer();
  }
  
  // Display results
  displayData();
  
  // Wait 2 seconds before next reading
  delay(2000);
}

// ===== DHT FUNCTIONS =====
void readDHT() {
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  
  if (isnan(temperature) || isnan(humidity)) {
    temperature = 0.0;
    humidity = 0.0;
  }
}

// ===== SOUND FUNCTIONS =====
void readSound() {
  soundDetected = digitalRead(SOUND_DIGITAL_PIN);
  
  if (soundDetected == HIGH) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
}

// ===== ACCELEROMETER FUNCTIONS =====
void calibrateAccelerometer() {
  float sumX = 0, sumY = 0, sumZ = 0;
  int samples = 100;
  
  for (int i = 0; i < samples; i++) {
    if (accel.available()) {
      accel.read();
      sumX += accel.cx;
      sumY += accel.cy;
      sumZ += accel.cz;
      delay(10);
    }
  }
  
  offsetX = sumX / samples;
  offsetY = sumY / samples;
  offsetZ = (sumZ / samples) - 1.0;
}

void readAccelerometer() {
  if (accel.available()) {
    accel.read();
    
    accelX = (accel.cx - offsetX) * 9.81;
    accelY = (accel.cy - offsetY) * 9.81;
    accelZ = (accel.cz - offsetZ) * 9.81;
  }
}

// ===== DISPLAY FUNCTIONS =====
void displayData() {
  Serial.println("========== SENSOR READINGS ==========");
  
  // Temperature & Humidity
  Serial.println("\n--- Temperature & Humidity ---");
  Serial.print("Temperature: ");
  Serial.print(temperature, 2);
  Serial.println(" °C");
  Serial.print("Humidity: ");
  Serial.print(humidity, 2);
  Serial.println(" %");
  
  // Sound
  Serial.println("\n--- Sound Sensor ---");
  Serial.print("Sound: ");
  if (soundDetected == HIGH) {
    Serial.println("DETECTED");
  } else {
    Serial.println("Quiet");
  }
  
  // Acceleration
  if (accelAvailable) {
    Serial.println("\n--- Acceleration ---");
    Serial.print("X: ");
    Serial.print(accelX, 2);
    Serial.println(" m/s²");
    Serial.print("Y: ");
    Serial.print(accelY, 2);
    Serial.println(" m/s²");
    Serial.print("Z: ");
    Serial.print(accelZ, 2);
    Serial.println(" m/s²");
  }
  
  Serial.println("\n=====================================\n");
}

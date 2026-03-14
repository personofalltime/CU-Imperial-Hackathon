/*
 * Multi-Sensor Monitoring System with Excel Data Logging
 * - DHT11: Temperature & Humidity (Pin 5)
 * - Sound Sensor: Digital Output Only (Pin 4)
 * - MMA8452: 3-Axis Accelerometer with Velocity & Displacement
 * 
 * Outputs CSV format for Excel import
 */

#include <DHT.h>
#include <Wire.h>
#include <SparkFun_MMA8452Q.h>

// ===== PIN CONFIGURATION =====
#define DHTPIN 5              // DHT sensor data pin
#define SOUND_DIGITAL_PIN 4   // Sound sensor digital output
#define LED_PIN 13            // Status LED

// ===== SENSOR CONFIGURATION =====
#define DHTTYPE DHT11         // DHT11 sensor (Changed from DHT22)

// ===== SENSOR OBJECTS =====
DHT dht(DHTPIN, DHTTYPE);
MMA8452Q accel;

// ===== TIMING VARIABLES =====
unsigned long previousMillis = 0;
const long interval = 2000;        // Read sensors every 2 seconds
unsigned long lastAccelTime = 0;
unsigned long startTime = 0;       // Track elapsed time
float deltaTime = 0.0;

// ===== DHT VARIABLES =====
float temperature = 0.0;
float humidity = 0.0;

// ===== SOUND SENSOR VARIABLES (DIGITAL ONLY) =====
int soundDigital = 0;              // Current digital state
unsigned long soundEventCount = 0; // Total number of sound events detected
unsigned long soundEventsInWindow = 0; // Events in current time window
unsigned long lastSoundTime = 0;   // Last time sound was detected
float soundFrequency = 0.0;        // Sound events per second

// Sound event tracking for the current interval
int soundEventsThisSecond = 0;
bool lastSoundState = LOW;

// ===== ACCELEROMETER VARIABLES =====
float accelX = 0.0, accelY = 0.0, accelZ = 0.0;
float prevAccelX = 0.0, prevAccelY = 0.0, prevAccelZ = 0.0;
float velocityX = 0.0, velocityY = 0.0, velocityZ = 0.0;
float displacementX = 0.0, displacementY = 0.0, displacementZ = 0.0;
float offsetX = 0.0, offsetY = 0.0, offsetZ = 0.0;

const float ACCEL_THRESHOLD = 0.1;

// ===== DATA LOGGING =====
bool headerPrinted = false;

// ===== SETUP =====
void setup() {
  Serial.begin(9600);
  
  // Wait for serial connection
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  
  Serial.println("=== Multi-Sensor System Initializing ===");
  
  // Initialize pins
  pinMode(SOUND_DIGITAL_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  
  // Initialize DHT sensor
  Serial.println("Initializing DHT11 sensor...");
  dht.begin();
  delay(2000);  // Give DHT sensor time to stabilize
  
  // Test DHT sensor
  float testTemp = dht.readTemperature();
  float testHum = dht.readHumidity();
  if (isnan(testTemp) || isnan(testHum)) {
    Serial.println("WARNING: DHT11 sensor not responding!");
    Serial.println("Check wiring on Pin 5");
  } else {
    Serial.print("DHT11 sensor: OK - Temp: ");
    Serial.print(testTemp);
    Serial.print("C, Humidity: ");
    Serial.print(testHum);
    Serial.println("%");
  }
  
  // Initialize MMA8452 Accelerometer
  Serial.println("Initializing MMA8452 accelerometer...");
  Wire.begin();
  
  if (accel.init()) {
    Serial.println("MMA8452 Accelerometer: OK");
    accel.setScale(SCALE_2G);
    accel.setDataRate(ODR_50);
    calibrateAccelerometer();
  } else {
    Serial.println("WARNING: MMA8452 not found!");
    Serial.println("Check I2C wiring (SDA/SCL)");
  }
  
  Serial.println("=== Initialization Complete ===\n");
  delay(1000);
  
  // Print CSV header
  printCSVHeader();
  
  startTime = millis();
  lastAccelTime = millis();
  lastSoundTime = millis();
}

// ===== MAIN LOOP =====
void loop() {
  unsigned long currentMillis = millis();
  
  // Continuously monitor sound sensor for events
  readSoundSensor();
  
  // Read accelerometer at high frequency
  readAccelerometer();
  
  // Read other sensors and log data at regular intervals
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    // Calculate elapsed time in seconds
    float elapsedTime = (currentMillis - startTime) / 1000.0;
    
    // Calculate sound frequency (events per interval)
    soundFrequency = soundEventsThisSecond / (interval / 1000.0);
    
    // Read DHT sensor
    readDHT();
    
    // Output data in CSV format
    printCSVData(elapsedTime);
    
    // Reset sound event counter for next interval
    soundEventsThisSecond = 0;
  }
}

// ===== CSV OUTPUT FUNCTIONS =====
void printCSVHeader() {
  Serial.println("Time(s),Temperature(C),Humidity(%),SoundDetected,SoundEvents,SoundFreq(Hz),AccelX(m/s2),AccelY(m/s2),AccelZ(m/s2),VelX(m/s),VelY(m/s),VelZ(m/s),DispX(m),DispY(m),DispZ(m)");
  headerPrinted = true;
}

void printCSVData(float time) {
  // Time
  Serial.print(time, 2);
  Serial.print(",");
  
  // Temperature
  Serial.print(temperature, 2);
  Serial.print(",");
  
  // Humidity
  Serial.print(humidity, 2);
  Serial.print(",");
  
  // Sound Digital State (0 or 1)
  Serial.print(soundDigital);
  Serial.print(",");
  
  // Total Sound Events Count
  Serial.print(soundEventCount);
  Serial.print(",");
  
  // Sound Frequency (events per second)
  Serial.print(soundFrequency, 2);
  Serial.print(",");
  
  // Acceleration X, Y, Z
  Serial.print(accelX, 4);
  Serial.print(",");
  Serial.print(accelY, 4);
  Serial.print(",");
  Serial.print(accelZ, 4);
  Serial.print(",");
  
  // Velocity X, Y, Z
  Serial.print(velocityX, 4);
  Serial.print(",");
  Serial.print(velocityY, 4);
  Serial.print(",");
  Serial.print(velocityZ, 4);
  Serial.print(",");
  
  // Displacement X, Y, Z
  Serial.print(displacementX, 4);
  Serial.print(",");
  Serial.print(displacementY, 4);
  Serial.print(",");
  Serial.print(displacementZ, 4);
  
  Serial.println();
}

// ===== DHT SENSOR FUNCTIONS =====
void readDHT() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  
  // Check if readings failed
  if (isnan(humidity) || isnan(temperature)) {
    humidity = 0.0;
    temperature = 0.0;
  }
  
  // Sanity check - valid temperature range for DHT11
  if (temperature < 0 || temperature > 50) {
    temperature = 0.0;
  }
  
  // Sanity check - valid humidity range for DHT11
  if (humidity < 20 || humidity > 90) {
    humidity = 0.0;
  }
}

// ===== SOUND SENSOR FUNCTIONS (DIGITAL ONLY) =====
void readSoundSensor() {
  // Read current digital state
  soundDigital = digitalRead(SOUND_DIGITAL_PIN);
  
  // Detect rising edge (sound event)
  if (soundDigital == HIGH && lastSoundState == LOW) {
    soundEventCount++;           // Increment total event counter
    soundEventsThisSecond++;     // Increment events in current interval
    lastSoundTime = millis();    // Update last detection time
    
    // Blink LED on sound detection
    digitalWrite(LED_PIN, HIGH);
  } else if (soundDigital == LOW) {
    digitalWrite(LED_PIN, LOW);
  }
  
  // Update last state
  lastSoundState = soundDigital;
}

// Get sound status as string
String getSoundStatus() {
  if (soundDigital == HIGH) {
    return "DETECTED";
  } else {
    return "Quiet";
  }
}

// Get time since last sound event
float getTimeSinceLastSound() {
  return (millis() - lastSoundTime) / 1000.0;
}

// ===== ACCELEROMETER FUNCTIONS =====
void calibrateAccelerometer() {
  Serial.println("Calibrating accelerometer... Keep device still!");
  
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
  offsetZ = (sumZ / samples) - 1.0;  // Subtract 1g for gravity
  
  Serial.println("Calibration complete!");
  Serial.print("Offsets - X: "); Serial.print(offsetX, 4);
  Serial.print(" Y: "); Serial.print(offsetY, 4);
  Serial.print(" Z: "); Serial.println(offsetZ, 4);
}

void readAccelerometer() {
  if (accel.available()) {
    accel.read();
    
    unsigned long currentTime = millis();
    deltaTime = (currentTime - lastAccelTime) / 1000.0;
    lastAccelTime = currentTime;
    
    // Get acceleration in m/s²
    accelX = (accel.cx - offsetX) * 9.81;
    accelY = (accel.cy - offsetY) * 9.81;
    accelZ = (accel.cz - offsetZ) * 9.81;
    
    // Low-pass filter
    accelX = 0.8 * prevAccelX + 0.2 * accelX;
    accelY = 0.8 * prevAccelY + 0.2 * accelY;
    accelZ = 0.8 * prevAccelZ + 0.2 * accelZ;
    
    // Apply threshold
    if (abs(accelX) < ACCEL_THRESHOLD) accelX = 0.0;
    if (abs(accelY) < ACCEL_THRESHOLD) accelY = 0.0;
    if (abs(accelZ) < ACCEL_THRESHOLD) accelZ = 0.0;
    
    // Calculate velocity
    velocityX += accelX * deltaTime;
    velocityY += accelY * deltaTime;
    velocityZ += accelZ * deltaTime;
    
    // Velocity threshold
    if (abs(velocityX) < 0.01) velocityX = 0.0;
    if (abs(velocityY) < 0.01) velocityY = 0.0;
    if (abs(velocityZ) < 0.01) velocityZ = 0.0;
    
    // Calculate displacement
    displacementX += velocityX * deltaTime + 0.5 * accelX * deltaTime * deltaTime;
    displacementY += velocityY * deltaTime + 0.5 * accelY * deltaTime * deltaTime;
    displacementZ += velocityZ * deltaTime + 0.5 * accelZ * deltaTime * deltaTime;
    
    // Store previous
    prevAccelX = accelX;
    prevAccelY = accelY;
    prevAccelZ = accelZ;
  }
}

// Reset displacement and velocity
void resetMotionTracking() {
  velocityX = velocityY = velocityZ = 0.0;
  displacementX = displacementY = displacementZ = 0.0;
}

// Reset sound event counter
void resetSoundCounter() {
  soundEventCount = 0;
  soundEventsThisSecond = 0;
}
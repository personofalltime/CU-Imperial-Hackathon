#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define DHTPIN 5
#define DHTTYPE DHT11
#define MIC_PIN 4

// **************************** ACCELEROMETER
#define MMA_ADDR 0x1C

int COLOR_1 = 0;
int STATE = 0;
int lastButtonState = HIGH;

#define BTNPIN 2
#define LED_RED 9
#define LED_GREEN 10
#define LED_BLUE 11

void writeRegister(byte reg, byte value) {
  Wire.beginTransmission(MMA_ADDR);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

byte readRegister(byte reg) {
  Wire.beginTransmission(MMA_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);

  Wire.requestFrom(MMA_ADDR, 1);

  if (Wire.available()) {
    return Wire.read();
  }

  return 0;
}

void readAccelRaw(int16_t &x, int16_t &y, int16_t &z) {

  Wire.beginTransmission(MMA_ADDR);
  Wire.write(0x01);
  Wire.endTransmission(false);

  Wire.requestFrom(MMA_ADDR, 6);

  if (Wire.available() == 6) {

    byte xhi = Wire.read();
    byte xlo = Wire.read();
    byte yhi = Wire.read();
    byte ylo = Wire.read();
    byte zhi = Wire.read();
    byte zlo = Wire.read();

    x = ((int16_t)(xhi << 8 | xlo)) >> 2;
    y = ((int16_t)(yhi << 8 | ylo)) >> 2;
    z = ((int16_t)(zhi << 8 | zlo)) >> 2;
  }
}

// ***************************************

DHT dht(DHTPIN, DHTTYPE);

void setup() {

  Serial.begin(115200);

  pinMode(MIC_PIN, INPUT);
  dht.begin();

  Wire.begin();
  delay(1000);

  byte whoami = readRegister(0x0D);

  writeRegister(0x2A, 0x01);

  pinMode(BTNPIN, INPUT_PULLUP);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
}

void loop() {

  float humidity = dht.readHumidity();
  float temp = dht.readTemperature();
  int mic = digitalRead(MIC_PIN);

  int16_t ax = 0;
  int16_t ay = 0;
  int16_t az = 0;

  readAccelRaw(ax, ay, az);

  int buttonState = digitalRead(BTNPIN);

  // Toggle LED on button press
  if (buttonState == LOW && lastButtonState == HIGH) {

    if (COLOR_1 == 0) {
      COLOR_1 = 255;
      STATE = 1;
    } else {
      COLOR_1 = 0;
      STATE = 0;
    }

    delay(50); // debounce
  }

  lastButtonState = buttonState;

  // RGB LED
  analogWrite(LED_RED, COLOR_1);
  analogWrite(LED_GREEN, 255 - COLOR_1);
  analogWrite(LED_BLUE, COLOR_1);

  if (!isnan(temp) && !isnan(humidity)) {

    Serial.print("{\"temp\":");
    Serial.print(temp);

    Serial.print(",\"hum\":");
    Serial.print(humidity);

    Serial.print(",\"mic\":");
    Serial.print(mic);

    Serial.print(",\"ax\":");
    Serial.print(ax);

    Serial.print(",\"ay\":");
    Serial.print(ay);

    Serial.print(",\"az\":");
    Serial.print(az);

    Serial.print(",\"stat\":");
    Serial.print(STATE);

    Serial.println("}");
  }

  delay(50);
}

#include <Arduino.h>
  
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_VEML7700.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>


// VEML7700 Lux sensor setup
Adafruit_VEML7700 veml = Adafruit_VEML7700();

// OLED display setup
Adafruit_SSD1306 display(128, 32, &Wire);

// BME280 sensor setup
Adafruit_BME280 bme;

// Vibration motor pin
const int motorPin = A1;

// Filter coefficients
float alpha = 0.2; 
float filteredTemperature = 0.0;
float filteredHumidity = 0.0;
float filteredLux = 0.0;

void setup() {
  Wire.begin();
  Serial.begin(9600);

  // Initialize BME280
  if (!bme.begin(0x76)) {  
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  // Initialize VEML7700
  if (!veml.begin()) {
    Serial.println("No VEML7700 sensor found... check your wiring?");
    while (1);
  }

  // Initialize SSD1306 OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.display();
  delay(500);
  display.setTextSize(1);
  display.setTextColor(WHITE);

  // Configure VEML7700 settings
  veml.setGain(VEML7700_GAIN_1);
  veml.setIntegrationTime(VEML7700_IT_100MS);

  // Initialize the vibration motor pin as output
  pinMode(motorPin, OUTPUT);
}

void loop() {
  // Read data from BME280
  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();

  // Read lux from VEML7700
  float lux = veml.readLux();

  // Apply exponential decay filter
  filteredTemperature = alpha * temperature + (1 - alpha) * filteredTemperature;
  filteredHumidity = alpha * humidity + (1 - alpha) * filteredHumidity;
  filteredLux = alpha * lux + (1 - alpha) * filteredLux;

  // Display filtered data on OLED
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("Temp: ");
  display.print(filteredTemperature, 1);
  display.print(" C");
  display.setCursor(0, 10);
  display.print("Hum: ");
  display.print(filteredHumidity, 1);
  display.print(" %");
  display.setCursor(0, 20);
  display.print("Lux: ");
  display.print(filteredLux);
  display.display();

  if (filteredLux < 100) { 
    digitalWrite(motorPin, HIGH);
  } else {
    digitalWrite(motorPin, LOW);
  }

  delay(100);
}
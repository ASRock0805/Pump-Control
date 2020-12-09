/*
  Pump Controller Using Serial and I2C

      Set the pump speed percentage (PWM) through the serial port and I^2C from another MCU.
  Every second, print the percentage of pump speed.

  Input range: 50 - 100 (%)

  - Kamoer KDS Peristaltic Pump (KDS-FE-2-S17B)
      12V DC brush motor

  Created 30 Nov. 2020
  by Yi-Xuan Wang

  Reference:
  http://kamoer.com/Products/showproduct.php?id=234
*/

/*--- Preprocessor ---*/
#include <Wire.h>   // Import Wire library for Inter-Integrated Circuit (I^2C)

#define MAIN_CTRL 1 // Address of main controller w/ sensor
#define PUMP_CTRL 2 // Address of pump controller

#define pumpPin 5   // Pin location of the pump (~D5) w/ PWM

/*--- Constants ---*/
const unsigned long baudSpeed = 115200; // Sets the data rate in bits per second (baud) for serial data transmission
const unsigned long period = 1000;      // The value is a number of milliseconds

const byte pctUL = 100;                 // Upper limit of pump speed (%)
const byte pctLL = 50;                  // Lower limit of pump speed (%)

/*--- Global Variables ---*/
unsigned long startTime;                // Start time
unsigned long currentTime;              // Current time

volatile byte pumpPct;                  // Percentage of pump PWM

/*--- Function Prototypes ---*/
void systemInit(void);
void systemPrompt(const unsigned long );
byte pumpOn(void);
byte pumpOff(void);
void setSpeed(int );
void setPump(const byte , byte );
void inputPrompt(int );
void flushReceive(void);
void getPct(int );
void serialEvent(void);
void setup(void);
void loop(void);

/*--- Initialization ---*/
void setup(void) {
  systemInit();

  Serial.println("Input percentage of PWM to the pump");
}

/*--- Operating ---*/
void loop(void) {
  setPump(pumpPin, pumpPct);

  /*--- Sensor prompt ---*/
  systemPrompt(period);
}

/*--- Functions Definition ---*/
void systemInit(void) {
  Wire.begin(PUMP_CTRL);    // Initialize Wire and join I2C bus
  Wire.onReceive(getPct);   // Register receive event
  Serial.begin(baudSpeed);  // Initializes serial port
  pinMode(pumpPin, OUTPUT); // Initializes pump pin
  startTime = millis();     // Initial start time
  pumpOff();                // Initializes Pump percentage

  return;
}

void systemPrompt(const unsigned long interval) {
  currentTime = millis();                       // Get the current "time"
  
  if ((currentTime - startTime) >= interval) {  // Test whether the period has elapsed
    Serial.print("Current pump speed: ");
    Serial.print(pumpPct);
    Serial.println(" %");

    startTime = currentTime;                    // Save the start time of the current state
  } else {
    return;
  }
}

byte pumpOn(void) {   // Full speed
  pumpPct = 100;

  return pumpPct;
}

byte pumpOff(void) {  // Stop the pump
  pumpPct = 0;

  return pumpPct;
}

void setSpeed(int percent) {
  percent = constrain(percent, 0, 100); // Limits the percentage of pump speed to between 0 - 100

  pumpPct = (byte)percent;

  if (pumpPct > pctUL) {
    pumpOn();
  } if (pumpPct < pctLL) {
    pumpOff();
  } else {
    return;
  }
}

void setPump(const byte pwmPin, byte pwmPct) { // Set the pump speed percentage
  analogWrite(pwmPin, (byte)(pwmPct * 2.55));

  return;
}

void inputPrompt(int percent) {
  if ((percent > pctUL) || (percent < pctLL)) {
    Serial.print(percent);
    Serial.println(", Input out of range! and return to normal speed.");
  } else { 
    Serial.print("Pump PWM Set to: ");
    Serial.print((byte)percent);
    Serial.println(" %");
  }

  return;
}

void flushReceive(void) { // Flush receive buffer
  while (Serial.available()) {
    Serial.read();
    Serial.flush();
  }

  while (Wire.available()) { // Wiring specific 
    Wire.read();
  }

  return;
}

void getPct(int percent) {  // Handler, Set the pump speed percentage through I2C
  if (Wire.available()) {
    percent = Wire.read();
    inputPrompt(percent);

    setSpeed(percent);
    flushReceive();
  } else {
    return;
  }
}

void serialEvent(void) {  // Set the pump speed percentage through serial input
  int percent;

  if (Serial.available()) {
    percent = Serial.parseInt();
    inputPrompt(percent);

    setSpeed(percent);
    flushReceive();
  } else {
    return;
  }
}

/*
  Set the pump speed percentage (PWM) through the serial port and I^2C from another MCU.
  Input range: 50 - 100 (%)

  - Kamoer KDS Peristaltic Pump (KDS-FE-2-S17B)
      12V DC brush motor

  Created 3 Nov. 2020
  by Yi-Xuan Wang

  References:
  http://kamoer.com/Products/showproduct.php?id=234
*/

/*--- Preprocessor ---*/
#include <Wire.h>   // Import Wire library for Inter-Integrated Circuit (I^2C)

#define MAIN_CTRL 1 // Address of main controller w/ sensor
#define PUMP_CTRL 2 // Address of pump controller

#define pumpPin 5   // The pin location of the pump (~D5) w/ PWM

/*--- Constants ---*/
const unsigned long baudSpeed = 115200; // Sets the data rate in bits per second (baud) for serial data transmission
const unsigned long period = 1000;      // The value is a number of milliseconds

const byte pctLL = 50;                  // Lower limit of pump speed (%)

/*--- Global Variables ---*/
unsigned long startTime;                // Start time
unsigned long currentTime;              // Current time

byte pumpPct;                           // Percentage of pump PWM

/*--- Function Prototypes ---*/
byte pumpOn(void);
byte pumpOff(void);
void setPump(const byte , byte );
void getPct(int );
void setup(void);
void loop(void);

/*--- Initialization ---*/
void setup(void) {
  Wire.begin(PUMP_CTRL);    // Initializes Wire and join I2C bus
  Wire.onReceive(getPct);   // Register receive event
  Serial.begin(baudSpeed);  // Initializes serial port
  pinMode(pumpPin, OUTPUT); // Initializes pump pin
  startTime = millis();     // Initial start time

  // Pump Percentage Initialization
  pumpPct = 0;

  Serial.println("Input a percentage of PWM to the pump");
}

/*--- Operating ---*/
void loop(void) {
  setPump(pumpPin, pumpPct);

  // Every second, print the percentage of pump speed
  currentTime = millis();                     // Get the current "time"

  /*--- Sensor prompt ---*/
  if ((currentTime - startTime) >= period) {  // Test whether the period has elapsed
    Serial.print("Current pump speed: ");
    Serial.print(pumpPct);
    Serial.println(" %");
    startTime = currentTime;                  // Save the start time of the current state
  } else {
    return;
  }
}

/*--- Functions Definition ---*/
byte pumpOn(void) {   // Full speed
  pumpPct = 100;

  return pumpPct;
}

byte pumpOff(void) {  // Stop the pump
  pumpPct = 0;

  return pumpPct;
}

void setPump(const byte pwmPin, byte pwmPct) { // Set the pump speed percentage
  analogWrite(pwmPin, (pwmPct * 2.55));

  return;
}

void serialEvent(void) {  // Set the pump speed percentage through serial input
  int pct;

  if (Serial.available()) {
    pct = Serial.parseInt();
    Serial.print("Pump PWM Set to: ");
    Serial.print(pct);
    Serial.println(" %");

    pct = constrain(pct, 0, 100); // Limits the percentage of pump speed to between 0 - 100
    if ((pct > pumpPct) && (pct != 0)) {
      pumpPct = pumpPct + pct;
    } if ((pct < pumpPct) && (pct >= pctLL)) {
      pumpPct = pumpPct + (pct - pumpPct);
    } else if ((pct == 0) || (pct < pctLL)) {
      pumpOff();
    }
    // Flush the receive buffer
    Serial.flush(); 
    while (Serial.read() >= 0) { }
  } else {
    return;
  }
}

void getPct(int percent) {  // Set the pump speed percentage through serial input
  if (Wire.available()) {
    percent = Wire.read();
    Serial.print("Pump PWM Set to: ");
    Serial.print(percent);
    Serial.println(" %");

    percent = constrain(percent, 0, 100); // Limits the percentage of pump speed to between 0 - 100
    if ((percent > pumpPct) && (percent != 0)) {
      pumpPct = pumpPct + percent;
    } if ((percent < pumpPct) && (percent >= pctLL)) {
      pumpPct = pumpPct + (percent - pumpPct);
    } else if ((percent == 0) || (percent < pctLL)) {
      pumpOff();
    }
  } else {
    return;
  }
}

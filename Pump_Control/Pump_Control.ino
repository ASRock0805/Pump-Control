/*
  Set the pump speed percentage (PWM) through the serial port.
  Input range: 50 - 100 (%)

  - Kamoer KDS Peristaltic Pump (KDS-FE-2-S17B)
      12V DC brush motor

  Created 20 Oct. 2020
  by Yi-Xuan Wang

  References:
  http://kamoer.com/Products/showproduct.php?id=234
*/

/*--- Preprocessor ---*/
#define pumpPin 5 // The pin location of the pump (~D5) w/ PWM

/*--- Constants ---*/
const unsigned long baudSpeed = 115200; // Sets the data rate in bits per second (baud) for serial data transmission
const unsigned long period = 1000;      // The value is a number of milliseconds

const byte pctLL = 50;                  // Lower limit of pump speed (%)

/*--- Global Variables ---*/
unsigned long startTime;                // Start time
unsigned long currentTime;              // Current time

byte pumpPct;                           // Percentage of pump PWM

/*--- Function Prototype ---*/
byte pumpOn(void);
byte pumpOff(void);
void setPump(byte , byte );
void serialEvent(void);
void setup(void);
void loop(void);

/*--- Functions Definition ---*/
byte pumpOn(void) {   // Full speed
  pumpPct = 100;

  return pumpPct;
}

byte pumpOff(void) {  // Stop the pump
  pumpPct = 0;

  return pumpPct;
}

void setPump(byte pwmPin, byte pwmPct) { // Set the pump speed percentage
  analogWrite(pwmPin, (pwmPct * 2.55));
}

void serialEvent(void) {  // Set the pump speed percentage through serial input
  int percent;

  if (Serial.available()) {
    percent = Serial.parseInt();
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
    // Flush the receive buffer
    Serial.flush(); 
    while (Serial.read() >= 0) { }
  } else {
    return;
  }
}

/*--- Initialization ---*/
void setup(void) {
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

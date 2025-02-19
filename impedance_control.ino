#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Servo.h>
#include <LiquidCrystal.h>

// ---------------------------
//  OBJECT INSTANTIATIONS
// ---------------------------
Adafruit_INA219 ina219;
Servo myServo;

// Create an LCD object. Adjust pins based on your wiring:
// lcd(RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// ---------------------------
//  PIN DEFINITIONS
// ---------------------------
const int buttonPin = 7; // The pin where your button is connected
1
2
// ---------------------------
//  IMPEDANCE PARAMETERS
// ---------------------------
const float K = 2000;      // Stiffness constant
const float C = 15;       // Damping constant

// ---------------------------
//  CONTROLLER VARIABLES
// ---------------------------
int angle = 0;              // Servo angle
float currentReading = 0;   // Current reading from INA219 (in A)
float force_raw = 0;        // Raw force based on measured current
float F = 0;                // Impedance force (C*X_dot + K*X)
float F_sat = 3.7;          // Force saturation limit
float X = 0;                // Displacement
float X_dot = 0;            // Velocity
float X_old = 0;
float X_dot_old = 0;
float X_new = 0;


// Timing for derivatives
unsigned long lastUpdateTime = 0;
unsigned long currentTime = 0;
float dt = 0;

// ---------------------------
//  SERVO LIMITS
// ---------------------------
const int maxServoAngle = 125; // Max angle for the servo
const int homeAngle = 8;       // Angle to return to when saturation is reached

// ---------------------------
//  SETUP
// ---------------------------
void setup() {
  // Initialize Serial
  Serial.begin(9600);
  while (!Serial) {
    delay(10);
  }

  // Initialize INA219
  if (!ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    #while (1) { delay(10); }
  }
  Serial.println("INA219 initialized");

  // Initialize servo
  myServo.attach(9);

  // Initialize the LCD (16 columns, 2 rows)
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("System Initializing");
  delay(2000);

  // Setup the button pin; using INPUT_PULLUP as an example
  pinMode(buttonPin, INPUT_PULLUP);
  // If your button is wired differently, you may need a different pinMode.
}

// ---------------------------
//  MAIN LOOP
// ---------------------------
void loop() {
  // Read current from INA219 in amperes (raw)
  currentReading = ina219.getCurrent_mA() / 1000.0; // Convert mA to A

  // Convert current to raw force
  force_raw = (41.871f * currentReading) - 0.414f;

  // --------------------------
  // PHASE 1: CLOSE GRIPPER until force >= 0.4
  // Only run if force_raw < 0.4 and button is HIGH
  // --------------------------
  while (force_raw < 0.4 && digitalRead(buttonPin) == HIGH) {

    angle += 5;
    if (angle > maxServoAngle) angle = maxServoAngle;

    // Calculate displacement based on angle
    X = (0.07459f * angle) * 0.001f;

    myServo.write(angle);
    Serial.print("Servo angle (phase 1): ");
    Serial.println(angle);

    // Display the raw force on the LCD (no impedance force yet)
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("F_cur: ");
    lcd.print(force_raw, 2);  // show raw
    lcd.setCursor(0, 1);
    lcd.print("F_imp: ");
    lcd.print(0, 2);          // Impedance not yet active

    delay(500); // Small delay for smooth motion

    // Update current reading
    currentReading = ina219.getCurrent_mA() / 1000.0;
    force_raw = (41.871f * currentReading) - 0.414f;

    // Reset timing for derivative calculation
    lastUpdateTime = millis();
    X_old = X;
    X_dot_old = 0;

    Serial.print("Current (A): ");
    Serial.println(currentReading);
    Serial.print("Raw Force: ");
    Serial.println(force_raw);

    // If the servo is at max angle but force < 0.4, break to avoid stalling
    if (angle >= maxServoAngle) {
      Serial.println("Max angle reached without reaching force 0.4. Breaking...");
      break;
    }
  }

  // --------------------------
  // PHASE 2: IMPEDANCE CONTROL once force >= 0.4
  // Only run if force_raw >= 0.4 and button is HIGH
  // --------------------------
  while (force_raw >= 0.4 && digitalRead(buttonPin) == HIGH) {
    // Calculate elapsed time (dt) since last iteration
    currentTime = millis();
    dt = (currentTime - lastUpdateTime) / 1000.0; // Convert ms to seconds
    lastUpdateTime = currentTime;

    // Update the angle
    angle += 5;
    if (angle > maxServoAngle) angle = maxServoAngle;
    myServo.write(angle);

    // Calculate displacement
    X_new = (0.07459f * angle) * 0.001f;
    X = X_new - X_old ;
    // Calculate velocity
    X_dot = X / dt;

    // Update old values
    X_old = X;
    X_dot_old = X_dot;

    // Impedance force
    F = C * X_dot + K * X;

    // Update current reading
    currentReading = ina219.getCurrent_mA() / 1000.0;
    force_raw = (41.871f * currentReading) - 0.414f;

    // Print debug info
    Serial.print("Current (A): ");
    Serial.println(currentReading);
    Serial.print("Raw Force (F_cur): ");
    Serial.println(force_raw);
    Serial.print("Impedance Force (F_imp): ");
    Serial.println(F);
    Serial.print("Displacement (X): ");
    Serial.println(X);
    Serial.print("Velocity (X_dot): ");
    Serial.println(X_dot);

    // Display on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("F_cur: ");
    lcd.print(force_raw, 2);
    lcd.setCursor(0, 1);
    lcd.print("F_imp: ");
    lcd.print(F, 2);

    // Check saturation conditions with raw force
    if (force_raw > F_sat || F > F_sat || angle == maxServoAngle) {
      Serial.println("Saturation reached! Waiting 10 seconds...");
      delay(10000); // Wait for 10 seconds

      // Smooth return to home angle
      Serial.println("Returning to home angle smoothly...");
      if (angle > homeAngle) {
        while (angle > homeAngle) {
          angle -= 5;
          if (angle < homeAngle) angle = homeAngle;
          myServo.write(angle);
          delay(200);
        }
      } else if (angle < homeAngle) {
        while (angle < homeAngle) {
          angle += 5;
          if (angle > homeAngle) angle = homeAngle;
          myServo.write(angle);
          delay(200);
        }
      }

      // Reset impedance variables
      F = 0;
      X = 0;
      X_dot = 0;
      X_old = 0;
      X_dot_old = 0;

      Serial.println("Servo is now at home angle; impedance force reset.");
      break; // Exit to restart cycle
    }

    // Small delay for demonstration
    delay(500);

    // Update once more for next iteration
    currentReading = ina219.getCurrent_mA() / 1000.0;
    force_raw = (41.871f * currentReading) - 0.414f;
  }

  // End of loop: If button goes LOW (or force changes), we exit the while loops
  // and return here, where the code can repeat or remain idle until the button is high again.
}

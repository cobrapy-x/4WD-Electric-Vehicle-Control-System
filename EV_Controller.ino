/*
  4WD Electric Vehicle Control System (Tinkercad)

  Arduino Uno Pin Connections
  ---------------------------
  D3  - Front-left motor PWM (L293D #2, pin 1)
  D5  - Front-right motor PWM (L293D #2, pin 9)
  D6  - Rear-left and rear-right motor PWM (L293D #1, pins 1 and 9)
  D7  - Shared motor direction input 1
  D8  - Shared motor direction input 2
  D9  - Steering servo signal

  A0  - Accelerator potentiometer
  A1  - Steering potentiometer

  5V  - Logic power for motor drivers and control components
  GND - Common ground

  External Motor Supply: 8.20V
  Arduino Logic Supply: 5V

  Motor Control:
  - Independent PWM connections for both front motors.
  - Shared PWM connection for both rear motors.
  - Shared forward direction control.
  - Open-loop speed control.

  Steering:
  - Servo angle ranges from 60 to 120 degrees.
  - 90 degrees represents straight ahead.

  Note:
  Motor RPM is not measured by this Arduino.
  RPM can be observed using the Tinkercad simulation interface.
*/

#include <Servo.h>

// ----- Pins -----

// Motor PWM outputs
const int FRONT_LEFT_MOTOR = 3;
const int FRONT_RIGHT_MOTOR = 5;
const int REAR_MOTORS = 6;

// Motor direction outputs
const int MOTOR_DIRECTION_1 = 7;
const int MOTOR_DIRECTION_2 = 8;

// Steering servo
const int STEERING_SERVO_PIN = 9;

// Analog control inputs
const int ACCELERATOR_PIN = A0;
const int STEERING_PIN = A1;


// ----- Circuit values -----

// Supply voltages used in the simulation
const float MOTOR_SUPPLY_VOLTAGE = 8.20;
const float LOGIC_SUPPLY_VOLTAGE = 5.00;

// Arduino ADC and PWM ranges
const int ADC_MIN = 0;
const int ADC_MAX = 1023;

const int PWM_MIN = 0;
const int PWM_MAX = 255;


// ----- Settings -----

// Steering limits
const int STEERING_MIN_ANGLE = 60;
const int STEERING_MAX_ANGLE = 120;
const int STEERING_CENTER_ANGLE = 90;

// Time between control updates
const unsigned long UPDATE_INTERVAL = 50;

// Steering servo object
Servo steeringServo;


// ----- Variables -----

int acceleratorReading = 0;
int steeringReading = 0;

int motorPWM = 0;
int steeringAngle = STEERING_CENTER_ANGLE;

unsigned long previousUpdate = 0;


// Reads the accelerator and converts its position into a PWM command.
int readAccelerator() {

  acceleratorReading = analogRead(ACCELERATOR_PIN);

  int pwm = map(
    acceleratorReading,
    ADC_MIN,
    ADC_MAX,
    PWM_MIN,
    PWM_MAX
  );

  return constrain(pwm, PWM_MIN, PWM_MAX);
}


// Reads the steering potentiometer and calculates the servo angle.
int readSteering() {

  steeringReading = analogRead(STEERING_PIN);

  int angle = map(
    steeringReading,
    ADC_MIN,
    ADC_MAX,
    STEERING_MIN_ANGLE,
    STEERING_MAX_ANGLE
  );

  return constrain(
    angle,
    STEERING_MIN_ANGLE,
    STEERING_MAX_ANGLE
  );
}


// Sends the same accelerator command to all four motors.
void controlMotors(int pwm) {

  // Front motors have separate PWM outputs.
  analogWrite(FRONT_LEFT_MOTOR, pwm);
  analogWrite(FRONT_RIGHT_MOTOR, pwm);

  // Both rear motors share one PWM output.
  analogWrite(REAR_MOTORS, pwm);
}


// Moves the steering servo to the requested angle.
void controlSteering(int angle) {

  steeringServo.write(angle);
}


// Prints the current control values to the Serial Monitor.
void displayTelemetry() {

  float dutyCycle = (motorPWM * 100.0) / PWM_MAX;

  Serial.println("------------------------------");

  Serial.println("4WD EV CONTROL SYSTEM");

  Serial.print("Accelerator ADC: ");
  Serial.println(acceleratorReading);

  Serial.print("Motor PWM: ");
  Serial.println(motorPWM);

  Serial.print("PWM Duty Cycle: ");
  Serial.print(dutyCycle, 2);
  Serial.println("%");

  Serial.print("Steering ADC: ");
  Serial.println(steeringReading);

  Serial.print("Steering Angle: ");
  Serial.print(steeringAngle);
  Serial.println(" degrees");

  Serial.println("RPM: Not measured by Arduino");

  Serial.println("------------------------------");
}


// ----- Setup -----

void setup() {

  // Configure motor PWM pins.
  pinMode(FRONT_LEFT_MOTOR, OUTPUT);
  pinMode(FRONT_RIGHT_MOTOR, OUTPUT);
  pinMode(REAR_MOTORS, OUTPUT);

  // Configure motor direction pins.
  pinMode(MOTOR_DIRECTION_1, OUTPUT);
  pinMode(MOTOR_DIRECTION_2, OUTPUT);

  // Configure analog input pins.
  pinMode(ACCELERATOR_PIN, INPUT);
  pinMode(STEERING_PIN, INPUT);

  // Set motor outputs to zero before enabling propulsion.
  controlMotors(0);

  // Configure the tested forward direction.
  digitalWrite(MOTOR_DIRECTION_1, HIGH);
  digitalWrite(MOTOR_DIRECTION_2, LOW);

  // Attach and centre the steering servo.
  steeringServo.attach(STEERING_SERVO_PIN);
  steeringServo.write(STEERING_CENTER_ANGLE);

  // Start serial communication.
  Serial.begin(9600);

  // Startup message.
  Serial.println("==============================");
  Serial.println("4WD ELECTRIC VEHICLE");
  Serial.println("CONTROL SYSTEM INITIALIZED");
  Serial.println("==============================");

  Serial.print("Motor Supply: ");
  Serial.print(MOTOR_SUPPLY_VOLTAGE, 2);
  Serial.println(" V (configured)");

  Serial.print("Logic Supply: ");
  Serial.print(LOGIC_SUPPLY_VOLTAGE, 2);
  Serial.println(" V (nominal)");

  Serial.println("Motor Control: OPEN LOOP");
  Serial.println("Steering: ENABLED");
  Serial.println("RPM Feedback: NOT CONNECTED");
  Serial.println();

  previousUpdate = millis();
}


// ----- Main Loop -----

void loop() {

  unsigned long currentTime = millis();

  // Run the control loop every 50 milliseconds.
  if (currentTime - previousUpdate >= UPDATE_INTERVAL) {

    previousUpdate = currentTime;

    // ----- 1) Measure -----

    // Read accelerator and steering potentiometers.
    motorPWM = readAccelerator();

    steeringAngle = readSteering();


    // ----- 2) Calculate -----

    // The accelerator PWM is applied equally to all motors.
    // No encoder feedback or speed correction is implemented.

    // The steering angle is mapped between 60 and 120 degrees.


    // ----- 3) Control -----

    // Update all four motor commands.
    controlMotors(motorPWM);

    // Update steering servo position.
    controlSteering(steeringAngle);


    // ----- 4) Serial Monitor -----

    // Display accelerator, PWM and steering information.
    displayTelemetry();
  }
}

#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>

void IR_Calibration();
void setMotorPower(int, int, int);
void readEncoders();
void setRightMotorSpeed(int);
void setLeftMotorSpeed(int);
void readIR();
int PD_IR();
void getSquaresTraveled();

// Built-in led
const int led = 13;

// Motor PWM pins
const int m1Forward = 4;
const int m1Reverse = 3;
const int m2Forward = 5;
const int m2Reverse = 6;

// Encoder digital pins
const int encoder_m1_A = 9;
const int encoder_m1_B = 10;
const int encoder_m2_A = 12;
const int encoder_m2_B = 11;

// Encoder objects
Encoder encoder_m1(encoder_m1_A, encoder_m1_B);
Encoder encoder_m2(encoder_m2_A, encoder_m2_B);

// Emitter digital pins
const int emitters[4]={23,22,21,20};
const int emit1 = 23;
const int emit2 = 22;
const int emit3 = 21;
const int emit4 = 20;

// frontRightRecieverer analog pins
const int receivers[4]={19,18,17,16};
const int frontLeftReciever = 18;
const int leftReciever = 19;
const int frontRightReciever = 17;
const int rightReciever = 16;

// IR Reciever variables
int r1;
int r2;
int r3;
int r4;

// PD IR variables
int errorP;
int errorD;
int oldErrorP;
int leftMiddleValue;
int rightMiddleValue;
int leftWallIR;
int rightWallIR;
int rightLeftOffset;
float totalError;
float P = 1.0;
float D = 0.5;

// Encoder variables
long enc1;
long enc2;
long enc1_old;
long enc2_old;
int enc1_ticksPerCycle;
int enc2_ticksPerCycle;

// Motor variables
int m1Speed = 30;
int m2Speed = 30;

// Travel Variables
const double distancePerTick = 0.244346095279;
int squares = 0;

void setup() 
{
  // Built-in led
  pinMode(led, OUTPUT);
  
  // Motor pin setup
  pinMode(m1Forward, OUTPUT);
  pinMode(m1Reverse, OUTPUT);
  pinMode(m2Forward, OUTPUT);
  pinMode(m2Reverse, OUTPUT);

  // Emitter pin setup
  pinMode(emit1, OUTPUT);
  pinMode(emit2, OUTPUT);
  pinMode(emit3, OUTPUT);
  pinMode(emit4, OUTPUT);

  // frontRightRecieverer pin setup
  pinMode(frontLeftReciever, INPUT);
  pinMode(leftReciever, INPUT);
  pinMode(frontRightReciever, INPUT); 
  pinMode(rightReciever, INPUT);
  
  // Turn on Teensy LED
  digitalWrite(led, HIGH);

  // Serial Monitor
  Serial.begin(9600);
  Serial.println("Micromouse: Team Roadent");

  // Run calibration
  delay(5000);
  IR_Calibration();
}

void loop()
{  
  readIR();

  readEncoders();

  setRightMotorSpeed(10);
  setLeftMotorSpeed(-10);

  PD_IR();

  getSquaresTraveled();

  /**
  while(squares >= 1)
  {
    setMotorPower(m1Forward, m1Reverse, 0);
    setMotorPower(m2Forward, m2Reverse, 0);
  }
  **/

  Serial.print("Right Reciever: ");
  Serial.println(r3);
  Serial.print("Front Right Reciever: ");
  Serial.println(r1);
  Serial.print("Front Left Reciever: ");
  Serial.println(r2);
  Serial.print("Left Reciever: ");
  Serial.println(r4);
  Serial.println();
  Serial.print("Right Encoder: ");
  Serial.println(enc2);
  Serial.print("Left Encoder: ");
  Serial.println(enc1);
  Serial.print("Right Encoder Ticks per Cycle: ");
  Serial.println(enc2_ticksPerCycle);
  Serial.print("Left Encoder Ticks per Cycle: ");
  Serial.println(enc1_ticksPerCycle);
  Serial.println();
  Serial.print("Right Motor Speed: ");
  Serial.println(m2Speed);
  Serial.print("Left Motor Speed: ");
  Serial.println(m1Speed);
  Serial.println();
  Serial.print("rightLeftOffset: ");
  Serial.println(rightLeftOffset);
  Serial.print("errorP: ");
  Serial.println(errorP);
  Serial.print("errorD: ");
  Serial.println(errorD);
  Serial.print("PD Error: ");
  Serial.println(totalError);
  Serial.println();
  Serial.print("Squares Traveled: ");
  Serial.println(squares);
  Serial.println();
  Serial.println();
  
  delay(10);

}

void IR_Calibration()
{
  // Turn emitters on
  digitalWrite(emit1, HIGH);
  digitalWrite(emit2, HIGH);
  digitalWrite(emit3, HIGH);
  digitalWrite(emit4, HIGH);
  
  r1 = analogRead(frontRightReciever);
  r2 = analogRead(frontLeftReciever);

  // Turn emmitters off
  digitalWrite(emit1, LOW);
  digitalWrite(emit2, LOW);
  digitalWrite(emit3, LOW);
  digitalWrite(emit4, LOW);
  
  leftMiddleValue = r2;
  rightMiddleValue = r1;
  leftWallIR = r2 - 5;
  rightWallIR = r1 - 5;
  rightLeftOffset = r1 - r2;
}

void setMotorPower(int mForward, int mReverse, int pwr)
{
  if (pwr >= 0)
  {
    analogWrite(mForward, pwr);
    digitalWrite(mReverse, LOW);
  }

  if (pwr < 0)
  {
    analogWrite(mReverse, -pwr);
    digitalWrite(mForward, LOW);
  }
}

void readEncoders()
{
  // Get Encoder Values
  enc1 = encoder_m1.read();
  enc2 = encoder_m2.read();

  // Calculate ticksPerCycle
  enc1_ticksPerCycle = (enc1 - enc1_old);
  enc2_ticksPerCycle = (enc2 - enc2_old);

  enc1_old = enc1;
  enc2_old = enc2;
}

void setRightMotorSpeed(int targetSpeed)
{  
  setMotorPower(m2Forward, m2Reverse, m2Speed);

  // Proportional Adjustment  
  if (enc2_ticksPerCycle < (targetSpeed - 1))
  {
    setMotorPower(m2Forward, m2Reverse, m2Speed++);
  }
  if (enc2_ticksPerCycle > (targetSpeed + 1))
  {
    setMotorPower(m2Forward, m2Reverse, m2Speed--);
  }
}

void setLeftMotorSpeed(int targetSpeed)
{
  setMotorPower(m1Forward, m1Reverse, m1Speed);

  // Proportional Adjustment
  if (enc1_ticksPerCycle < (targetSpeed - 1))
  {
    setMotorPower(m1Forward, m1Reverse, m1Speed++);
  }
  if (enc1_ticksPerCycle > (targetSpeed + 1))
  {
    setMotorPower(m1Forward, m1Reverse, m1Speed--);
  }
}

void readIR()
{
  // Turn emitters on
  digitalWrite(emit1, HIGH);
  digitalWrite(emit2, HIGH);
  digitalWrite(emit3, HIGH);
  digitalWrite(emit4, HIGH);

  // Get emitter values
  r1 = analogRead(frontRightReciever);
  r2 = analogRead(frontLeftReciever);
  r3 = analogRead(rightReciever);
  r4 = analogRead(leftReciever);

  // Turn emmitters off
  digitalWrite(emit1, LOW);
  digitalWrite(emit2, LOW);
  digitalWrite(emit3, LOW);
  digitalWrite(emit4, LOW);
}

// IR PD for going straight
int PD_IR()
{  
  // If there are both walls
  if ((r2 > leftWallIR) && (r1 > rightWallIR))
  {
    errorP = r1 - r2 - rightLeftOffset;
    errorD = errorP - oldErrorP;
  }

  // Only left wall
  else if (r2 > leftWallIR)
  {
    errorP = 2 * (leftMiddleValue - r2);
    errorD = errorP - oldErrorP;
  }

  // Only right wall
  else if (r1 > rightWallIR)
  {
    errorP = 2 * (r1 - rightMiddleValue);
    errorD = errorP - oldErrorP;
  }

  // If no walls
  else if ((r2 < leftWallIR) && (r1 < rightWallIR))
  {
    errorP = 0;
    errorD = 0;
  }

  totalError = (P * (float)errorP) + (D * (float)errorD);
  oldErrorP = errorP;
  
  return totalError;
}

void getSquaresTraveled()
{
  float encAverage = ((float)(enc1 + enc2)) / 2.0;
  squares = (int)((encAverage * distancePerTick) / 180.0);
}


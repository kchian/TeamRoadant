#include <StackArray.h>
#include <QueueList.h>

#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>

void IRCalibration();
void setMotorPower(int, int, int);
void readEncoders();
void motorPD();
void readIR();
void IRPD();
void isFrontWall();
void turnRight();
void turnLeft();
void reverse();

// Built-in led
const int led = 13;

// Motor PWM pins
const int m1Forward = 4;
const int m1Reverse = 3;
const int m2Forward = 5;
const int m2Reverse = 6;

// Encoder digital pins
const int encoderM1A = 9;
const int encoderM1B = 10;
const int encoderM2A = 12;
const int encoderM2B = 11;

// Encoder objects
Encoder encoderM1(encoderM1A, encoderM1B);
Encoder encoderM2(encoderM2A, encoderM2B);

// Emitter digital pins
const int emit1 = 23;
const int emit2 = 22;
const int emit3 = 21;
const int emit4 = 20;

// frontRightRecieverer analog pins
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
int errorPIR;
int errorDIR;
int olderrorPIR;
int leftMiddleValue;
int rightMiddleValue;
int leftWallValue;
int rightWallValue;
int rightLeftOffset;
float totalErrorIR;
const float PIR = 0.2; // Tuned
const float DIR = 0.5; // Tuned

// PD Motor variables
int errorPM1;
int errorPM2;
int errorDM1;
int errorDM2;
int olderrorPM1;
int olderrorPM2;
float totalErrorM1;
float totalErrorM2;
const float PMotor = 0.6; // Tuned
const float DMotor = 0.8; // Tuned

// Encoder variables
long enc1;
long enc2;
long enc1Old;
long enc2Old;
int enc1Speed;
int enc2Speed;

// Motor variables
int m1Power = 30;
int m2Power = 30;

// Travel Variables
const double distancePerTick = 0.244346095279;
int squares = 0;

// Walls
int frontWallValue; // Needs to be set during calibration somehow --> Ask Kevin
int frontOffset;
int frontEncAvg;

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

  delay(5000);
  
  // Serial Monitor  
  Serial.begin(9600);
  Serial.println("Micromouse: Team RoadAnt");
  

  // Run calibration
  delay(5000);
  IRCalibration();
}

void loop()
{
  readIR();

  IRPD();
  motorPD(30, 30);

  setMotorPower(m1Forward, m1Reverse, m1Power);
  setMotorPower(m2Forward, m2Reverse, m2Power);
  

  isFrontWall();
  
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
  Serial.println(enc2Speed);
  Serial.print("Left Encoder Ticks per Cycle: ");
  Serial.println(enc1Speed);
  Serial.println();
  Serial.print("Right Motor Power: ");
  Serial.println(m2Power);
  Serial.print("Left Motor Power: ");
  Serial.println(m1Power);
  Serial.println();
  Serial.print("rightLeftOffset: ");
  Serial.println(rightLeftOffset);
  Serial.print("errorPIR: ");
  Serial.println(errorPIR);
  Serial.print("errorDIR: ");
  Serial.println(errorDIR);
  Serial.print("PD Error_IR: ");
  Serial.println(totalErrorIR);
  Serial.print("errorPM1: ");
  Serial.println(errorPM1);
  Serial.print("errorPM2: ");
  Serial.println(errorPM2);
  Serial.print("errorDM1: ");
  Serial.println(errorDM1);
  Serial.print("errorDM2: ");
  Serial.println(errorDM2);
  Serial.print("PD Error_m1: ");
  Serial.println(totalErrorM1);
  Serial.print("PD Error_m2: ");
  Serial.println(totalErrorM2);
  Serial.println();
  Serial.println();
  
  delay(50);
}

void IRCalibration()
{
  readIR();

  // Calibrate the front IR
  frontOffset = r3 - r4;
  frontWallValue = ((r3 + r4) / 2) - 5;

  // Calibrate the right IR
  rightMiddleValue = r1;
  rightWallValue = r1 - 5;

  delay(1000);
  
  turnLeft();

  delay(1000);

  reverse(200);
  
  delay(1000);

  readIR();
  
  // Calibrate the left IR
  leftMiddleValue = r2;
  leftWallValue = r2 - 5;
  
  rightLeftOffset = rightMiddleValue - leftMiddleValue;

  delay(1000);
}

void setMotorPower(int mForward, int mReverse, int pwr)
{
  if (pwr >= 0)
  {
    analogWrite(mForward, pwr);
    analogWrite(mReverse, 0);

  }

  if (pwr < 0)
  {
    analogWrite(mReverse, -pwr);
    analogWrite(mForward, 0);
  }
}

void readEncoders()
{
  // Get Encoder Values
  enc1 = encoderM1.read();
  enc2 = encoderM2.read();

  // Calculate ticksPerCycle
  enc1Speed = (enc1 - enc1Old);
  enc2Speed = (enc2 - enc2Old);

  enc1Old = enc1;
  enc2Old = enc2;
}

void motorPD(int targetM1, int targetM2)
{
  readEncoders();
  
  // Encoder 1
  errorPM1 = targetM1 - enc1Speed;
  errorDM1 = errorPM1 - olderrorPM1;

  // Encoder 2
  errorPM2 = targetM2 - enc2Speed;
  errorDM2 = errorPM2 - olderrorPM2;

  // Adding the proportional and derivative errors
  totalErrorM1= (PMotor * (float)errorPM1) + (DMotor * (float)errorDM1);
  totalErrorM2= (PMotor * (float)errorPM2) + (DMotor * (float)errorDM2);

  //Adjust motors
  m1Power += totalErrorM1;
  m2Power += totalErrorM2;

  // Prevents speed from getting too high
  if (m1Power > 75)
  {
    m1Power = 75;
  }
  if (m1Power < -75)
  {
    m1Power = -75;
  }
  if (m2Power > 75)
  {
    m2Power = 75;
  }
  if (m2Power < -75)
  {
    m2Power = -75;
  }
  
  olderrorPM1 = errorPM1;
  olderrorPM2 = errorPM2;  
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
void IRPD()
{  
  // If there are both walls
  if ((r2 > leftWallValue) && (r1 > rightWallValue))
  {
    errorPIR = r1 - r2 - rightLeftOffset;
    errorDIR = errorPIR - olderrorPIR;
  }

  // Only left wall
  else if (r2 > leftWallValue)
  {
    errorPIR = 2 * (leftMiddleValue - r2);
    errorDIR = errorPIR - olderrorPIR;
  }

  // Only right wall
  else if (r1 > rightWallValue)
  {
    errorPIR = 2 * (r1 - rightMiddleValue);
    errorDIR = errorPIR - olderrorPIR;
  }

  // If no walls
  else if ((r2 < leftWallValue) && (r1 < rightWallValue))
  {
    errorPIR = 0;
    errorDIR = 0;
  }

  // If getting close to a front wall
  if (frontEncAvg > frontWallValue)
  {
    errorPIR = 0;
    errorDIR = 0;
  }

  totalErrorIR = (PIR * (float)errorPIR) + (DIR * (float)errorDIR);

  m1Power -= totalErrorIR;
  m2Power += totalErrorIR;

  // Prevents speed from getting too high
  if (m1Power > 75)
  {
    m1Power = 75;
  }
  if (m1Power < -75)
  {
    m1Power = -75;
  }
  if (m2Power > 75)
  {
    m2Power = 75;
  }
  if (m2Power < -75)
  {
    m2Power = -75;
  }
  
  setMotorPower(m1Forward, m1Reverse, m1Power);
  setMotorPower(m2Forward, m2Reverse, m2Power);
  
  olderrorPIR = errorPIR;
  
}

void isFrontWall()
{
  frontEncAvg = (r3 + r4 + frontOffset) / 2;
  if (frontEncAvg > frontWallValue)
  {
    setMotorPower(m1Forward, m1Reverse, 0);
    setMotorPower(m2Forward, m2Reverse, 0);
  }
}

void turnRight()
{
  enc1Old = encoderM1.read();

  while(encoderM1.read() <= (enc1Old + 500))
  {
    motorPD(30, 0);
    setMotorPower(m1Forward, m1Reverse, m1Power);
    setMotorPower(m2Forward, m2Reverse, m2Power);
    delay(50);
  }

  setMotorPower(m1Forward, m1Reverse, 0);
  setMotorPower(m2Forward, m2Reverse, 0);

}

void turnLeft()
{
  short enc_old = encoderM2.read();

  while(encoderM2.read() <= (enc_old + 550))
  {
    motorPD(0, 30);
    setMotorPower(m1Forward, m1Reverse, m1Power);
    setMotorPower(m2Forward, m2Reverse, m2Power);
    delay(50);
  }

  setMotorPower(m1Forward, m1Reverse, 0);
  setMotorPower(m2Forward, m2Reverse, 0);

}

void reverse(short ticks)
{
  short enc_old = encoderM1.read();

  while(encoderM1.read() >= (enc_old - ticks))
  {
    motorPD(-30, -30);
    setMotorPower(m1Forward, m1Reverse, m1Power);
    setMotorPower(m2Forward, m2Reverse, m2Power);
    delay(50);
  }
  
  setMotorPower(m1Forward, m1Reverse, 0);
  setMotorPower(m2Forward, m2Reverse, 0);

}

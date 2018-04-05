#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>

// Built-in led
#define LED 13

// Motor PWM pins
#define M1FORWARD 4
#define M1REVERSE 3
#define M2FORWARD 5
#define M2REVERSE 6

// Encoder digital pins
#define ENCODER_M1_A 9
#define ENCODER_M1_B 10
#define ENCODER_M2_A 12
#define ENCODER_M2_B 11

// Emitter digital pins
#define EMIT1 23
#define EMIT2 22
#define EMIT3 21
#define EMIT4 20

// Reciever analog pins
#define FRONT_LEFT_RECIEVER 18
#define LEFT_RECIEVER 19
#define FRONT_RIGHT_RECIEVER 17
#define RIGHT_RECIEVER 16

void IR_Calibration();
void setMotorPower(int, int, int);
void readEncoders();
void PD_Motor();
void readIR();
void PD_IR();
void isFrontWall();
void isLeftWall();
void isRightWall();
void rightTurn();
void leftTurn();
void getSquaresTraveled();

// Encoder objects
Encoder encoder_m1(ENCODER_M1_A, ENCODER_M1_B);
Encoder encoder_m2(ENCODER_M2_A, ENCODER_M2_B);

// IR Reciever variables
int r1;
int r2;
int r3;
int r4;

// PD IR variables
int errorP_IR;
int errorD_IR;
int oldErrorP_IR;
int leftMiddleValue;
int rightMiddleValue;
int leftWallIR;
int rightWallIR;
int rightLeftOffset;
float totalError_IR;
const float P_IR = 0.2; // Tuned
const float D_IR = 0.5; // Tuned

// PD Motor variables
int errorP_m1;
int errorP_m2;
int errorD_m1;
int errorD_m2;
int oldErrorP_m1;
int oldErrorP_m2;
float totalError_m1;
float totalError_m2;
const float P_motor = 0.6; // Tuned
const float D_motor = 0.6; // Tuned

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

// Walls
int frontWallValue = 150; // Needs to be set during calibration somehow --> Ask Kevin
int frontOffset;
int frontEncAvg;

void setup() 
{
  // Built-in led
  pinMode(LED, OUTPUT);
  
  // Motor pin setup
  pinMode(M1FORWARD, OUTPUT);
  pinMode(M1REVERSE, OUTPUT);
  pinMode(M2FORWARD, OUTPUT);
  pinMode(M2REVERSE, OUTPUT);

  // Emitter pin setup
  pinMode(EMIT1, OUTPUT);
  pinMode(EMIT2, OUTPUT);
  pinMode(EMIT3, OUTPUT);
  pinMode(EMIT4, OUTPUT);

  // Reciever pin setup
  pinMode(FRONT_LEFT_RECIEVER, INPUT);
  pinMode(LEFT_RECIEVER, INPUT);
  pinMode(FRONT_RIGHT_RECIEVER, INPUT); 
  pinMode(RIGHT_RECIEVER, INPUT);
  
  // Turn on Teensy LED
  digitalWrite(LED, HIGH);

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

  PD_IR();
  PD_Motor(50, 50);

  isFrontWall();

  getSquaresTraveled();
  
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
  Serial.print("errorP_IR: ");
  Serial.println(errorP_IR);
  Serial.print("errorD_IR: ");
  Serial.println(errorD_IR);
  Serial.print("PD Error_IR: ");
  Serial.println(totalError_IR);
  Serial.print("errorP_m1: ");
  Serial.println(errorP_m1);
  Serial.print("errorP_m2: ");
  Serial.println(errorP_m2);
  Serial.print("errorD_m1: ");
  Serial.println(errorD_m1);
  Serial.print("errorD_m2: ");
  Serial.println(errorD_m2);
  Serial.print("PD Error_m1: ");
  Serial.println(totalError_m1);
  Serial.print("PD Error_m2: ");
  Serial.println(totalError_m2);
  Serial.println();
  Serial.println();
  
  delay(50);

}

void IR_Calibration()
{
  // Turn emitters on
  digitalWrite(EMIT1, HIGH);
  digitalWrite(EMIT2, HIGH);
  digitalWrite(EMIT3, HIGH);
  digitalWrite(EMIT4, HIGH);
  
  r1 = analogRead(FRONT_RIGHT_RECIEVER);
  r2 = analogRead(FRONT_LEFT_RECIEVER);
  r3 = analogRead(RIGHT_RECIEVER);
  r4 = analogRead(LEFT_RECIEVER);

  // Turn emitters off
  digitalWrite(EMIT1, LOW);
  digitalWrite(EMIT2, LOW);
  digitalWrite(EMIT3, LOW);
  digitalWrite(EMIT4, LOW);
  
  leftMiddleValue = r2;
  rightMiddleValue = r1;
  leftWallIR = r2 - 5;
  rightWallIR = r1 - 5;
  rightLeftOffset = r1 - r2;
  frontOffset = r3 - r4;
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

void PD_Motor(int targetM1, int targetM2)
{
  // Encoder 1
  errorP_m1 = targetM1 - enc1_ticksPerCycle;
  errorD_m1 = errorP_m1 - oldErrorP_m1;

  // Encoder 2
  errorP_m2 = targetM2 - enc2_ticksPerCycle;
  errorD_m2 = errorP_m2 - oldErrorP_m2;

  // Adding the proportional and derivative errors
  totalError_m1= (P_motor * (float)errorP_m1) + (D_motor * (float)errorD_m1);
  totalError_m2= (P_motor * (float)errorP_m2) + (D_motor * (float)errorD_m2);

  //Adjust motors
  m1Speed += totalError_m1;
  m2Speed += totalError_m2;

  // Prevents speed from getting too high
  if (m1Speed > 75)
  {
    m1Speed = 75;
  }
  if (m1Speed < -75)
  {
    m1Speed = -75;
  }
  if (m2Speed > 75)
  {
    m2Speed = 75;
  }
  if (m2Speed < -75)
  {
    m2Speed = -75;
  }
  
  setMotorPower(M1FORWARD, M1REVERSE, m1Speed);
  setMotorPower(M2FORWARD, M2REVERSE, m2Speed);
  
  oldErrorP_m1 = errorP_m1;
  oldErrorP_m2 = errorP_m2;  
}

void readIR()
{
  // Turn emitters on
  digitalWrite(EMIT1, HIGH);
  digitalWrite(EMIT2, HIGH);
  digitalWrite(EMIT3, HIGH);
  digitalWrite(EMIT4, HIGH);

  // Get emitter values
  r1 = analogRead(FRONT_RIGHT_RECIEVER);
  r2 = analogRead(FRONT_LEFT_RECIEVER);
  r3 = analogRead(RIGHT_RECIEVER);
  r4 = analogRead(LEFT_RECIEVER);

  // Turn emmitters off
  digitalWrite(EMIT1, LOW);
  digitalWrite(EMIT2, LOW);
  digitalWrite(EMIT3, LOW);
  digitalWrite(EMIT4, LOW);
}

// IR PD for going straight
void PD_IR()
{  
  // If there are both walls
  if ((r2 > leftWallIR) && (r1 > rightWallIR))
  {
    errorP_IR = r1 - r2 - rightLeftOffset;
    errorD_IR = errorP_IR - oldErrorP_IR;
  }

  // Only left wall
  else if (r2 > leftWallIR)
  {
    errorP_IR = 2 * (leftMiddleValue - r2);
    errorD_IR = errorP_IR - oldErrorP_IR;
  }

  // Only right wall
  else if (r1 > rightWallIR)
  {
    errorP_IR = 2 * (r1 - rightMiddleValue);
    errorD_IR = errorP_IR - oldErrorP_IR;
  }

  // If no walls
  else if ((r2 < leftWallIR) && (r1 < rightWallIR))
  {
    errorP_IR = 0;
    errorD_IR = 0;
  }

  // If getting close to a front wall
  if (frontEncAvg > frontWallValue)
  {
    errorP_IR = 0;
    errorD_IR = 0;
  }

  totalError_IR = (P_IR * (float)errorP_IR) + (D_IR * (float)errorD_IR);

  m1Speed -= totalError_IR;
  m2Speed += totalError_IR;

  // Prevents speed from getting too high
  if (m1Speed > 75)
  {
    m1Speed = 75;
  }
  if (m1Speed < -75)
  {
    m1Speed = -75;
  }
  if (m2Speed > 75)
  {
    m2Speed = 75;
  }
  if (m2Speed < -75)
  {
    m2Speed = -75;
  }
  
  setMotorPower(M1FORWARD, M1REVERSE, m1Speed);
  setMotorPower(M2FORWARD, M2REVERSE, m2Speed);
  
  oldErrorP_IR = errorP_IR;
  
}

void isFrontWall()
{
  frontEncAvg = (r3 + r4 + frontOffset) / 2;
  if (frontEncAvg > frontWallValue)
  {
    setMotorPower(M1FORWARD, M1REVERSE, 0);
    setMotorPower(M2FORWARD, M2REVERSE, 0);
  }
}

void isLeftWall()
{
  
}

void isRightWall()
{
  
}

void rightTurn()
{
  enc1_old = encoder_m1.read();

  while(encoder_m1.read() <= (enc1_old + 500))
  {
    PD_Motor(30, 0);
  }

  setMotorPower(M1FORWARD, M1REVERSE, 0);
  setMotorPower(M2FORWARD, M2REVERSE, 0);

  while(true);  // Remove this later
}

void leftTurn()
{
  enc2_old = encoder_m2.read();

  while(encoder_m2.read() <= (enc2_old + 550))
  {
    PD_Motor(0, 30);
  }

  setMotorPower(M1FORWARD, M1REVERSE, 0);
  setMotorPower(M2FORWARD, M2REVERSE, 0);

  while(true);  // Remove this later
}

void getSquaresTraveled()
{
  float encAverage = ((float)(enc1 + enc2)) / 2.0;
  squares = (int)((encAverage * distancePerTick) / 180.0);
}


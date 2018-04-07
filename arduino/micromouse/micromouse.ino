#include <StackArray.h>
#include <QueueList.h>

#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>

#define RIGHTANGLE 370
#define FORWARD 500//737

// Flood fill vars
#define CELL_CHECKED 16
#define NORTH 0x1
#define EAST 0x2
#define SOUTH 0x4
#define WEST 0x8

#define SHORT_MAX 32767

void map();

short row;
short col;
short edge;
//short dir;  Use with pathing later

short memory[16][16];
short nWeights[16][16];  // init this
short wWeights[16][16];  // init this
bool goal_found = false;

struct location
{
  short row;
  short col;
  short edge;
  short dist;
};
struct node
{
  short row;
  short col;
};
struct curPrev
{
  short row1;
  short col1;
  short row2;
  short col2;
};

typedef struct location Location;
typedef struct node Node;
typedef struct curPrev CurPrev;

void IRCalibration();
void setMotorPower(short, short, short);
void readEncoders();
void motorPD();
void encoderPD();
void readIR();
void IRPD();
bool isFrontWall();
void turnRight(short, short, short);
void turnLeft(short, short, short);
void forward(short, short, short);
void reverse(short, short, short);
short getWalls();
void dirToTurn(short);
void threadPD();

// Built-in led
const short led = 13;

// Motor PWM pins
const short m1Forward = 4;
const short m1Reverse = 3;
const short m2Forward = 5;
const short m2Reverse = 6;

// Encoder digital pins
const short encoderM1A = 9;
const short encoderM1B = 10;
const short encoderM2A = 12;
const short encoderM2B = 11;

// Encoder objects
Encoder encoderM1(encoderM1A, encoderM1B);
Encoder encoderM2(encoderM2A, encoderM2B);

// Emitter digital pins
const short emit1 = 23;
const short emit2 = 22;
const short emit3 = 21;
const short emit4 = 20;

// frontRightRecieverer analog pins
const short frontLeftReciever = 18;
const short leftReciever = 19;
const short frontRightReciever = 17;
const short rightReciever = 16;

// IR Reciever variables
short r1;
short r2;
short r3;
short r4;

// PD IR variables
short errorPIR;
short errorDIR;
short olderrorPIR;
short leftMiddleValue;
short rightMiddleValue;
short leftWallValue;
short rightWallValue;
short rightLeftOffset;
float totalErrorIR;
const float PIR = 0.1; // Tuned
const float DIR = 0.5; // Tuned

// PD Motor variables
short errorPM1;
short errorPM2;
short errorDM1;
short errorDM2;
short olderrorPM1;
short olderrorPM2;
float totalErrorM1;
float totalErrorM2;
const float PMotor = 0.1; // Tuned
const float DMotor = 0.2; // Tuned

// PD Encoder variables
short errorEncoderP;
short oldErrorEncoderP;
short errorEncoderD;
float totalErrorEncoder;
const float PEncoder = 0.0;
const float DEncoder = 0.0;

// Encoder variables
long enc1;
long enc2;
long enc1Old;
long enc2Old;
short enc1Speed;
short enc2Speed;

// Motor variables
short m1Power = 30;
short m2Power = 30;

// Travel Variables
const double distancePerTick = 0.244346095279;
short squares = 0;

// Walls
short frontWallValue;
short frontOffset;
short frontEncAvg;

// Orientation
short dir = EAST;

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

  //map();
}

void loop()
{
  readIR();

  IRPD();

  motorPD(60, 60);

  if (isFrontWall())
  {
    setMotorPower(m1Forward, m1Reverse, 0);
    setMotorPower(m2Forward, m2Reverse, 0);
    //turnRight(RIGHTANGLE, 20, -20);
    //delay(1000);
  }
//  
//  if (isFrontWall())
//  {
//    setMotorPower(m1Forward, m1Reverse, 0);
//    setMotorPower(m2Forward, m2Reverse, 0);
//    
//    if (dir == NORTH)
//    {
//      if (((~ getWalls()) & 0b0010) == 0b0010)
//      {
//        turnRight(RIGHTANGLE);
//        setMotorPower(m1Forward, m1Reverse, 0);
//        setMotorPower(m2Forward, m2Reverse, 0);
//      }
//    }
//    if (dir == EAST)
//    {
//      if (((~ getWalls()) & 0b0100) == 0b0100)
//      {
//        turnRight(RIGHTANGLE);
//        setMotorPower(m1Forward, m1Reverse, 0);
//        setMotorPower(m2Forward, m2Reverse, 0);
//      }
//    }
//    if (dir == WEST)
//    {
//      if (((~ getWalls()) & 0b1000) == 0b1000)
//      {
//        turnRight(RIGHTANGLE);
//        setMotorPower(m1Forward, m1Reverse, 0);
//        setMotorPower(m2Forward, m2Reverse, 0);
//      }
//    }
//    if (dir == SOUTH)
//    {
//      if (((~getWalls()) & 0b0001) == 0b0001)
//      {
//        turnRight(RIGHTANGLE);
//      }
//    }
//  }
//
  Serial.print("Walls: ");
  Serial.println(getWalls());
  
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
//  
    delay(50);
}

void IRCalibration()
{

  Serial.println("Running Calibration");
  
  readIR();

  // Calibrate the front IR
  frontOffset = r3 - r4;
  frontWallValue = ((r3 + r4) / 2);

  delay(1000);
  
  turnLeft(600, 0, 60);

  delay(1000);

  m1Power = -30;
  m2Power = -30;
  reverse(200, -60);

  m1Power = 0;
  m2Power = 0;
  
  delay(1000);

  setMotorPower(m1Forward, m1Reverse, 0);
  setMotorPower(m2Forward, m2Reverse, 0);

  readIR();

  // Calibrate the right IR
  rightMiddleValue = r1;
  rightWallValue = r1;
  
  // Calibrate the left IR
  leftMiddleValue = r2;
  leftWallValue = r2;
  
  rightLeftOffset = rightMiddleValue - leftMiddleValue;

  delay(1000);

  Serial.println("Calibration complete");
}

void setMotorPower(short mForward, short mReverse, short pwr)
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

void motorPD(short targetM1, short targetM2)
{
  readEncoders();
  
  // Encoder 1
  errorPM1 = targetM1 - enc1Speed;
  errorDM1 = errorPM1 - olderrorPM1;

  // Encoder 2
  errorPM2 = targetM2 - enc2Speed;
  errorDM2 = errorPM2 - olderrorPM2;

  // Adding the proportional and derivative errors
  totalErrorM1 = (PMotor * (float)errorPM1) + (DMotor * (float)errorDM1);
  totalErrorM2 = (PMotor * (float)errorPM2) + (DMotor * (float)errorDM2);

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

void encoderPD(short enc1Old, short enc2Old)
{
  readEncoders();

  short delta1 = enc1 - enc1Old;
  short delta2 = enc2 - enc2Old;

  errorEncoderP = delta2 - delta1;
  errorEncoderD = errorEncoderP - oldErrorEncoderP;

  totalErrorEncoder = (PEncoder * (float)errorEncoderP) + (DEncoder * (float)errorEncoderD);

  // Adjust motor to match the other
  m1Power += totalErrorEncoder;

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

  oldErrorEncoderP = errorEncoderP; 
  
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

bool isFrontWall()
{
  frontEncAvg = (r3 + r4 + frontOffset) / 2;
  if (frontEncAvg > frontWallValue)
  {
    return true;
  }
  return false;
}

void turnRight(short ticks, short m1Speed, short m2Speed)
{
  // ticks: RIGHTANGLE = 90 degree, 250 = 45 degree
  short enc1Old = encoderM1.read();

  while(encoderM1.read() <= (enc1Old + ticks))
  {
    motorPD(m1Speed, m2Speed);
    setMotorPower(m1Forward, m1Reverse, m1Speed);
    setMotorPower(m2Forward, m2Reverse, m2Speed);
    delay(50);
  }

  setMotorPower(m1Forward, m1Reverse, 0);
  setMotorPower(m2Forward, m2Reverse, 0);

  // Keep track of the orientation of the mouse
  if (dir == NORTH)
  {
    dir = EAST;
  }
  else if (dir == EAST)
  {
    dir = SOUTH;
  }
  else if (dir == SOUTH)
  {
    dir = WEST;
  }
  else
  {
    dir = NORTH;
  }

  Serial.print("dir: ");
  Serial.println(dir);

}

void turnLeft(short ticks, short m1Speed, short m2Speed)
{
  // ticks: RIGHTANGLE = 90 degree, 250 = 45 degree
  short enc_old = encoderM2.read();

  while(encoderM2.read() <= (enc_old + ticks))
  {
    motorPD(m1Speed, m2Speed);
    setMotorPower(m1Forward, m1Reverse, m1Power);
    setMotorPower(m2Forward, m2Reverse, m2Power);
    delay(50);
  }

  setMotorPower(m1Forward, m1Reverse, 0);
  setMotorPower(m2Forward, m2Reverse, 0);

  // Keep track of the orientation of the mouse
  if (dir == NORTH)
  {
    dir = WEST;
  }
  else if (dir == EAST)
  {
    dir = NORTH;
  }
  else if (dir == SOUTH)
  {
    dir = EAST;
  }
  else
  {
    dir = SOUTH;
  }

}

void reverse(short ticks, short speed)
{
  setMotorPower(m1Forward, m1Reverse, 0);
  setMotorPower(m2Forward, m2Reverse, 0);
  
  short enc1Old = encoderM1.read();
  short enc2Old = encoderM2.read();  

  while(encoderM1.read() >= (enc1Old - ticks))
  {
    encoderPD(enc1Old, enc2Old);
    motorPD(speed, speed);
    setMotorPower(m1Forward, m1Reverse, m1Power);
    setMotorPower(m2Forward, m2Reverse, m2Power);
    delay(50);
  }
  
  setMotorPower(m1Forward, m1Reverse, 0);
  setMotorPower(m2Forward, m2Reverse, 0);

}

void forward(short ticks, short speed)
{
  short enc1Old = encoderM1.read();
  short enc2Old = encoderM2.read();

  while(encoderM1.read() <= (enc1Old + ticks))
  {
    encoderPD(enc1Old, enc2Old);
    motorPD(speed, speed);
    setMotorPower(m1Forward, m1Reverse, m1Power);
    setMotorPower(m2Forward, m2Reverse, m2Power);
    delay(50);
  }
  
  setMotorPower(m1Forward, m1Reverse, 0);
  setMotorPower(m2Forward, m2Reverse, 0);

}

short getWalls()
{
  short walls = 0x0;
  frontEncAvg = (r3 + r4 + frontOffset) / 2;

  if (dir == NORTH)
  {
    if (frontEncAvg > frontWallValue)
    {
      walls += NORTH;
    }
    if (r1 > rightWallValue)
    {
      walls += EAST;
    }
    if (r2 > leftWallValue)
    {
      walls += WEST;
    }
  }
  else if (dir == EAST)
  {
    if (frontEncAvg > frontWallValue)
    {
      walls += EAST;
    }
    if (r1 > rightWallValue)
    {
      walls += SOUTH;
    }
    if (r2 > leftWallValue)
    {
      walls += NORTH;
    }
  }
  else if (dir == SOUTH)
  {
    if (frontEncAvg > frontWallValue)
    {
      walls += SOUTH;
    }
    if (r1 > rightWallValue)
    {
      walls += WEST;
    }
    if (r2 > leftWallValue)
    {
      walls += EAST;
    }
  }
  else
  {
    if (frontEncAvg > frontWallValue)
    {
      walls += WEST;
    }
    if (r1 > rightWallValue)
    {
      walls += NORTH;
    }
    if (r2 > leftWallValue)
    {
      walls += SOUTH;
    }
  }

  return walls;
}

void dirToTurn(short dirTurn)
{
  if (dir == NORTH)
  {
    if (dirTurn == EAST)
    {
      turnRight(RIGHTANGLE, 20, -20);
    }
    if (dirTurn == WEST)
    {
      turnLeft(RIGHTANGLE, -20, 20);
    }
    if (dirTurn == SOUTH)
    {
      turnLeft(RIGHTANGLE, -20, 20);
      turnLeft(RIGHTANGLE, -20, 20);
    }
  }
  else if (dir == EAST)
  {
    if (dirTurn == SOUTH)
    {
      turnRight(RIGHTANGLE, 20, -20);
    }
    if (dirTurn == NORTH)
    {
      turnLeft(RIGHTANGLE, -20, 20);
    }
    if (dirTurn == WEST)
    {
      turnLeft(RIGHTANGLE, -20, 20);
      turnLeft(RIGHTANGLE, -20, 20);
    }
  }
  else if (dir == SOUTH)
  {
    if (dirTurn == WEST)
    {
      turnRight(RIGHTANGLE, 20, -20);
    }
    if (dirTurn == EAST)
    {
      turnLeft(RIGHTANGLE, -20, 20);
    }
    if (dirTurn == NORTH)
    {
      turnLeft(RIGHTANGLE, -20, 20);
      turnLeft(RIGHTANGLE, -20, 20);
    }
  }
  else
  {
    if (dirTurn == NORTH)
    {
      turnRight(RIGHTANGLE, 20, -20);
    }
    if (dirTurn == SOUTH)
    {
      turnLeft(RIGHTANGLE, -20, 20);
    }
    if (dirTurn == EAST)
    {
      turnLeft(RIGHTANGLE, -20, 20);
      turnLeft(RIGHTANGLE, -20, 20);
    }
  }
}


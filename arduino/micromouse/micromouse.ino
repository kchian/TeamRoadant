#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>

void motormove(int, int, float);//foward, backward pin and rotation speed.

void travel(float, float);//moves motors foward at a given float speed for a given time
//**BACKWARDS IF SPEED INPUT IS NEGATIVE

void turn(float,float,float);
//Motor Enable pins *powering through VRegulator.

void emit(float);//activate emitters for a given time

int Receive();

// Built-in led
const int led = 13;

// Motor PWM pins
const int m1Forward = 3;
const int m1Reverse = 4;
const int m2Forward = 5;
const int m2Reverse = 6;

// Encoder digital pins
const int encoder_m1_A = 9;
const int encoder_m1_B = 10;
const int encoder_m2_A = 11;
const int encoder_m2_B = 12;

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
int reading[4]={0,0,0,0};
const int frontLeftReciever = 19;
const int leftReciever = 18;
const int frontRightReciever = 17;
const int rightReciever = 16;

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

  // Serial Monitor
  Serial.begin(9600);
  Serial.println("Micromouse: Team Roadent");
}

void loop()
{
  digitalWrite(emit1, HIGH);
  digitalWrite(emit2, HIGH);
  digitalWrite(emit3, HIGH);
  digitalWrite(emit4, HIGH);

  
  //digitalWrite(led, HIGH);   // set the LED on
  
  //delay(1000);                  // wait for a second
  //digitalWrite(led, LOW);    // set the LED off
  
  int r1;
  int r2;
  int r3;
  int r4;
  r1 = analogRead(frontRightReciever);
  r2 = analogRead(frontLeftReciever);
  r3 = analogRead(rightReciever);
  r4 = analogRead(leftReciever);


  Serial.print("Right Reciever: ");
  Serial.println(r3);
  Serial.print("Front Right Reciever: ");
  Serial.println(r1);
  Serial.print("Front Left Reciever: ");
  Serial.println(r2);
  Serial.print("Left Reciever: ");
  Serial.println(r4);
  Serial.println();

  delay(100);
}


void onLight()
{
  digitalWrite(13, HIGH);
  delay(1000);
  digitalWrite(13, LOW);
  delay(100);
}

void travel (float motorSpeed, float t)//moves  at speed s for time s
{
//*** speed  must be between 0 and 255;
if (motorSpeed>255 || motorSpeed<-255)
{
motorSpeed=constrain(motorSpeed,-255,255);
Serial.println("ERROR: SPEED OUT OF RANGE.");
}
motormove(m1Forward,m1Reverse,motorSpeed);
motormove(m2Forward,m2Reverse,motorSpeed);
delay(t);
}

/*
float Receive()//get reading from receivers
{
static int i;
int result
for (i=0;i<4;i++)
{
analogRead(receivers[i]);
}
return
}
*/

void emit(float t)//flash LED's for a given time t
{
int i;
for (i=0;i<4;i++)
{
digitalWrite(emitters[i],HIGH);
delay(t);//stay on for time t in ms 
digitalWrite(emitters[i],LOW);
delay(t);
}
}

void turn(float rightspeed,float leftspeed, float t)//turns by spinning motors at different speeds.
{
motormove(m1Forward,m1Reverse,rightspeed);
motormove(m2Forward,m2Reverse,leftspeed);
delay(t);//in milliseconds
}  

void motormove (int pinforward,int pinbackward ,float velocity)//velocity for a single motor
{
if (velocity>=0)
  {
  analogWrite(pinforward,velocity);
  digitalWrite(pinbackward,LOW);
  }
if (velocity<0)//reverse if motorvelocity is negative
  {
  analogWrite(pinbackward,-velocity);//makes value positive
  digitalWrite(pinforward,LOW);
  }
}

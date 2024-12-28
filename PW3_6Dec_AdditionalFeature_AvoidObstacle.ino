#include <Servo.h>

Servo myservo; 

int pos;    // Variable to store the servo position
int centerPosition = 78; // Real-world center alignment

#define in1 A1
#define in2 A2
#define in3 A3
#define in4 A4
#define enA 6
#define enB 5
#define trig 7
#define echo 4

float duration;
float distance;


void count1(){
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH);
  distance = duration * 0.034 / 2;
}

void move_straight() {
  digitalWrite(in1, LOW); 
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enA, 120);
  analogWrite(enB, 120);
}

void turn_right() {
  digitalWrite(in1, LOW); 
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enA, 150);
  analogWrite(enB, 150);
}

void stop() {
  digitalWrite(in1, LOW); 
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

void turn_left() {
  digitalWrite(in1, HIGH); 
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enA, 150);
  analogWrite(enB, 150);
}

void setup() {
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(trig, OUTPUT);

  myservo.attach(9); 
  myservo.write(78);
}

void loop() {

  move_straight();

  count1();
  
  if (distance < 30){
    turn_right();
    delay(800);
    move_straight();
    delay(500);
    stop();
    delay(1000);

    myservo.write(180);
    delay(1000);
    count1();
    float distanceTemp = distance;
    myservo.write(78);
    delay(10);
    if(distanceTemp < 50){
      move_straight();
      delay(500);
    }
    
    turn_left();
    delay(800);
    stop();
    delay(1000);
  }
}






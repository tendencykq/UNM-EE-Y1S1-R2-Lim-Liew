// MPU 6050
#include "Wire.h"       
#include "I2Cdev.h"     
#include "MPU6050.h" 
#define SAMPLE_COUNT 10  // Number of samples for averaging
float ax_avg = 0, ay_avg = 0, az_avg = 0;
float ax_samples[SAMPLE_COUNT], ay_samples[SAMPLE_COUNT], az_samples[SAMPLE_COUNT];
int sample_index = 0;   

MPU6050 mpu;
int16_t ax, ay, az;
int16_t gx, gy, gz;

MPU6050 imu; // Create an object for the MPU6050

float pitch = 0.0;
float roll = 0.0;
float yaw = 0.0;   // Yaw angle for 360° tracking
unsigned long prevTime = 0; // For time integration

struct MyData {
  byte X;
  byte Y;
  byte Z;
};

MyData data;

float distanceSinceLastReset = 0.0; // Partial distance from a reset point

// Headers
#include <LiquidCrystal.h>

// Motor Driver Declare
#define in1 A1
#define in2 A2
#define in3 A3
#define in4 2
#define enA 3
#define enB 11

// Encoder Declare
#define CLK_Encoder1 1
#define CLK_Encoder2 0

// IR sensor Declare
#define sensor_right 12
#define sensor_left 13 

// Display Declare 
#define button A0 

const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Display function declare with variables
void display();
int lastStateCLK_Encoder1;
int lastStateCLK_Encoder2;
volatile int counter_Encoder1 = 0; 
volatile int counter_Encoder2 = 0; 
unsigned long lastTime = 0;
volatile int lastCounter_Encoder1 = 0;
volatile int lastCounter_Encoder2 = 0;
float pulsesPerRevolution = 20.0;
float wheelCircumference = 20.895;
float distance_Encoder1 = 0.0;     
float distance_Encoder2 = 0.0; 
float timeInterval = 100.0; 
float avgDistance; 
unsigned long previousMillis = 0;
const unsigned long interval = 10;
float average;
float randomNum1;
float distanceSinceLastReset2;
float distanceSinceLastReset3;
float distanceSinceLastReset4;
float distanceSinceLastReset5;
bool isCountingPaused = false;
int countDifference_Encoder1;
int countDifference_Encoder2;

// Motion function delcare
void move_straight();
void turn_left();
void turn_right(); 
void stop();
void up_ramp();
void turn_360();
void search_for_line();
void down_ramp();
void resetPartialDistance();
void reset_avgDistance();
int x = 1;
int y = 2;
int z = 9;
int p = 1;
int o = 1;
int ok = 1;
int next = 0;

// Setup
void setup() {
  // LCD 
  lcd.begin(16, 2); 
  lcd.clear();
  lcd.setCursor(0, 0);
  // Encoder
  pinMode(CLK_Encoder1, INPUT);
  pinMode(CLK_Encoder2, INPUT);
  lastStateCLK_Encoder1 = digitalRead(CLK_Encoder1);
  lastStateCLK_Encoder2 = digitalRead(CLK_Encoder2);

  // IR sensor motor 
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(sensor_left, INPUT);
  pinMode(sensor_right, INPUT);
  Wire.begin();
  mpu.initialize();
}

void loop(){
  // IR sensor motor 
  unsigned long currentMillis = millis();
  display();
if (currentMillis - previousMillis >= interval) {
  previousMillis = currentMillis;
  
  int read_left = digitalRead(sensor_left);
  int read_right = digitalRead(sensor_right);

  mpu6050();
  delay(7);

  if (next == 1){
    digitalWrite(in1, LOW); 
    digitalWrite(in2, HIGH);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    analogWrite(enA, 150);
    analogWrite(enB, 150);
    next = next + 1;
  }


  if (read_left == 1 && read_right == 1){
    move_straight();
  }

  if (read_left == 1 && read_right == 0){
    delay(3);
    turn_right();
  }

  if (read_left == 0 && read_right == 1){
    delay(3);
    turn_left();
  }

  if (read_left == 0 && read_right == 0){
    delay(8);
    stop();
  }

  if (pitch >= 14 && y ==2){
    if (not_repeat == 1){
    up_ramp();
    not_repeat = not_repeat + 1;
    mpu6050();
    delay(5);
    while (pitch >= 14){
      up_ramp();
      mpu6050();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Angle: ");
      lcd.print(pitch);
      delay(5);
      if (pitch < 14 || pitch < 0){
        y = y + 1;
        if (z == 9){
        move_straight();
        delay(190);
        stop();
        delay(4000);
        reset_avgDistance();
        delay(2);
        turn_360();
        stop();
        delay(500);
        z = z + 1;
        next = next + 1;
        resetPartialDistance();
      }
      }
    }
    }

  }
  if (distanceSinceLastReset >= 0.6){
    resetPartialDistance2();
    distanceSinceLastReset = distanceSinceLastReset - 10000;
  }

  if (distanceSinceLastReset4 >= 1.44){
    stop();
    delay(2000);
    if (o == 1){
      digitalWrite(in1, LOW); 
      digitalWrite(in2, HIGH);
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
      analogWrite(enA, 120);
      analogWrite(enB, 120); 
      o = o + 1;
      distanceSinceLastReset5 = 1.5;
    }
    distanceSinceLastReset4 = distanceSinceLastReset4 - 1000000;
  }
}
}
// IR motor function
void move_straight(){
  digitalWrite(in1, LOW); 
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enA, 69);
  analogWrite(enB, 69);
}

void turn_right(){
  digitalWrite(in1, LOW); 
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enA, 240);
  analogWrite(enB, 249);
}

void turn_left(){
  digitalWrite(in1, HIGH); 
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enA, 247);
  analogWrite(enB, 247);
}

void stop(){
  digitalWrite(in1, LOW); 
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

void reset_avgDistance(){
  distanceSinceLastReset3 = 0.00;
}

// 360 turn
void turn_360() {
    while (1) { // Infinite loop
        display(); // Update display
        if (distanceSinceLastReset3 < 0.68) { // Check if the distance is less than the target
          digitalWrite(in1, HIGH); 
          digitalWrite(in2, LOW);
          digitalWrite(in3, LOW);
          digitalWrite(in4, HIGH);
          analogWrite(enA, 175);
          analogWrite(enB, 175);
        } else { 
            stop();
            break; // Exit the infinite loop
        }
    }
}


// Up ramp more power
void up_ramp(){
    digitalWrite(in1, LOW); 
    digitalWrite(in2, HIGH);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    analogWrite(enA, 180);
    analogWrite(enB, 180);
}

void down_ramp(){
    int power = 120; // Start with higher power
    power = max(60, power - 5); // Gradually reduce power to a minimum of 30
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH); // Forward
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    analogWrite(enA, power);
    analogWrite(enB, power);
    int read_left = digitalRead(sensor_left);
    int read_right = digitalRead(sensor_right);
    if (read_left == 0 && read_right ==0){
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH); // Forward
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
      analogWrite(enA, 74);
      analogWrite(enB, 74);
      delay(1500);
    }
    if (read_left == 1 && read_right ==1){
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH); // Forward
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
      analogWrite(enA, 74);
      analogWrite(enB, 74);
      delay(200);
    }
}

// Encoder function 
void display() {
  if (isCountingPaused) return; // Skip time counting if paused
  unsigned long currentTime = millis();
  int currentStateCLK_Encoder1 = digitalRead(CLK_Encoder1);
  if (currentStateCLK_Encoder1 != lastStateCLK_Encoder1 && currentStateCLK_Encoder1 == 1) {
    counter_Encoder1++;
  }
  lastStateCLK_Encoder1 = currentStateCLK_Encoder1;

  int currentStateCLK_Encoder2 = digitalRead(CLK_Encoder2);
  if (currentStateCLK_Encoder2 != lastStateCLK_Encoder2 && currentStateCLK_Encoder2 == 1) {
    counter_Encoder2++; 
  }
  lastStateCLK_Encoder2 = currentStateCLK_Encoder2;

  if (currentTime - lastTime >= timeInterval) {

    int countDifference_Encoder1 = counter_Encoder1 - lastCounter_Encoder1;
    distance_Encoder1 += ((countDifference_Encoder1 / pulsesPerRevolution) * wheelCircumference)/100;

    int countDifference_Encoder2 = counter_Encoder2 - lastCounter_Encoder2;t
    distance_Encoder2 += ((countDifference_Encoder2 / pulsesPerRevolution) * wheelCircumference)/100;
    
    lcd.setCursor(14, 0); // difference is here
    lcd.print(currentTime / 1000);
    
    lcd.setCursor(0, 1);
    lcd.print("Distance: ");

    distanceSinceLastReset3 += (distance_Encoder1 + distance_Encoder2)/2);

    distanceSinceLastReset += (((countDifference_Encoder1 + countDifference_Encoder2) / (2 * pulsesPerRevolution)) * wheelCircumference / 100);
    distanceSinceLastReset2 += (((countDifference_Encoder1 + countDifference_Encoder2) / (2 * pulsesPerRevolution)) * wheelCircumference / 100);



    distanceSinceLastReset4 += (((countDifference_Encoder1 + countDifference_Encoder2) / (2 * pulsesPerRevolution)) * wheelCircumference / 100);
    distanceSinceLastReset5 += (((countDifference_Encoder1 + countDifference_Encoder2) / (2 * pulsesPerRevolution)) * wheelCircumference / 100);
    lcd.print(distanceSinceLastReset5, 2);
    lcd.print(" m");
    lastCounter_Encoder1 = counter_Encoder1;
    lastCounter_Encoder2 = counter_Encoder2;
    lastTime = currentTime;
  }
}

void resetPartialDistance() {
    distanceSinceLastReset = 0.0;
    distanceSinceLastReset2 = 0.0;
}

void resetPartialDistance2() {
    distanceSinceLastReset4 = 0.0;
    distanceSinceLastReset5 = 0.0;
}
  

void mpu6050(){

      // Accelerometer
    mpu.getMotion6(&ax, &ay, &az, NULL, NULL, NULL);

    float ax_g = ax / 16384.0;
    float ay_g = ay / 16384.0;
    float az_g = az / 16384.0;

    ax_samples[sample_index] = ax_g;
    ay_samples[sample_index] = ay_g;
    az_samples[sample_index] = az_g;

    ax_avg = 0;
    ay_avg = 0;
    az_avg = 0;
    for (int i = 0; i < SAMPLE_COUNT; i++) {
        ax_avg += ax_samples[i];
        ay_avg += ay_samples[i];
        az_avg += az_samples[i];
    }
    ax_avg /= SAMPLE_COUNT;
    ay_avg /= SAMPLE_COUNT;
    az_avg /= SAMPLE_COUNT;

    sample_index = (sample_index + 1) % SAMPLE_COUNT;

    pitch = atan2(-ay_avg, sqrt(ax_avg * ax_avg + az_avg * az_avg)) * 180 / PI;
}
//Code to adjust the pulse values manually and find min, max pulse widths for each angle

// Current pulse widths found 
// BASE servo --> min = 110 | max = 510
// LEFT servo --> min = 340 | max = 510
// RIGHT servo --> min = 160 | max = 440

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define   BASE_MOTOR    0
#define   LEFT_MOTOR    1
#define   RIGHT_MOTOR   2

struct ServoData {
  String servo;
  int pin;
};

ServoData servos[] = {
  {"b", BASE_MOTOR},   
  {"l", LEFT_MOTOR},   
  {"r", RIGHT_MOTOR}   
};

String u_in;

void setup() {
	Serial.begin(9600);
  
	pwm.begin();
	pwm.setPWMFreq(50);
}

void loop() {
  u_in = Serial.readString();
  Serial.println(u_in);

  if(u_in.length() == 0) return;
  
  Serial.println(get_servo_pin(u_in.substring(0, 1)));
  Serial.println(u_in.substring(2).toFloat());

  int pin_ = get_servo_pin(u_in.substring(0, 1));
  int pulse_ = u_in.substring(2).toFloat();

  pwm.setPWM(pin_, 0, pulse_);
}

int get_servo_pin(String key) {
  for (int i = 0; i < 4; i++) {
    if (servos[i].servo == key) return servos[i].pin;  
  }
  return -1; 
}
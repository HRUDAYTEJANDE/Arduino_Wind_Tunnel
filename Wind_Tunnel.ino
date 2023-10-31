#include <Servo.h>
#include <LiquidCrystal_I2C.h>

#define Servo_minus 4
#define servo_preset 7
#define Servo_plus 2
#define Lock 9
#define Lights 10
int s_angle = 0; // angle of the servo
int lock = 0; // 0 refers to unlocked (the other side controls), 1 refers to locked
int count = 0;

Servo mainservo;

void setup() {
  pinMode(Servo_plus, INPUT_PULLUP);
  pinMode(Servo_minus, INPUT_PULLUP);
  pinMode(servo_preset, INPUT_PULLUP);
  pinMode(Lock, INPUT_PULLUP);
  mainservo.attach(11);
  mainservo.write(180);
  delay(1000);
  mainservo.write(0);// home the servo
}
void loop() {
  if (s_angle < 0) // limit the servo to 0 degrees
  {
    s_angle = 0;
  }
  if (s_angle > 60) // limit the servo to 60 degrees
  {
    s_angle = 60;
  }
  if (digitalRead(Servo_plus) == HIGH) {
    s_angle = s_angle + 2; // change angle by 1
    mainservo.write(s_angle);
    delay(10); // to help the servo reach the desired position
  }
  if (digitalRead(Servo_minus) == HIGH) {
    s_angle = s_angle - 2; // change the angle by 1
    mainservo.write(s_angle);
    delay(10); // to help the servo reach the desired position
  }
  if (digitalRead(servo_preset) == HIGH) {
    if (count = 0) {
      s_angle = 60; // change angle by 1
      mainservo.write(s_angle);
      delay(10); // to help the servo reach the desired position
      count = 1;
    } else {
      if (count = 1) {
        s_angle = 0; // change angle by 1
        mainservo.write(s_angle);
        delay(10); // to help the servo reach the desired position
        count = 0;
      }
    }
  }

}

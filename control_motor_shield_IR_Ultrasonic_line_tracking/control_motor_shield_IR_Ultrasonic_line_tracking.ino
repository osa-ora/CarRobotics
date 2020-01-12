/*
  Infrared controlled car
  It assumes 2 motors are attached one for front wheels
  and one for back wheels
  It also contains an ultrasonic sensor that prevent the car
  from moving forward if the safety distance is less than 5 cm
  missing ultrasonic sensors for the backward movement
  It also add the line tracking as the default for the car
*/
#include <IRremote.h>
// Connected pins to IR 
int RECV_PIN = 12;
// Connected pins to motor
// en for speed and inp1/2 for directions
int motor1_en = 6;
int motor1_inp1 = 7;
int motor1_inp2 = 8;
int motor2_en = 11;
int motor2_inp1 = 9;
int motor2_inp2 = 10;
//Ultrasonic sensor pins
int trig_pin = 4;
int echo_pin = 5;
//distance calculation variables
int distance = 0;
int safetyDistance = 0;
long duration = 0;
//line tracking variables
int direction_allowed = 0;
int rt_line_pin = 35;
int lt_1_line_pin = 30;
int lt_2_line_pin = 33;
int rt_value = 0;
int lt_value = 0;

IRrecv irrecv(RECV_PIN);
decode_results results;
void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn();
  pinMode(motor1_en, OUTPUT);
  pinMode(motor1_inp1, OUTPUT);
  pinMode(motor1_inp2, OUTPUT);
  pinMode(motor2_en, OUTPUT);
  pinMode(motor2_inp1, OUTPUT);
  pinMode(motor2_inp2, OUTPUT);
  pinMode(echo_pin, INPUT);
  pinMode(trig_pin, OUTPUT);
}
// move method, it take direction and speed parameters
void move(int direction, int speed) {
  if (direction == 1) {   //move forward
    digitalWrite(motor1_inp1, HIGH);
    digitalWrite(motor1_inp2, LOW);
    digitalWrite(motor2_inp1, HIGH);
    digitalWrite(motor2_inp2, LOW);
    analogWrite(motor1_en, speed);
    analogWrite(motor2_en, speed);
  } else if(direction == 2) { //move forward and right
    digitalWrite(motor1_inp1, HIGH);
    digitalWrite(motor1_inp2, LOW);
    digitalWrite(motor2_inp1, LOW);
    digitalWrite(motor2_inp2, LOW);
    analogWrite(motor1_en, speed);
    analogWrite(motor2_en, speed);
  } else if(direction == 3) { //move forward and left
    digitalWrite(motor1_inp1, LOW);
    digitalWrite(motor1_inp2, LOW);
    digitalWrite(motor2_inp1, HIGH);
    digitalWrite(motor2_inp2, LOW);
    analogWrite(motor1_en, speed);
    analogWrite(motor2_en, speed);
  } else if (direction == 4) { //move backword
    digitalWrite(motor1_inp1, LOW);
    digitalWrite(motor1_inp2, HIGH);
    digitalWrite(motor2_inp1, LOW);
    digitalWrite(motor2_inp2, HIGH);
    analogWrite(motor1_en, speed);
    analogWrite(motor2_en, speed);
  } else {                      // don't move i.e. stop
    digitalWrite(motor1_inp1, LOW);
    digitalWrite(motor1_inp2, LOW);
    digitalWrite(motor2_inp1, LOW);
    digitalWrite(motor2_inp2, LOW);
    analogWrite(motor1_en, speed);
    analogWrite(motor2_en, speed);
  }
}
boolean checkDistance (int threshold)
{
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(10);
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);
  duration = pulseIn(echo_pin, HIGH);
  distance = duration * 0.034 / 2;
  safetyDistance = distance;
  //Serial.println(duration);
  Serial.println(safetyDistance);
  if (safetyDistance <= threshold) {//threshold in cm
    Serial.println("Not Safe");
    return false;
  } else {
    Serial.println("Safe");
    return true;
  }
}
//check distance with threshold, it set direction_allowed variables to either 0,1,2 or 3
void checkDirection () {
  rt_value = digitalRead(rt_line_pin);
  lt_value = digitalRead(lt_1_line_pin) + digitalRead(lt_2_line_pin);
  Serial.println(rt_value);
  Serial.println(lt_value);
  direction_allowed = 0;
  if (rt_value == 0) direction_allowed = 2; //go right
  if (lt_value < 2) direction_allowed = 3; //go left
  //if (rt_value == 0 && lt_value < 2) direction_allowed = 0; //stop
  if (rt_value == 1 && lt_value == 2) direction_allowed = 1; //move forward
  //what is the decision now ?
  if (direction_allowed == 0) Serial.println("STOP, end of line!");
  if (direction_allowed == 1) Serial.println("Forward is following the line!");
  if (direction_allowed == 2) Serial.println("Need to move also right to back to line");
  if (direction_allowed == 3) Serial.println("Need to move also left to back to line");
}
void loop() {
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    if (results.value == 0xFFC23D || results.value == 0x20FE4DBB) {
      Serial.println("Forward is requested!");
      if (checkDistance(5)) { // 5 CM is there
        checkDirection();
        move(direction_allowed, 200);
      }
    } else if (results.value == 0xFF22DD || results.value == 0x52A3D41F) {
      Serial.println("Backword is okay"); //need to be removed, no backward is required as all sensors in the front
      move(4, 200);
    } else {
      Serial.println("No way to go so STOP!");
      move(0, 0);
    }
    irrecv.resume();
  }
}

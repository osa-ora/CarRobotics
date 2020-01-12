/*
  Infrared controlled car
  It assumes 2 motors are attached one for front wheels
  and one for back wheels
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

}
void move(int direction, int speed){
  if(direction==1){ //move forward
    digitalWrite(motor1_inp1,HIGH);
    digitalWrite(motor1_inp2,LOW);
    digitalWrite(motor2_inp1,HIGH);
    digitalWrite(motor2_inp2,LOW);
    analogWrite(motor1_en,speed);
    analogWrite(motor2_en,speed);
  }else if(direction==2){    //move backword
    digitalWrite(motor1_inp1,LOW);
    digitalWrite(motor1_inp2,HIGH);
    digitalWrite(motor2_inp1,LOW);
    digitalWrite(motor2_inp2,HIGH);    
    analogWrite(motor1_en,speed);
    analogWrite(motor2_en,speed);
  }else {
    digitalWrite(motor1_inp1,LOW);
    digitalWrite(motor1_inp2,LOW);
    digitalWrite(motor2_inp1,LOW);
    digitalWrite(motor2_inp2,LOW);    
    analogWrite(motor1_en,speed);
    analogWrite(motor2_en,speed);    
  }
  
}
void loop() {
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    if (results.value == 0xFFC23D || results.value == 0x20FE4DBB) {
      move(1,200);
      Serial.println("Forward");
    } else if (results.value == 0xFF22DD || results.value == 0x52A3D41F){
      Serial.println("Backword");
      move(2,200);
    } else{
      Serial.println("Stop");
      move(0,0);      
    }
    irrecv.resume();
  }
}

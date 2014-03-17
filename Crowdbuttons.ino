#include <Process.h>

int buttonPin = 2;
int buttonPin2 = 3;
int buttonPin3 = 4;
int buttonPin4 = 5;

int ledPin = 8;
int ledPin2 = 9;
int ledPin3 = 10;
int currState = LOW;
int prevState = LOW;
int currState2 = LOW;
int prevState2 = LOW;
int currState3 = LOW;
int prevState3 = LOW;
int currState4 = LOW;
int prevState4 = LOW;

int brightness = 0;
int fadeAmount = 5;

bool working = false;


//profile
String device_id = "53267a7c08df4f000247d843"; //manually, should use web interface to assign
String question_id = "53267e1908df4f000247d845";
String api_url = "http://crowdbuttons.herokuapp.com/add_answer/"+question_id+"/";


void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(buttonPin3, INPUT);
  pinMode(buttonPin4, INPUT);
  
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  digitalWrite(buttonPin, HIGH);
  digitalWrite(buttonPin2, HIGH);
  digitalWrite(buttonPin3, HIGH);
  digitalWrite(buttonPin4, HIGH);
  digitalWrite(ledPin, LOW);
  digitalWrite(ledPin2, LOW);
  digitalWrite(ledPin3, LOW);
  
  Bridge.begin();
}

void loop() {
  currState = digitalRead(buttonPin);
  currState2 = digitalRead(buttonPin2);
  currState3 = digitalRead(buttonPin3);
  currState4 = digitalRead(buttonPin4);
  
  //button 1
  if(working == false && currState == LOW && prevState == HIGH){
    working = true;
    allHigh();
    runCurl("0");
    moving(2);
    working = false;
  }else{
    allLow();
  }
  
  //button 2
  if(working == false && currState2 == LOW && prevState2 == HIGH){
    working = true;
    allHigh();
    runCurl("1");
    moving(2);
    working = false;
  }else{
    allLow();
  }
  
  //button 3
  if(working = true && currState3 == LOW && prevState3 == HIGH){
    working = true;
    allHigh();
    runCurl("2");
    moving(2);
    working = false;
  }else{
    allLow();
  }
  
  //button 4
  if(working = true && currState4 == LOW && prevState4 == HIGH){
    working = true;
    allHigh();
    runCurl("3");
    moving(2);
    working = false;
  }else{
    allLow();
  }
  
  //breathLight(10);
  
  prevState = currState;
  prevState2 = currState2;
  prevState3 = currState3;
  prevState4 = currState4;
}

void moving(int count){
  
  for(int i=0; i<count; i++){
    digitalWrite(ledPin, HIGH);
    digitalWrite(ledPin2, LOW);
    digitalWrite(ledPin3, LOW);
    delay(200);
  
    digitalWrite(ledPin, LOW);
    digitalWrite(ledPin2, HIGH);
    digitalWrite(ledPin3, LOW);
    delay(200);
  
    digitalWrite(ledPin, LOW);
    digitalWrite(ledPin2, LOW);
    digitalWrite(ledPin3, HIGH);
    delay(200);
  }  
}
void allHigh(){
  digitalWrite(ledPin, HIGH);
  digitalWrite(ledPin2, HIGH);
  digitalWrite(ledPin3, HIGH);
  delay(200);
}
void allLow(){
  digitalWrite(ledPin, LOW);
  digitalWrite(ledPin2, LOW);
  digitalWrite(ledPin3, LOW);
}

void breathLight(int count){
  analogWrite(ledPin2, brightness);
  
  brightness = brightness + fadeAmount;
  
  if(brightness == 0 || brightness == 255){
    fadeAmount = -fadeAmount;
  }
  delay(30);
}

void runCurl(String ans){
  Process p;
  p.begin("curl");
  p.addParameter(api_url+ans+"?created_user=yun1&device_id="+device_id);
  p.run();
}

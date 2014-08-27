#include <Process.h>
#include <HttpClient.h>
#include <Bridge.h>


int buttonPin = 2;
int buttonPin2 = 4;
int buttonPin3 = 7;
int buttonPin4 = 12;

int ledPin = 8;
int ledPin2 = 9;
int ledPin3 = 10;

int motionPin = 13;

//button LED
int buttonLED4 = 3;
int buttonLED3 = 5;
int buttonLED2 = 6;
int buttonLED1 = 11;

int currState = LOW;
int prevState = LOW;
int currState2 = LOW;
int prevState2 = LOW;
int currState3 = LOW;
int prevState3 = LOW;
int currState4 = LOW;
int prevState4 = LOW;

int currMotionState = LOW;
int prevMotionState = LOW;

int brightness = 0;
int fadeAmount = 20;
bool working = false;
bool guiding = false;

char data[10];
String response;
String ans;
String guide_str;
String url_str;

long startTime;
long long elapsedTime;

//profile
//device #1 (yun1): "53267a7c08df4f000247d843"
//device #2 (yun2): "5326e2f73e3d8b0002b98b0d" //
//device #3 (yun3): "5326e3913e3d8b0002b98b0e" //
//device #0 (yun0): "53746b5beea0850002578ec8"
//device #experiment (yunExp): "53e87b719f0c8d0002cd1c93"
String device_id = "5326e3913e3d8b0002b98b0e"; //manually, should use web interface to assign
//testing question: 531d9d7f15027e00026d51b6
//room question: 53267e1908df4f000247d845
String room_question_id = "53267e1908df4f000247d845";

String api_url = "http://crowdbuttons.herokuapp.com/add_answer/";
String device_name = "yun3";

//R324/R326 status
String api_url_for_room = "http://crowdbuttons.herokuapp.com/get_status/"+room_question_id;
String api_url_for_guide = "http://crowdbuttons.herokuapp.com/get_guide/"+room_question_id;

String motion_question_id = "53faf8cdd3c6ab0002c0d020";

//8x8 matrix
String matrix_agent = "http://10.5.1.196/arduino/";

//for which room
String room_name = "R324/R326";

boolean LEDstatus1=false;
boolean LEDstatus2=false;
boolean LEDstatus3=false;
boolean LEDstatus4=false;

String input;
String command;

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(buttonPin3, INPUT);
  pinMode(buttonPin4, INPUT);
  
  pinMode(motionPin, INPUT);
  
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);

  //button LED
  pinMode(buttonLED1, OUTPUT);
  pinMode(buttonLED2, OUTPUT);
  pinMode(buttonLED3, OUTPUT);
  pinMode(buttonLED4, OUTPUT);

  digitalWrite(buttonPin, HIGH);
  digitalWrite(buttonPin2, HIGH);
  digitalWrite(buttonPin3, HIGH);
  digitalWrite(buttonPin4, HIGH);
  
  digitalWrite(ledPin, LOW);
  digitalWrite(ledPin2, LOW);
  digitalWrite(ledPin3, LOW);
  
  digitalWrite(buttonLED1, LOW);
  digitalWrite(buttonLED2, LOW);
  digitalWrite(buttonLED3, LOW);
  digitalWrite(buttonLED4, LOW);
  
  Bridge.begin();
  
  guiding = false;
}

void loop() {
  
  if(LEDstatus1) breathLight(buttonLED1);
  else turnOffButtonLED(buttonLED1);
  
  if(LEDstatus2) breathLight(buttonLED2);
  else turnOffButtonLED(buttonLED2);
  
  if(LEDstatus3) breathLight(buttonLED3);
  else turnOffButtonLED(buttonLED3);
  
  if(LEDstatus4) breathLight(buttonLED4);
  else turnOffButtonLED(buttonLED4);
  
  //---------
  currState = digitalRead(buttonPin);
  currState2 = digitalRead(buttonPin2);
  currState3 = digitalRead(buttonPin3);
  currState4 = digitalRead(buttonPin4);

  currMotionState = digitalRead(motionPin);
  
  //detect people
  if(currMotionState == HIGH && prevMotionState == LOW){
    Console.println("detect people");
    if(guiding){
      Console.print("guiding!-->");
      Console.println(guide_str);
    }
    else Console.println("no guiding");
    
    if(guiding == false){
      Console.println("--------call---------");
      submitAns("0", motion_question_id);
      //getGuide(room_name);
      guiding = true;
      delay(500);
    }
    startTime = millis();
  }
  
  
  if(guiding == true){
    elapsedTime =  millis() - startTime;
    Console.println(int(elapsedTime/1000));
    
    //stop guiding
    if(elapsedTime >= 15000){
      submitAns("1", motion_question_id);
      guiding = false;
      LEDstatus1 = false;
      LEDstatus2 = false;
      LEDstatus3 = false;
      LEDstatus4 = false;
      startTime = millis();
    }
  }
  
  //button 1
  if(working == false && currState == LOW && prevState == HIGH){
    turnOffAllButtonLED();
    turnOnButtonLED(buttonLED1);
    
    working = true;
    allHigh();
    submitAns("0", room_question_id);
    moving(2);  
    working = false;
    
    if(room_name.equals("R324/R326")){
      sendFeedbackForDisplay("0");
      delay(10);
    }
    
    //turn off guiding mode
    resetStatusAllButtonLED();
    turnOffAllButtonLED();
    guiding = false;

  }else{
    allLow();
  }
  
  //button 2
  if(working == false && currState2 == LOW && prevState2 == HIGH){
    turnOffAllButtonLED();
    turnOnButtonLED(buttonLED2);
    working = true;
    allHigh();
    submitAns("1", room_question_id);
    moving(2);
    working = false;
    
    if(room_name.equals("R324/R326")){
      sendFeedbackForDisplay("1");
      delay(10);
    }
    
    //turn off guiding mode
    resetStatusAllButtonLED();
    turnOffAllButtonLED();
    guiding = false;

  }else{
    allLow();
  }
  
  //button 3
  if(working = true && currState3 == LOW && prevState3 == HIGH){
    turnOffAllButtonLED();
    turnOnButtonLED(buttonLED3);
    working = true;
    allHigh();
    submitAns("2", room_question_id);
    moving(2);
    working = false;
    
    if(room_name.equals("R324/R326")){
      sendFeedbackForDisplay("2");
      delay(10);
    }
    
    resetStatusAllButtonLED();
    turnOffAllButtonLED();
    guiding = false;
    
  }else{
    allLow();
  }
  
  //button 4
  if(working = true && currState4 == LOW && prevState4 == HIGH){
    turnOffAllButtonLED();
    turnOnButtonLED(buttonLED4);
    working = true;
    allHigh();
    submitAns("3", room_question_id);
    moving(2);
    working = false;
    
    if(room_name.equals("R324/R326")){
      sendFeedbackForDisplay("3");
      delay(10);
    }
    
    resetStatusAllButtonLED();
    turnOffAllButtonLED();
    guiding = false;
    
  }else{
    allLow();
  }
  
  prevState = currState;
  prevState2 = currState2;
  prevState3 = currState3;
  prevState4 = currState4;
  
  prevMotionState = currMotionState;
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
    
    digitalWrite(ledPin, LOW);
    digitalWrite(ledPin2, LOW);
    digitalWrite(ledPin3, LOW);
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

void breathLight(int ledId){
  analogWrite(ledId, brightness);
  
  brightness = brightness + fadeAmount;
  
  if(brightness <= 0 || brightness >= 255){
    fadeAmount = -fadeAmount;
  }
  delay(30);
}


void submitAns(String ans, String question_id){
  Console.println(api_url+question_id+"/"+ans+"?created_user="+device_name+"&device_id="+device_id);
  HttpClient hclient;
  hclient.get(api_url+question_id+"/"+ans+"?created_user="+device_name+"&device_id="+device_id);
  response = "";
  delay(20);
  while (hclient.available()) {
    char c = hclient.read();
    response += c;
  }
}


void getGuide(String room){
  Console.println(api_url_for_guide+"?location="+room);
  HttpClient hclient;
  hclient.get(api_url_for_guide+"?location="+room);
  response = "";
  delay(50);
  while (hclient.available()) {
    char c = hclient.read();
    response += c;
  }
  response.trim();
  int lastIndex = response.lastIndexOf(':');
  ans = response.substring(lastIndex+1);
  guide_str = response.substring(0, lastIndex);
  
  setLEDstatus(guide_str);
}


String getStatus(String room){
  HttpClient hclient; 
  hclient.get(api_url_for_room+"?location="+room+"&for_display=1");
  response = "";
  while (hclient.available()) {
    char c = hclient.read();
    response += c;
  }
  response.trim();
  
  return response;
}

void turnOnButtonLED(int ledId){
  analogWrite(ledId, 255);
}

void turnOffButtonLED(int ledId){
  analogWrite(ledId, 0);
}

void turnOffAllButtonLED(){  
  turnOffButtonLED(buttonLED1);
  turnOffButtonLED(buttonLED2);
  turnOffButtonLED(buttonLED3);
  turnOffButtonLED(buttonLED4);
}
void resetStatusAllButtonLED(){
  LEDstatus1 = false;
  LEDstatus2 = false;
  LEDstatus3 = false;
  LEDstatus4 = false;
}

void setLEDstatus(String input){
  int commaIndex = input.indexOf(',');
  int secondCommaIndex = input.indexOf(',', commaIndex+1);
  int lastCommaIndex = input.lastIndexOf(',');
  
  String status1 = input.substring(0, commaIndex);;
  String status2 = input.substring(commaIndex+1, secondCommaIndex);
  String status3 = input.substring(secondCommaIndex+1, lastCommaIndex);
  String status4 = input.substring(lastCommaIndex+1);
  
  //button LED 1
  if(status1.equals("1")) LEDstatus1 = true;
  else LEDstatus1 = false;
  
  //button LED 2
  if(status2.equals("1")) LEDstatus2 = true;
  else LEDstatus2 = false;
  
  //button LED 3
  if(status3.equals("1")) LEDstatus3 = true;
  else LEDstatus3 = false;
  
  //button LED 4
  if(status4.equals("1")) LEDstatus4 = true;
  else LEDstatus4 = false;

  if(LEDstatus1 && LEDstatus2 && LEDstatus3 && LEDstatus4) guiding = true;
  else guiding = false;
}

void sendFeedbackForDisplay(String statusInd){
  Process p;
  p.runShellCommand("curl -u root:arduino "+matrix_agent+statusInd);
}


#include <Process.h>
#include <HttpClient.h>
#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>

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
String device_id = "53267a7c08df4f000247d843"; //manually, should use web interface to assign
//testing question: 531d9d7f15027e00026d51b6
//room question: 53267e1908df4f000247d845
String question_id = "53267e1908df4f000247d845";

String api_url = "http://crowdbuttons.herokuapp.com/add_answer/"+question_id+"/";
String device_name = "yun1";

//R324/R326 status
String api_url_for_room = "http://crowdbuttons.herokuapp.com/get_status/"+question_id;
String api_url_for_guide = "http://crowdbuttons.herokuapp.com/get_guide/"+question_id;

//8x8 matrix
String matrix_agent = "http://10.5.1.196/arduino/";

//for which room
String room_name = "R324/R326";

YunServer server;

boolean LEDstatus1=false;
boolean LEDstatus2=false;
boolean LEDstatus3=false;
boolean LEDstatus4=false;

int feedbackType;
int guidanceType;


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
  
  // Listen for incoming connection only from localhost
  // (no one from the external network could connect)
  server.listenOnLocalhost();
  server.begin();
  reset8x8LED();
  guiding = false;
  feedbackType = 0;
}

void loop() {
  
  YunClient client = server.accept();
  
  if (client) {
    // read the command
    String command = client.readStringUntil('/');
    command.trim();        //kill whitespace
    if (command == "buttons") {
      String input = client.readString();
      input.trim();
      client.println("turn on Button LED: "+input);
      
      int lastIndex = input.lastIndexOf(':');
      ans = input.substring(lastIndex+1);
      guide_str = input.substring(0, lastIndex);
      setLEDstatus(guide_str);
      guiding = true;
    }
    
    if (command == "feedback"){
      String input = client.readString();
      input.trim();
      
      feedbackType = input.toInt();
      client.println("feedback: "+feedbackType);
    }
    
    // Close connection and free resources.
    client.stop();
  }
  delay(50); // Poll every 50ms
  
  //---------
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
      getGuide(room_name);
      guiding = true;
      delay(500);
    }
    startTime = millis();
  }
  
  
  if(guiding == true){
    elapsedTime =  millis() - startTime;
    //Console.println(int(elapsedTime/1000));
    
    if(elapsedTime >= 20000){
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
    runCurl("0");
    moving(2);  
    working = false;
    
    Console.println(ans);
    Console.println(LEDstatus1);
    Console.println(LEDstatus2);
    Console.println(LEDstatus3);
    Console.println(LEDstatus4);
    
    //send feedback to feedback agent
    //feedback(feedbackType, ans, "0", LEDstatus1);
/*    if(ans.equals("0")){
      sendSmile();
    }else{
      
      if(LEDstatus1 == true)
        sendBecomeSmile();
      else
        sendCry();
    }*/
    
    //turn off guiding mode
    resetStatusAllButtonLED();
    turnOffAllButtonLED();
    guiding = false;
    
    //reset
    reset8x8LED();
  }else{
    allLow();
  }
  
  //button 2
  if(working == false && currState2 == LOW && prevState2 == HIGH){
    turnOffAllButtonLED();
    turnOnButtonLED(buttonLED2);
    working = true;
    allHigh();
    runCurl("1");
    moving(2);
    working = false;
    
    Console.println(ans);
    Console.println(LEDstatus1);
    Console.println(LEDstatus2);
    Console.println(LEDstatus3);
    Console.println(LEDstatus4);
    
    
    //send feedback
    //feedback(feedbackType, ans, "1", LEDstatus2);
    /*if(ans.equals("1")) sendSmile();
    else{
      if(LEDstatus2 == true) sendBecomeSmile();
      else sendCry();
    }*/
    
    //turn off guiding mode
    resetStatusAllButtonLED();
    turnOffAllButtonLED();
    guiding = false;
    
    //reset
    reset8x8LED();
  }else{
    allLow();
  }
  
  //button 3
  if(working = true && currState3 == LOW && prevState3 == HIGH){
    turnOffAllButtonLED();
    turnOnButtonLED(buttonLED3);
    working = true;
    allHigh();
    runCurl("2");
    moving(2);
    working = false;
    
    Console.println(ans);
    Console.println(LEDstatus1);
    Console.println(LEDstatus2);
    Console.println(LEDstatus3);
    Console.println(LEDstatus4);
    
    
    //feedback(feedbackType, ans, "2", LEDstatus3);
    /*
    if(ans.equals("2")) sendSmile();
    else{
      if(LEDstatus3 == true) sendBecomeSmile();
      else sendCry();
    }*/
    
    resetStatusAllButtonLED();
    turnOffAllButtonLED();
    guiding = false;
    
    reset8x8LED();
  }else{
    allLow();
  }
  
  //button 4
  if(working = true && currState4 == LOW && prevState4 == HIGH){
    turnOffAllButtonLED();
    turnOnButtonLED(buttonLED4);
    working = true;
    allHigh();
    runCurl("3");
    moving(2);
    working = false;
    
    
    Console.println(ans);
    Console.println(LEDstatus1);
    Console.println(LEDstatus2);
    Console.println(LEDstatus3);
    Console.println(LEDstatus4);
    
    
    //feedback(feedbackType, ans, "3", LEDstatus4);
    /*if(ans.equals("3")) sendSmile();
    else{
      if(LEDstatus4 == true) sendBecomeSmile();
      else sendCry();
    }*/
    
    resetStatusAllButtonLED();
    turnOffAllButtonLED();
    guiding = false;
    
    reset8x8LED();
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

void runCurl(String ans){
  Process p;
  p.begin("curl");
  p.addParameter(api_url+ans+"?created_user="+device_name+"&device_id="+device_id);
  p.run();
}
void getGuide(String room){
  HttpClient hclient;
  hclient.get(api_url_for_guide+"?location="+room);
  response = "";
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
  Console.print("get room status:");
  //Console.println(room);
  url_str = "fetch url:"+api_url_for_room+"?location="+room+"&for_display=1";
  //Console.println(api_url_for_room);
  //Console.println(api_url_for_room+"?");
  //Console.println("?location="+room+"&for_display=1");
  //Console.println(api_url_for_room+"?location="+room+"&for_display=1");
  Console.println(url_str);
  HttpClient hclient; 
  hclient.get(api_url_for_room+"?location="+room+"&for_display=1");
  response = "";
  while (hclient.available()) {
    char c = hclient.read();
    response += c;
  }
  response.trim();
  Console.println("status: ");
  Console.println(response);
  
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


void sendSmile(){
  Console.println("smile");
  Process p;
  p.runShellCommand("curl -u root:arduino "+matrix_agent+"4");
}

void sendCry(){
  Console.println("cry");
  Process p;
  p.runShellCommand("curl -u root:arduino "+matrix_agent+"5");
}

void sendBecomeSmile(){
  Console.println("become simle");
  Process p;
  p.runShellCommand("curl -u root:arduino "+matrix_agent+"6");
}

void sendMeow(){
  Console.println("meow");
  Process p;
  p.runShellCommand("curl -u root:arduino "+matrix_agent+"7");
}

void reset8x8LED(){
  delay(100);
  sendStatusForDisplay(room_name);
}

void sendStatusForDisplay(String room){
  Process p;
  String displayStatus = getStatus(room);
  displayStatus.trim();
  Console.println("send: room"+room);
  Console.println(displayStatus);
  if(displayStatus.equals("Empty")) p.runShellCommand("curl -u root:arduino "+matrix_agent+"0");
  if(displayStatus.equals("Meeting")) p.runShellCommand("curl -u root:arduino "+matrix_agent+"1");
  if(displayStatus.equals("Course")) p.runShellCommand("curl -u root:arduino "+matrix_agent+"2");
  if(displayStatus.equals("Study")) p.runShellCommand("curl -u root:arduino "+matrix_agent+"3");
}

String getIndexFromStatus(String roomStatus){
  if(roomStatus.equals("Empty")) return "0";
  if(roomStatus.equals("Meeting")) return "1";
  if(roomStatus.equals("Course")) return "2";
  if(roomStatus.equals("Study")) return "3";
}

void feedback(int type, String groundTrue, String ans, boolean LEDstatus){
   switch(type){
     case 0:
       reset8x8LED();
       break;
     case 1:
       Console.println("status");
       break;
     case 2:
       Console.println("positive");
       sendSmile();
       break;
     case 3:
       Console.println("random positive");
       if(random(2) == 1) sendSmile();
       break;
     case 4:
       Console.println("negative");
       sendCry();
       break;
     case 5:
       Console.println("random negative");
       if(random(2) == 1) sendCry();
       break;
     case 6:
       Console.println("only guidance positive");
       if(LEDstatus == true) sendSmile();
       break;
     case 7:
       Console.println("guidance positive + negative");
       if(LEDstatus == true)  sendSmile();
       else  sendCry();
       break;
     case 8:
       Console.println("2-level positive + 1 negative");
       if(ans.equals(groundTrue)){
         sendSmile();
       }else{
         if(LEDstatus == true)
           sendBecomeSmile();
         else
           sendCry();
         }
     default:
       Console.println("default");
       break;
   }
}

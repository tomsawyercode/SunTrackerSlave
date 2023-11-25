

#include <Wire.h>
const byte MY_ADDRESS = 42; //I2C Address

#include <TinyGPS++.h>
//http://arduiniana.org/libraries/tinygpsplus/

#include "commands.h"  
#include "configurations.h"
#include "Tracker.h"


// GPS Date Time
TinyGPSPlus gps;
TinyGPSCustom satsInView(gps, "GPGSV", 3);  // $GPGSV sentence, third element of the sentence
//use satsInView.value(); or gps.satellites.value()// Satellites in use
//---------------------------------------------
Tracker tracker;      

//---------------------------------------------
// I2C
//---------------------------------------------

//Mega2560-->sizeof(int)=2,sizeof(long)=4,sizeof(float)=4,

// Struct for incoming command and values
//not work: volatile struct {long code;float v1;float v2; } request={0,0,0}; 
//not work: struct { volatile long code;float v1;float v2; } request={0,0,0}; 

struct {long code;  float v1; float v2; } request={0,0,0};//4+4+4


//I2C Command Code, volatile allow to be modified by an Interruption
volatile int I2Ccode=0;// without volatile doesn't work
volatile bool newCmd = false;// with bool doesn't work

//---------------------------------------------
// GPS Responses from MEGA to ESP
//---------------------------------------------
//struct {long v1;long v2;long v3;} responseTime={12,10,10};
//struct {long v1;long v2;long v3;} responseDate={2020,10,5};

//---------------------------------------------
// Update time interval 
//--------------------------------------------- 
unsigned long prevTime = 0;
  
//################################################
int ledPin = 13;//2 Lolin; //13 Arduino MEGA,
void setup() {

    // Set up I2C receive handlers

    Wire.begin (MY_ADDRESS);
    Wire.onReceive (receiveEvent);  // interrupt handler for incoming messages
    Wire.onRequest (requestEvent);  // interrupt handler for when data is wanted

      
    pinMode(ledPin, OUTPUT);
    
    Serial.begin(9600);
   
    // GPS, read from slave
    Serial2.begin(9600);
    
    Serial.println("Slave start: ---------------------------------");
    printMenu();
    
    // TODO:  save an retrieve this values from EEPROM  
    tracker.setShift(SHIFT_AZIMUTH,SHIFT_ELEVATION); /// tracker.setShift(0,0); 
    tracker.setTarget(TARGET_AZIMUTH,TARGET_ELEVATION );
    tracker.setParking(PARKING_AZIMUTH,PARKING_ELEVATION );
     
    // Move system to ZERO
    // here or call it from master
    //tracker.goAzimuthEndstop();//set steppers to 0
    //tracker.goElevationEndstop();//set steppers to 0


}

void loop() { 
  
    //---------------------------------------------
    // Read Serial Command 
    //---------------------------------------------
    if (Serial.available())
    {    
    int cod = 0;
    float v1=0,v2=0;
    parseSerial(cod,v1,v2);
    //Serial.print(" Serial cod:");  Serial.println(cod);    
    if (cod > 0)  SerialCommand(cod,v1,v2);  
    }
      
    //---------------------------------------------
    // I2C Command request 
    //---------------------------------------------
    
    
    if(newCmd==true){
      Serial.print("new I2C Command"); Serial.print(", I2Ccode:");Serial.println(I2Ccode); 
      newCmd = false;//Clean here allow to run another command even if the previous I2C_Command call does not finished yet
      I2C_Command();
      // Test Led  on-off
      //if (I2Ccode==LED_ON) digitalWrite(ledPin,HIGH);//12
      //if (I2Ccode==LED_OFF) digitalWrite(ledPin,LOW);//13     
      //Serial.print("newCmd cleaned:"); Serial.println(newCmd);
            
    }
    
    // alternative, but can't run two command
    // if(I2Ccode!0 && I2Ccode<100) //there is a command
    // { I2C_Command(); I2Ccode=0;} //use and then clean it 
    
    //---------------------------------------------    
    // Retrieve GPS data from Slave
    //---------------------------------------------
    //smartDelay(1000);// never use, it is not necessary
    
     while (Serial2.available()){
      gps.encode(Serial2.read());
      }
      
    
     // Populate Date Time responses to avoid time consuming in OnRequest
     /*
     if ((millis()-prevTime) > 1000)
     {
        prevTime = millis();
        responseTime = {gps.time.hour(),gps.time.minute(),gps.time.second()};
        responseDate = {gps.date.year(),gps.date.month(),gps.date.day()};
        //printDateTime();
           
     }*/

      /*
      if ((millis()-prevTime) > 4000)
       {
          noInterrupts();  
          prevTime = millis();
          Serial.print(millis()/1000%60);Serial.print(", newCmd:"); Serial.print(newCmd); Serial.print(", I2Ccode:");Serial.print(I2Ccode); 
          Serial.print(", request.code:");Serial.println(request.code);          
          char msg[100]={0};  
          sprintf(msg,"%02d:%02d, newCmd: %d, I2Ccode: %d , request.code: %d \r\n",(millis()/60000) %60,millis()/1000%60,199,299,request.code);//(int)newCmd,(int)I2Ccode
          Serial.print(msg);
          interrupts();
           
       }*/
     
}
//-----------------------------------------------
//  Serial command parser
//-----------------------------------------------
void parseSerial(int  &cod,float & out1, float & out2){

    char commandBuffer[20]={NULL};         
    //int bytesCount = Serial.readBytesUntil('\n',commandBuffer,20);
     Serial.readBytesUntil('\n',commandBuffer,20);
    //Serial.println("-----------------------------------");    
    //Serial.print("bytesCount:");Serial.println(bytesCount);
    //Serial.print("Buffer:");Serial.println(commandBuffer);          
    //cod
    char * token = strtok(commandBuffer, ","); // not use char ','       
    cod = token==NULL ? 0 : atoi(token);   
    //out1      
    token = strtok(NULL,",");       
    //Serial.print("token: ");Serial.println(token);
    out1 = (token==NULL) ? 0 : atof(token);   
    //out2
    token = strtok(NULL,",");      
    //Serial.print("token: ");Serial.println(token);
    out2 = token==NULL ? 0 : atof(token);
        
}

// Serial Command Table
void printMenu(){
         //START and STOP are only o master.
        
        Serial.print("GO_END_A: ");Serial.println(GO_END_A);
        Serial.print("GO_END_E: ");Serial.println(GO_END_E);
        Serial.print("GO_PARK : ");Serial.println(GO_PARK);
        Serial.print("MOVE_UP : ");Serial.print(MOVE_UP);Serial.println(",value");               
        Serial.print("MOVE_DN : ");Serial.print(MOVE_DN);Serial.println(",value");
        Serial.print("MOVE_LF : ");Serial.print(MOVE_LF);Serial.println(",value");
        Serial.print("MOVE_RG : ");Serial.print(MOVE_RG);Serial.println(",value");
        Serial.print("UPD_SUN_POSITION: ");Serial.print(UPD_SUN_POSITION);Serial.println(",value,value");
        Serial.print("Slave Led On    : ");Serial.println(LED_ON);
        Serial.print("Slave Led Off   : "); Serial.println(LED_OFF);
        
        Serial.print("SET_MODE_MIRROR : ");Serial.println(SET_MODE_MIRROR);
        Serial.print("SET_MODE_TRACKER: ");Serial.println(SET_MODE_TRACKER);        
        
        Serial.print("SET_PARK        : "); Serial.print(SET_PARK);Serial.println(",value,value");
        Serial.print("SET_SHIFT       : "),Serial.print(SET_SHIFT);Serial.println(",value,value");
        Serial.print("SET_TARGET      : ");Serial.print(SET_TARGET);Serial.println(",value,value");
        
      
        Serial.println("50: Print System Position");
        Serial.println("51: Print Vars");
        Serial.println("52: Print last received command");
        Serial.println("53: Print GPS");        
        Serial.println("55: Test Motors");
        Serial.println("60: Test Mirror Orientation");
        Serial.println("99: Print Menu"); 
  
  }

//-----------------------------------------------
// Call commands from Serial port
//-----------------------------------------------
void SerialCommand(int cod, float v1, float v2  ) {    

     

    Serial.print("Slave Serial command-----------------------:");
    Serial.println(cod);

   

    switch (cod)
    {
        //START and STOP are only o master.
            

        case GO_END_A:
        Serial.println("GO_END_A"); tracker.goAzimuthEndstop();
        break;

        case GO_END_E:
        Serial.println("GO_END_E"); tracker.goElevationEndstop();
        break;
        
        case GO_PARK:
        Serial.println("GO_PARK");tracker.goParking();
        break;
        
        case MOVE_UP: 
        Serial.print("MOVE_UP by:"),Serial.println(v1) ;tracker.moveUP(v1);
        break;

        case MOVE_DN:        
        Serial.print("MOVE_DN by:");Serial.println(v1);tracker.moveDN(v1);
        break;

        case MOVE_LF:         
        Serial.print("MOVE_LF by:");Serial.println(v1);tracker.moveLF(v1);
        break;

        case MOVE_RG:
        Serial.print("MOVE_RG by:");Serial.println(v1);tracker.moveRG(v1);        
        break;



        case UPD_SUN_POSITION:
        Serial.print("UPD_SUN_POSITION to:");Serial.println(v1),Serial.println(v2);tracker.updSunPosition(v1,v2);//Azimut,Elevation
        break;

        case LED_ON:
        Serial.println("LED_ON");   digitalWrite(ledPin,HIGH);   
        break; 
        
        case LED_OFF:
        Serial.println("LED_OFF");   digitalWrite(ledPin,LOW);   
        break; 

        case SET_MODE_MIRROR:
        Serial.println("SET_MODE_MIRROR");tracker.setModeMirror();
        break;  

        case SET_MODE_TRACKER:
        Serial.println("SET_MODE_TRACKER");tracker.setModeTracker();
        break;  

        case SET_PARK:
        Serial.println("SET_PARK to:");Serial.println(v1),Serial.println(v2);tracker.setParking(v1,v2);
        break;  
        
        case SET_SHIFT:
        Serial.println("SET_SHIFT  to:");Serial.println(v1),Serial.println(v2);tracker.setShift(v1,v2);
        break;  
        
        case SET_TARGET:
        Serial.println("SET_TARGET to:");Serial.println(v1),Serial.println(v2);tracker.setTarget(v1,v2);//Azimut,Elevation
        break;  

        case 50:
        Serial.println("Print System Position"); tracker.printSystemPosition();
        break;
        case 51:
        Serial.println("Print Vars"); tracker.printVars();
        break;
        case 52:
        Serial.println("Print Last received command");printLastCommand();        
        break;
        
        case 53:
        Serial.println("Print GPS"); printDateTime();
        break;

        case 55:
        Serial.println("Test Motors"); tracker.testMotors();
        break;

        case 60:
        Serial.println("Test Mirror Orientation"); tracker.test_mirror_orientation() ;
        break;

        

        case 99:
        printMenu();
        break; 

      
    }

}



//-----------------------------------------------
// I2c
//-----------------------------------------------

// Cuidado aca no se puede hacer Serial.print u otras cosas
// Warning here cant do anything that consume time
// Called by interrupt service routine when incoming data arrives
// Should not: Do serial prints, Use "delay", Do anything lengthy, Do anything that requires interrupts to be active
// or call another function from here
void receiveEvent (int howMany) //(the number of bytes to read from the controller device) ????
{  
    
    Wire.readBytes((byte*)&request, 12);//master always send 12 so always read 12(3*4)
    
    // Do it in two steps  does not work
    //Wire.readBytes((byte*)&I2Ccommand, 4);Wire.readBytes((byte*)&request, 8);// does not work
    
    newCmd=true;
    I2Ccode=request.code; // remember command in global variable for requestEvent()       
   
    //if (I2Ccode<100)I2C_Command();//only for setValue, sendCmd
    //if (request.code<100) I2C_Command(); // if code>=100 is a GET
      
} 

void  I2C_Command(){
     //  I2Ccode between 1 and 100
     /*
    Serial.print("Slave I2C_Command, code:");   
    Serial.print(I2Ccode);  
    Serial.print(", v1:");Serial.print(request.v1);
    Serial.print(", v2:");Serial.println(request.v2);
   */
    switch (I2Ccode){  //work with request.code, or I2Ccommand

        case MOVE_UP: 
        Serial.println("MOVE_UP");
        tracker.moveUP(request.v1);     
        break;

        case MOVE_DN:
        Serial.println("MOVE_DN");tracker.moveDN(request.v1);
        break;

        case MOVE_LF: 
        Serial.println("MOVE_LF");tracker.moveLF(request.v1);      
        break;

        case MOVE_RG:
        Serial.println("MOVE_RG");tracker.moveRG(request.v1);
        break;

        case GO_PARK:
        Serial.println("GO_PARK");tracker.goParking();
        break;
      
        case GO_END_A:
        Serial.println("GO_END_A"); tracker.goAzimuthEndstop();
        break;

        case GO_END_E:
        Serial.println("GO_END_E"); tracker.goElevationEndstop();
        break;

        case UPD_SUN_POSITION:
        Serial.println("UPD_SUN_POSITION");tracker.updSunPosition(request.v1,request.v2);
        break;

        case LED_ON:
        Serial.println("LED_ON");
        digitalWrite(ledPin,HIGH);   
        break; 
        
        case LED_OFF:
        Serial.println("LED_OFF"); 
        digitalWrite(ledPin,LOW);   
        break; 

       
     }
}
//---------------------------------------------
// I2C Responses
//---------------------------------------------
//mirror,shift,pos =[float,float],2x4=8 chars
//date =[long,long,long],3x4=12 chars, en Mega
//time =[long,long,long],3x4=12 chars, en Mega
//endstop = [long,long],2x4=8 chars, en Mega
// 
// You can use a struct to send multiple things at once. 


void send2L(long v1, long v2){

 struct {long v1; long v2;} response = {v1,v2};
 Wire.write ((byte *) &response, 8);//4+4
 
}

void send3L(long v1,long v2,long v3)
{
 struct {long v1; long v2; long v3;} response = {v1,v2,v3};
 Wire.write ((byte *) &response, 12);//4+4+4
}

void send2F(float v1, float v2){  
 struct {float v1; float v2;} response = {v1,v2};
 Wire.write ((byte *) &response, 8);//4+4
}

//ok
void send3F(float v1, float v2, float v3){  
 struct {float v1; float v2;float v3;} response = {v1,v2,v3};
 Wire.write ((byte *) &response, 12);//4+4+4
}

// Test request with dummy data
// Wire.write(value) Wire.write(string) Wire.write(data, length)
//https://www.arduino.cc/reference/en/language/functions/communication/wire/write/

void requestEvent00()
{
  
   struct {long v1;long v2;long v3;} response = {101,202,303};
   //struct {long v1;long v2;} response = {101,202};
   //struct {float v1;float v2;} response = {10.1,20.2};
   Wire.write((byte *) &response, sizeof(response));
   //Wire.write(125) ;
   //Wire.write("Hello NodeMCU");
}

// I2C requestEvent
// Called from master with: requestFrom(device,responseSize)
// I2Ccode > 100
void requestEvent() 
{   
  
  //if (I2Ccode<100){ I2C_Command();  send2L(1,1); return;  }
  
  switch (I2Ccode) // With request.code doesn't work
     {
      
     case I2C_ROUND_TRIP: //Test I2C, re sending request values          
          send2F(request.v1+3.141516,request.v2+3.141516);
     break;
     //--------------------------
     // Setters
     //--------------------------
     case SET_PARK:
          //Serial.println("SET_PARK");
          tracker.setParking(request.v1,request.v2);
          send2L(1,SET_PARK);
     break;  
        
     case SET_SHIFT:
          //Serial.println("SET_SHIFT");
          tracker.setShift(request.v1,request.v2);
          send2L(1,SET_SHIFT);
     break;  
        
     case SET_TARGET:
          //Serial.println("SET_TARGET");
          tracker.setTarget(request.v1,request.v2);
          send2L(1,SET_TARGET);
     break;  

     case SET_MODE_MIRROR:
          //Serial.println("SET_MODE_MIRROR");
          tracker.setModeMirror();
          digitalWrite(ledPin,LOW);  
          send2L(1,SET_MODE_MIRROR);
     break;  

     case SET_MODE_TRACKER:
          //Serial.println("SET_MODE_TRACKER");
          tracker.setModeTracker();
          digitalWrite(ledPin,HIGH);  
          send2L(1,SET_MODE_TRACKER);
     break;  
     
     //--------------------------
     // Getters
     //--------------------------

     case GET_GPS_STATUS:   
          {   
          //send3L((long)gps.location.isValid(),(long)gps.date.isValid(),(long)gps.time.isValid());
          //send2L(gps.location.isValid(),(gps.date.isValid() && gps.time.isValid()));
          int isvalid = (gps.location.isValid() && gps.date.isValid() && gps.time.isValid());
          send3L(isvalid,atoi(satsInView.value()), gps.satellites.value());//isValid,inView,inUse
          
          //send3L(gps.location.isValid(),(gps.date.isValid() && gps.time.isValid()),gps.satellites.value());//satellites in use
          //send2L((long)199,(long)199);         
          break;
          }
     
     case GET_DATE:     
          send3L(gps.date.year(),gps.date.month(),gps.date.day());//Y,M,D  //Master receive ok                
          //Wire.write ((byte *) &responseDate, 12);// send from buffer to avoid time consuming
          break;
     case GET_TIME:  
          send3L(gps.time.hour(),gps.time.minute(),gps.time.second());//Y,M,D   
          //Wire.write ((byte *) &responseTime, 12);// send from buffer to avoid time consuming
          break;
     
     case GET_LOCATION:     
          send3F(gps.location.lng(),gps.location.lat(),gps.altitude.meters());
          //send3F(11.22,33.44,8845.50);    
          break;

     case GET_END_STOPS:
          send2L(digitalRead(A_ENDSTOP_PIN),digitalRead(E_ENDSTOP_PIN));
          break;
     case GET_END_STOPS_STEPS:                
          send2L(99,99);// TODO
          break;

     case GET_SYSTEM_POSITION: 
          //send2F(11.11,22.22);
          send2F(tracker.getMirrorAzimuth(),tracker.getMirrorElevation());
          break;

     case GET_SHIFT:
          //send2F(33.33,44.44);//
          send2F(tracker.getShiftAzimuth(),tracker.getShiftElevation());
          break; 

     case GET_TARGET:
          //send2F(55.55,66.66);//
          send2F(tracker.getTargetAzimuth(),tracker.getTargetElevation());
          break;   
          
     case GET_PARK:
          //send2F(55.55,66.66);//
          send2F(tracker.getParkAzimuth(),tracker.getParkElevation());
          break;   

     case GET_MODE:
          //send2L(1,1);//         
          send2L(tracker.getMode(),tracker.getMode());
          break;  
}
 
}

void printLastCommand()
{
    Serial.print("Slave I2C_Command, code:");   
    Serial.println(I2Ccode);  
    Serial.print(" Request:");  
    Serial.print(", code:");Serial.print(request.code);
    Serial.print(", v1:");Serial.print(request.v1);
    Serial.print(", v2:");Serial.println(request.v2);

}

//============

//-----------------------------------------------
// GPS functions
//-----------------------------------------------



// This custom version of delay() ensures that the gps object
// is being "fed".
/*
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (Serial2.available())
      gps.encode(Serial2.read());
  } while (millis() - start < ms);
}
*/


void printDateTime() 
{
  // http://arduiniana.org/libraries/tinygpsplus/
 
  //Serial.print(" printDateTime:");



  /*Serial.print(gps.altitude.meters()); 

  Serial.print(" n sat:");Serial.print(gps.satellites.value());*/
  
  Serial.print(" Sats in view: ");
  Serial.println(satsInView.value());
 
  Serial.print(" Sats in use : ");
  Serial.println(gps.satellites.value()); // Number of satellites in use 

  //Serial.print(gps.libraryVersion());
  //write buffer
  char msg2[100]={0};  
    
  //gps.date.isValid(),gps.location.isValid()gps.time.age()
  //,(int)gps.time.age(),(int)gps.time.isValid(),(int)gps.date.isValid(),(int)gps.location.isValid());
  
  
  sprintf(msg2," age: %4d, time is valid :%d, date is valid:%d, location is valid: %d ",(int)gps.time.age(),(int)gps.time.isValid(),(int)gps.date.isValid(),gps.location.isValid());   
  
  Serial.println(msg2);
 
    
  if (gps.time.isValid()) //check whether gps date is valid
      {
        char msg[50]={0}; 
        int year = gps.date.year();        
        int month = gps.date.month();
        int day = gps.date.day();
               
        int hour = gps.time.hour();
        int minute = gps.time.minute();
        int second = gps.time.second();
        
        sprintf(msg,"\r\n Date: %d-%02d-%02d, Time: %02d:%02d:%02d utc London \r\n",year, month, day, hour, minute, second);
        Serial.print(msg);
      }  
   
 
}

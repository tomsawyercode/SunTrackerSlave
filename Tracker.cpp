

#include "Arduino.h"
#include "configurations.h"
#include "pins.h"
#include "Tracker.h"



//#include "AccelStepper.h"


Tracker::Tracker():
   _shiftAzimuth(0),_shiftElevation(0),
   _targetAzimuth(0),_targetElevation(0),
   _parkingAzimuth(0),_parkingElevation(0),  
   _maxAngleA(0),_maxAngleE(0),
   _mode(Modes::tracker)

  { 
  //-------------------------------------------
  // Enable Motors
  //-------------------------------------------
  pinMode(A_ENABLE_PIN, OUTPUT); 
  digitalWrite(A_ENABLE_PIN, LOW);

  pinMode(E_ENABLE_PIN, OUTPUT); 
  digitalWrite(E_ENABLE_PIN, LOW);

  stepperA = AccelStepper(AccelStepper::DRIVER, A_STEP_PIN, A_DIR_PIN);
  stepperE = AccelStepper(AccelStepper::DRIVER, E_STEP_PIN, E_DIR_PIN);

  stepperA.setMinPulseWidth(100);

  stepperA.setMaxSpeed(MAX_SPEED);
  stepperE.setMaxSpeed(MAX_SPEED);

  //-------------------------------------------
  // Set EndStops pins
  //-------------------------------------------
  pinMode(A_ENDSTOP_PIN, INPUT);
  pinMode(E_ENDSTOP_PIN, INPUT);
  
  //-------------------------------------------

    
}

void  Tracker::setModeMirror(){
  _mode = Modes::mirror;

}
void  Tracker::setModeTracker(){
  _mode = Modes::tracker;
}

void Tracker::setShift(double A, double E){

   _shiftAzimuth=A;
   _shiftElevation=E;
}
void Tracker::setParking(double A, double E){

   _parkingAzimuth=A;
   _parkingElevation=E;
}  
  
void Tracker::setTarget(double A,double E){
  _targetAzimuth=A;
  _targetElevation=E;
}

//System or Sun ??

//This code is WRONG, 
void Tracker:: updSunPosition(double sunAzimuth,double sunElevation){
 
  Serial.print("Upd Sun Position to---------- A:");
  Serial.print(sunAzimuth);
  Serial.print(", E:");
  Serial.println(sunElevation);

  if (_mode == Modes::mirror)
  {
    Serial.println(" mode: mirror");
    double out_a=0;
    double out_e=0;
    
    mirror_orientation(sunAzimuth,sunElevation,_targetAzimuth,_targetElevation,out_a,out_e);
    goAzimuth(out_a);
    goElevation(out_e);
  }
  else // tracker
  {
    Serial.println(" mode: tracker");
    goAzimuth(sunAzimuth);
    goElevation(sunElevation);
  }
}


bool Tracker::goAzimuth(double newAzimuthAngle)
{
     Serial.print("goAzimuth:------------:");
     Serial.println(newAzimuthAngle);

     //Calculate newSystemAngle from end stop
     double newSystemAngle =  newAzimuthAngle -_shiftAzimuth;//100-(-10)=100+10=110
     
     /*Serial.print(" newSystemAngle:");
     Serial.println(newSystemAngle);
     Serial.print(" currentPosition:");
     Serial.println(stepperA.currentPosition());
     */

     // Before move CHECK limits min and max    
     if (newSystemAngle < 0) return false; 
     if (newSystemAngle >A_MAX_ANGLE) return false;

     
     // calculate direction
     int steps =  newSystemAngle/A_RES - stepperA.currentPosition(); // (100-0) positive movement, 90-100 negative movement
     //Serial.print(" relative steps:");
     //Serial.println(steps);
     // calculate position
     int newPosition = newSystemAngle/A_RES;
     //Serial.print(" newPosition steps:");
     //Serial.println(newPosition);  
    
        

      if (steps>0) {
            stepperA.setSpeed(TRACK_SPEED);
            while(stepperA.currentPosition()< newPosition) 
                {
                  stepperA.runSpeed();         
                } 
            
          }
      else
          {
            
            stepperA.setSpeed(-TRACK_SPEED);
            while(stepperA.currentPosition()> newPosition)
                {
                  stepperA.runSpeed();         
                } 
            
          }
    
     //stepperA.runToNewPosition(newSystemAngle/A_RES);
     //stepperA.move(steps);//relative positive or negative
     //stepperA.moveTo(newSystemAngle/A_RES);//absolute
     //stepperA.setSpeed(TRAVEL_SPEED);
     //stepperA.runToPosition();
     //stepperA.runSpeedToPosition();
     //https://arduino.stackexchange.com/questions/61218/accelstepper-runtoposition-is-working-fine-accelstepper-run-does-nothing

     //Serial.print(" final position (steps):");
     //Serial.println(stepperA.currentPosition());
     
     Serial.print(" final position (angle):");     
     Serial.println(stepperA.currentPosition()*A_RES);
     
     return true;
     

}


bool Tracker::goElevation(double newElevationAngle)
{

     Serial.print("goElevation:------------: ");
     Serial.println(newElevationAngle);
  
     
     double newSystemAngle =  newElevationAngle -_shiftElevation;//90-(-15) = 90+15
      /*   
     Serial.print(" newSystemAngle:");
     Serial.println(newSystemAngle);
     Serial.print(" currentPosition :");
     Serial.println(stepperE.currentPosition());
*/
     // Before move CHECK limits min and max    
     if (newSystemAngle < 0) return false; 
     if (newSystemAngle >E_MAX_ANGLE) return false;

    
     
   // calculate direction
     int steps =  newSystemAngle/E_RES - stepperE.currentPosition(); // (100-0) positive movement, 90-100 negative movement
     //Serial.print(" relative steps:");
     //Serial.println(steps);
     // calculate position
     int newPosition = newSystemAngle/E_RES;
     //Serial.print(" newPosition steps:");
    // Serial.println(newPosition);  

        

      if (steps>0) {
            stepperE.setSpeed(TRACK_SPEED);
            while(stepperE.currentPosition()< newPosition) 
                {
                  stepperE.runSpeed();         
                } 
            
          }
      else
          {
            
            stepperE.setSpeed(-TRACK_SPEED);
            while(stepperE.currentPosition()> newPosition) 
                {
                  stepperE.runSpeed();         
                } 
            
          }          

        
           //Serial.print(" final position (steps):");
           //Serial.println(stepperE.currentPosition());
           //Serial.println(posA+stepsA);   
           Serial.print(" final position (angle):");           
           Serial.println(stepperE.currentPosition()*E_RES);
      
       return true;

}




// Move by n degrees
bool Tracker::moveUP(double degrees){
      double current =stepperE.currentPosition()*E_RES;//(100steps) * (0.5 e_RES) = (50 degrees)
            
      Serial.println("Move UP");
      Serial.print(" current position (angle):");           
      Serial.println(stepperE.currentPosition()*E_RES);
      Serial.print(" final position (angle):");
      Serial.println(current+degrees);
      
      return goElevation(current+degrees);
}
bool Tracker::moveDN(double degrees){
      double current =stepperE.currentPosition()*E_RES;
      return goElevation(current-degrees);
}
bool Tracker::moveLF(double degrees){
      double current =stepperA.currentPosition()*A_RES;
      return goAzimuth(current+degrees);
}
bool Tracker::moveRG(double degrees){
      double current =stepperA.currentPosition()*A_RES;
      return goAzimuth(current-degrees);
}

//  #goTest
void Tracker::testMotors() //
{
     //return 44;
 
    //moveTo(_currentPos + relative);
    //gotoTime(_lat,_lon,_altitude,_year,_month,_day,12,0,0)

     
     //int steps=0;
     int moveSteps = 90/A_RES;// (90degress)/(0.9 A_RES)= 100 steps
     
     Serial.print(" steps:");
     Serial.println(moveSteps);    
     
     stepperA.setCurrentPosition(0);
     stepperA.setSpeed(TRAVEL_SPEED);
     
     Serial.print("A currentPosition:");
     Serial.println(stepperA.currentPosition()); 

         
    int moved=0;
    while (stepperA.currentPosition() < moveSteps )    
    {
      
     stepperA.runSpeed() ;           
     moved++;
     
     Serial.print(".");
    
           
    }
    Serial.print(" moved:");
    Serial.println(moved);
    
    Serial.print("A currentPosition:");
    Serial.println(stepperA.currentPosition());

    
   // Serial.println(steps); 
    
    
    /*
    
    stepperE.setCurrentPosition(0);
    stepperE.setSpeed(TRAVEL_SPEED);
    
    Serial.print("E currentPosition:"); 
    Serial.println(stepperE.currentPosition()); 
    
     
    while (stepperE.currentPosition() != moveSteps ) //&& digitalRead(A_ENDSTOP_PIN)!== END_STOP_ACTIVATED)
    {
      stepperE.runSpeed();
    }
    //_elevationSteps+=moveSteps;
    Serial.print("E currentPosition:");
    Serial.println(stepperE.currentPosition());*/
}




//EndStop for : 0 Azimuth, 1 Elevation, 2 Both

// #goAzimuthEndstop
bool Tracker::goAzimuthEndstop()
{
  //_azimuthSteps=0;
   //assume lack of syncronization with real position due to steps count lost.
  stepperA.setCurrentPosition(0);
 
  // if all ready endstop is activated : move until release
  
  if (digitalRead(A_ENDSTOP_PIN)==END_STOP_ACTIVATED)
  { 
    Serial.println("Azimuth EndStop is: Closed");
    stepperA.setSpeed(TRAVEL_SPEED);
    while (digitalRead(A_ENDSTOP_PIN)== END_STOP_ACTIVATED)
    {
      stepperA.runSpeed();
    }  
  }
  
  //else  // All ready endstop is not activated : Move until reach endstop
  //{
    Serial.println("Azimuth EndStop is: Open");
    stepperA.setSpeed(-TRAVEL_SPEED);
    // Make the Stepper move back until the switch is activated 
    // Move until EndStop trigger   
    int maxSteps = (int)(A_MAX_ANGLE/A_RES);
    while (digitalRead(A_ENDSTOP_PIN)== END_STOP_DEACTIVATED) {
      
      stepperA.runSpeed();
     /*
      if( abs(stepperA.currentPosition()) > maxSteps){
        Serial.println("Unreacheable Azimuth EndStop:");                 
        return false;      
        }  */  
     
      //Serial.println(steps);
     }       
  //}
 
 Serial.print("A_ENDSTOP is closed:");
 Serial.println(digitalRead(A_ENDSTOP_PIN)== END_STOP_ACTIVATED);
 Serial.print(" moved steps:");
 Serial.println(stepperA.currentPosition());  
 stepperA.setCurrentPosition(0);
 stepperA.setSpeed(TRAVEL_SPEED);
 return true;
}

// #goElevationEndstop
bool Tracker::goElevationEndstop()
{
 
   stepperE.setCurrentPosition(0);
  
  // If all ready endstop is activated : move until release
  
  if (digitalRead(E_ENDSTOP_PIN)==END_STOP_ACTIVATED)
  { 
    Serial.println("Elevation EndStop is: Closed");
    stepperE.setSpeed(TRAVEL_SPEED);
    while (digitalRead(E_ENDSTOP_PIN)== END_STOP_ACTIVATED)
    {
      stepperE.runSpeed();
    }
     
    
    
  }
  // else  // All ready endstop is not activated : Move until reach endstop
  //{
    Serial.println("Elevation EndStop is: Open then move until close it");
    
    stepperE.setSpeed(-TRAVEL_SPEED);//go backward
    // Make the Stepper move back until the switch is activated 
    // Move until EndStop trigger
    
    int maxSteps = (int)(E_MAX_ANGLE/E_RES);
    while(digitalRead(E_ENDSTOP_PIN)== END_STOP_DEACTIVATED)
     {   
      stepperE.runSpeed();        
     /*
        if(abs(stepperE.currentPosition()) > maxSteps){
          Serial.print("Unreacheable Azimuth EndStop:");                 
          return false;      
        }    */
     }
    
  //}
  
   Serial.print("E_ENDSTOP is closed:");
   Serial.println(digitalRead(E_ENDSTOP_PIN)== END_STOP_ACTIVATED);
   Serial.print(" moved steps:");
   Serial.println(stepperE.currentPosition());
   stepperE.setCurrentPosition(0);
   return true;
}


void Tracker:: goParking(){

  Serial.print("goParking:");   
    goAzimuth(_parkingAzimuth);
    goElevation(_parkingElevation);
  
}

void Tracker::printSystemPosition() 
{  
  Serial.print(" Position A steps:");
  Serial.print(stepperA.currentPosition());
  Serial.print(", angle:");
  Serial.println(stepperA.currentPosition()/A_RES);


  Serial.print(" Position E steps:");
  Serial.print(stepperE.currentPosition());
  Serial.print(", angle:");
  Serial.println(stepperE.currentPosition()/E_RES);

}

char * Tracker::getModeStr()
{
   if(_mode== Modes::mirror) return "mirror"; 
   if(_mode== Modes::tracker) return "tracker";   
   
}

void Tracker::printVars() 
{  
  printEndstops();
  Serial.print(" Mode: "); Serial.println(getModeStr());  

  Serial.print(" Target azimuth   : "); Serial.println(_targetAzimuth ); 
  Serial.print(" Target elevation : "); Serial.println(_targetElevation );  

  Serial.print(" Shift azimuth   : "); Serial.println(_shiftAzimuth ); 
  Serial.print(" Shift elevation : "); Serial.println(_shiftElevation ); 

  Serial.print(" Parking azimuth   : "); Serial.println(_parkingAzimuth ); 
  Serial.print(" Parking elevation : "); Serial.println(_parkingElevation ); 
    
  // Serial.print("Track speed: ");Serial.println(TRACK_SPEED);
  // Serial.print("Travel speed: ");Serial.println(TRAVEL_SPEED);
  

}  

void Tracker::printEndstops()
{
  Serial.print(" Endstop azimuth  :");
  Serial.println(digitalRead(A_ENDSTOP_PIN));
  Serial.print(" Endstop elevation:");
  Serial.println(digitalRead(E_ENDSTOP_PIN));
  
}

void Tracker::vector_from_a_e(double a,double e,double & vx,double & vy,double & vz){ 

    //azimuth (gamma)  γ
    double gamma = a * 3.141592653/180 ; 
    // elevation (alpha)  α 
    double alpha = e * 3.141592653/180;
            
    vx= cos(gamma)* cos(alpha);
    vy= sin(gamma)* cos(alpha);
    vz= sin(alpha);
}
void Tracker::mirror_orientation(double sun_a,double sun_e,double target_a,double target_e,double & out_a, double & out_e)
{
    
    //Serial.println("Mirror_orientation------------------------------------");
    //Serial.println("Vectors:");
    double sx=0,sy=0,sz=0;
    vector_from_a_e(sun_a,sun_e,sx,sy,sz);
    
    Serial.print("Sun    a: ");Serial.print(sun_a,2);Serial.print(", e:");Serial.println(sun_e,2);
    //Serial.print("Sun vec: ");Serial.print(sx,4);Serial.print(", ");Serial.print(sy,4);Serial.print(", ");Serial.println(sz,4);
    
    double tx=0,ty=0,tz=0;
    vector_from_a_e(target_a,target_e,tx,ty,tz);
    
    Serial.print("Target a: ");Serial.print(target_a,2);Serial.print(", e:");Serial.println(target_e,2);
    //Serial.print("Target vec: ");Serial.print(tx,4);Serial.print(", ");Serial.print(ty,4);Serial.print(", ");Serial.println(tz,4);
    
    double divisor = sqrt( pow(sx+tx,2)+pow(sy+ty,2)+pow(sz+tz,2));
    //Serial.print("Module: ");Serial.println(divisor,4);
    
    double mx = (sx+tx)/divisor;
    double my = (sy+ty)/divisor;
    double mz = (sz+tz)/divisor; 
    
    out_a = atan2(my,mx) * 180 / 3.141592653;
    out_e = asin(mz)  * 180 / 3.141592653;  
 
    Serial.print("Mirror a: ");Serial.print(out_a,2);Serial.print(", e: ");Serial.println(out_e,2);
   // Serial.print("Mirror vec: ");Serial.print(mx,4);Serial.print(", ");Serial.print(my,4);Serial.print(", ");Serial.println(mz,4);
}

void Tracker::test_mirror_orientation(){

  double out_a=0;
  double out_e=0;

  Serial.println("Test 1:------------------------------------"); 
  mirror_orientation(0,45,0,0,out_a, out_e);
  Serial.println("Test 2:------------------------------------"); 
  mirror_orientation(0,0,90,0,out_a, out_e);
  Serial.println("Test 3:------------------------------------"); 
  mirror_orientation(45,0,135,0,out_a, out_e);
  Serial.println("Test 4:------------------------------------"); 
  mirror_orientation(50,0,150,0,out_a, out_e);

}

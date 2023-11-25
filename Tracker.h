

//void SaveParamsToEEPROM(bool bLoad);

#include "AccelStepper.h"

#include "pins.h"

enum class Modes : int {mirror, tracker};

class Tracker
{
public:

Tracker();



void setShift(double ,double);
void setParking(double ,double);
void setTarget(double ,double);
void setModeMirror();
void setModeTracker();


double getParkAzimuth(){return _parkingAzimuth;}
double getParkElevation(){return _parkingElevation;}
double getShiftAzimuth(){return _shiftAzimuth;}
double getShiftElevation(){return _shiftElevation;}
double getTargetAzimuth(){return _targetAzimuth;};
double getTargetElevation(){return _targetElevation;};

int  getMode(){return (int)_mode;};
char getModeChar(){return ( _mode == Modes::tracker) ? 'T' : 'M';};
char * getModeStr();

// Get from ZERO without shifts
double getMirrorAzimuth(){return stepperA.currentPosition()*A_RES + _shiftAzimuth;};
double getMirrorElevation(){return stepperE.currentPosition()*E_RES + _shiftElevation;};

void testMotors();


//void goAzimuthElevation(double,double);
void updSunPosition(double,double);
void vector_from_a_e(double a,double e,double & vx,double & vy,double & vz);
void mirror_orientation(double sun_a,double sun_e,double target_a,double target_e,double & out_a, double & out_e);
void test_mirror_orientation();

bool goAzimuth(double );
bool goElevation(double );
void goParking();

bool moveUP(double);// move by n degrees
bool moveDN(double);// move by n degrees
bool moveLF(double);// move by n degrees
bool moveRG(double);// move by n degrees



bool goAzimuthEndstop();
bool goElevationEndstop();
int getAzimuthEndstop(){return digitalRead(A_ENDSTOP_PIN);};
int gettElevationEndstop(){return digitalRead(E_ENDSTOP_PIN);};

// Steep loose
// int getEndstopSteps();
// int getEndstopSteps();


void printEndstops();
void printSystemPosition();
void printVars();


private:
  AccelStepper stepperA,stepperE; 
  Modes _mode;

  double _shiftAzimuth,_shiftElevation; //End Stop shift from Equator - Nadir  
  double _targetAzimuth,_targetElevation;  
  double _parkingAzimuth,_parkingElevation; 
  double _maxAngleA,_maxAngleE;



};

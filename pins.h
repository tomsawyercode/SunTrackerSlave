#ifndef __PINS_H__
#define __PINS_H__

// Mechanical Resolution is in configurations.h
// To turn motor direction just reverse one motor coil.


//---------------------------------
// Position Zero Switches - EndStop
//---------------------------------

#define A_ENDSTOP_PIN 3  // X_MIN_PIN   3
#define E_ENDSTOP_PIN 14  // Y_MIN_PIN 14
//---------------------------------

// EndStops Logic
//---------------------------------
// Optical EndStop
#define END_STOP_ACTIVATED HIGH
#define END_STOP_DEACTIVATED LOW

//Mecanical EndStop
//#define END_STOP_ACTIVATED LOW
//#define END_STOP_DEACTIVATED HIGH 

//-----------------------------------------------
// Ramps pins
//-----------------------------------------------

// Ramps X Axis  motor
#define X_STEP_PIN         54
#define X_DIR_PIN          55
#define X_ENABLE_PIN       38

// Ramps Y Axis  motor

#define Y_STEP_PIN         60
#define Y_DIR_PIN          61
#define Y_ENABLE_PIN       56

// Ramps Z Axis  motor

//#define Z_STEP_PIN         46
//#define Z_DIR_PIN          48
//#define Z_ENABLE_PIN       62
//-----------------------------------------------

// Map Azimuth to X axis of Ramps 1.4

#define A_STEP_PIN         X_STEP_PIN
#define A_DIR_PIN          X_DIR_PIN
#define A_ENABLE_PIN       X_ENABLE_PIN


// Map Elevation to Y axis of Ramps 1.4

#define E_STEP_PIN         Y_STEP_PIN
#define E_DIR_PIN          Y_DIR_PIN
#define E_ENABLE_PIN       Y_ENABLE_PIN

//-----------------------------------------------

#endif // __PINS_H__

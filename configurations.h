#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

// TODO Save this three in EEPROM 

#define PARKING_AZIMUTH  90
#define PARKING_ELEVATION  0 

#define SHIFT_AZIMUTH    0
#define SHIFT_ELEVATION  -5

#define TARGET_AZIMUTH   90.00
#define TARGET_ELEVATION 0.0

//Mechanical Resolution

#define A_RES   (0.9/16) // 0.9=360/(200 * 2) Ratio 2:1, 
#define E_RES  (1.8/16) //  1.8=360/(200)

#define A_MAX_ANGLE  180 // mechanical aperture: 0 to x degrees 
#define E_MAX_ANGLE   95 // mechanical aperture: 0 to x degrees

// Speed Settings

#define MAX_SPEED    601 //360 steps/seconds, for1/16
#define TRACK_SPEED  600 //300 steps/seconds, for1/16
#define TRAVEL_SPEED 600 //400 steps/seconds go endstop, for1/16



#endif // __CONFIGURATION_H__

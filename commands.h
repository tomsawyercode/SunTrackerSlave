

enum cmd {
  
  START = 1, //START and STOP are only for master.
  STOP, //START and STOP are only for master.
  GO_END_A,
  GO_END_E,
  GO_PARK ,    
  MOVE_UP,
  MOVE_DN,
  MOVE_LF,
  MOVE_RG, 
  UPD_SUN_POSITION,      
  LED_ON,
  LED_OFF,
  SAVE_EEPRON  //Save changes to EEPROM must be implemented
};

// Set - Get commands start at 100
enum vars{
         
     I2C_ROUND_TRIP = 100, //Test I2C, master send and receive same values 
     SET_PARK,
     SET_SHIFT,
     SET_TARGET,  
     SET_MODE_MIRROR,
     SET_MODE_TRACKER,
     GET_GPS_STATUS,     
     GET_TIME,
     GET_DATE,
     GET_LOCATION,
     GET_END_STOPS,
     GET_END_STOPS_STEPS,
     GET_SYSTEM_POSITION,
     GET_SHIFT,
     GET_TARGET,
     GET_PARK,
     GET_MODE
};

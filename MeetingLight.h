// Definitions for MeetingLight code

#ifndef _MEETINGLIGHT_H_
#define _MEETINGLIGHT_H_

// RFM69
NODE_MEETINGLIGHT_BUTTON = 5   (This device)
NODE_MEETINGLIGHT_DISPLAY = 6  (Remote device)

// RFM69 limits for sendWithRetry()
#define MAX_TIMEOUT   50
#define MAX_RETRIES   5
#define RADIO_DELAY   500

// Serial port code. 
//#undef SERIAL_EN    // comment this out to get debugging info on the Serial line
#ifdef SERIAL_EN
  #define DEBUG(input)   { Serial.print(input); }
  #define DEBUGln(input) { Serial.println(input); }
  #define SERIALFLUSH()  { Serial.flush(); }
#else
  #define DEBUG(input);
  #define DEBUGln(input);
  #define SERIALFLUSH();
#endif

struct xmitStruct
{
  char pktType;
  unsigned short int red, blue, green;
  char symbol;
  unsigned short int row;
  unsigned short int col;
  byte vector[8];
};

#define EEPROM_STARTADDRESS 10	// Starting point for EEPROM info

#endif  // _MEETINGLIGHT_H_
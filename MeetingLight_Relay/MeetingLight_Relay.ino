#include <Wire.h>
#include <MoteDefs.h>

// NODE IDs: 
// NODE_MEETINGLIGHT_BUTTON = 5   (Remote device)
// NODE_MEETINGLIGHT_DISPLAY = 6  (This device)

// Limits for sendWithRetry()
#define MAX_TIMEOUT 50
#define MAX_RETRIES 5
#define RADIO_DELAY 500

#define LED_INTERNAL    9 // Moteinos hsave LEDs on D9
#define FLASH_SS      8 // and FLASH SS on D8
const int button1Pin = 3;  // pushbutton 1 pin

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

// Generic character buffer
char xmitBuff[RF69_MAX_DATA_LEN];
RFM69_ATC radio;

void initRadio()
{
  radio.initialize(FREQUENCY,NODE_MEETINGLIGHT_DISPLAY,NETWORKID);
  radio.setHighPower(); //must include this only for RFM69HW/HCW!
  radio.encrypt(ENCRYPTKEY); //OPTIONAL
  radio.enableAutoPower(ATC_RSSI);
}

struct 
{
  char pktType;
  unsigned short int red, blue, green;
  char symbol;
  unsigned short int row;
  unsigned short int col;
  byte vector[8];
} xmitStruct;

void buttonEvent()
{
  int buffLen, rows, cols, i;
  char tmp[1], t1;

  DEBUGln("Relay - Got button event type.");

  Wire.beginTransmission(2);
  switch(xmitStruct.pktType)
  {
    case '0':   // new sign
      DEBUG("New Sign.");
      break;
    case '1':   // Change color
      DEBUG("Change color.");
      break;
    case '2':   // Place character
      DEBUG("Place character.");
      break;
    case '3':   // Place border
      DEBUG("Place border.")
      break;
    default:
      DEBUG("Invalid packet type.")
      break;
  }

  Wire.write((byte *)&xmitStruct, sizeof(xmitStruct));
  Wire.endTransmission();
  DEBUGln("Packet sent to display.");
}

#undef SERIAL_EN
void setup()
{
#ifdef SERIAL_EN
  Serial.begin(SERIAL_BAUD);
#endif // SERIAL_EN

  DEBUGln("Begin setup()");
  Wire.begin();                // join i2c bus - Master node
  pinMode(button1Pin, INPUT);

  initRadio();

  // Blink the internal LED
  ledBlink(250, 3);
  DEBUGln("End setup()");
}

void loop()
{
  //radioData radioInfo;
  char tmp[100];
  int i;
  
  if (radio.receiveDone())
  {
    memcpy(&xmitStruct, radio.DATA, sizeof(xmitStruct));
    /*
    sprintf(tmp, "Packet: type=%c rgb=%d%d%d symbol=%c row=%d col=%d", 
        xmitStruct.pktType,xmitStruct.red, xmitStruct.green, xmitStruct.blue, 
        xmitStruct.symbol, xmitStruct.row, xmitStruct.col);
    Serial.print(tmp);
    for (i=0; i<8; ++i)
    {
      sprintf(tmp, "%d-", xmitStruct.vector[i]);
      Serial.print(tmp);
    }
    Serial.println("");
    */

    buttonEvent();
  }

  if (radio.ACKRequested())
  {
    radio.sendACK();
    DEBUGln("ACK sent.");
  }
}

void ledBlink(byte delayMs, byte loops)
{
  for (byte i = 0; i < loops; i++)
  {
    digitalWrite(LED_INTERNAL, HIGH);
    delay(delayMs);
    digitalWrite(LED_INTERNAL, LOW);
    delay(delayMs);
  }
}

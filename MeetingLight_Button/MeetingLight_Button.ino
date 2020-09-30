#include <MoteDefs.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// NODE IDs: 
// NODE_MEETINGLIGHT_BUTTON = 5   (This device)
// NODE_MEETINGLIGHT_DISPLAY = 6  (Remote device)

// Limits for sendWithRetry()
#define MAX_TIMEOUT   50
#define MAX_RETRIES   5
#define RADIO_DELAY   500

// Button Info
const int button1pin = 16;

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

// More radio stuff
// Generic character buffer
char xmitBuff[RF69_MAX_DATA_LEN];  // Generic xmit/recv buffer
RFM69_ATC radio;                        // RFM 69 RF Radio Manager

#define NUMSIGNS 4
char *colorList[NUMSIGNS][3] = {{0,0,0}, 
                                {0,7,0}, 
                                {7,7,0}, 
                                {7,0,0}}; // Black, Green, Yellow, Red
char *signs[NUMSIGNS]= {" ", 
                        //"OK TO+ENTER", 
                        "COME+ON IN",
                        "AUDIO+CALL.", 
                        "VIDEO+CALL."};  // '+'=\n  '.'=(space)

char *oledSigns[NUMSIGNS]= {" ", 
                        "ENTER", 
                        "AUDIO", 
                        "VIDEO"};
int currSignIndex=0;
int currentRow=0, currentColumn= 0;
int numChars;   // Number of characters possible. Read in from EEPROM.
int dataPoints;   // Number of data points per character
const unsigned int EepromStartAddress = 10;

struct 
{
  char pktType;
  unsigned short int red, blue, green;
  char symbol;
  unsigned short int row;
  unsigned short int col;
  byte vector[8];
} xmitStruct;

void initRadio()
{
  Serial.println("Initializing radio");

  // Initialize radio
  radio.initialize(FREQUENCY, NODE_MEETINGLIGHT_BUTTON, NETWORKID);
  radio.setHighPower(); //must include this only for RFM69HW/HCW!
  radio.encrypt(ENCRYPTKEY); //OPTIONAL
  radio.enableAutoPower(ATC_RSSI);
}

int findIniters(char cWanted)
{
  int i, startAddress;
  char c;

  startAddress = EepromStartAddress + 2;  // Skip over header bytes
  for (i=0; i<numChars; ++i)
  {
    c = EEPROM.read(startAddress + (i * (dataPoints+1)));
    if (c == cWanted)
       return i;
  }
}

void oledDisplayText(char *text)
{
  char c, *p = text;
  
  DEBUG("OLED: ["); DEBUG(text); DEBUGln("]");
  display.clearDisplay();

  display.setTextSize(4);
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner

  while (*p != '\0')
  {
      c = (*p == '+'?' ':(*p == '.'?' ':*p));
      DEBUG(c); DEBUG("-");
      display.write(c);
      ++p;
  }
  DEBUGln("");
  display.display();
  return;
}

void setup() {
  
#ifdef SERIAL_EN
  Serial.begin(SERIAL_BAUD);
#endif // SERIAL_EN

  DEBUGln("Begin setup()");

  numChars = EEPROM.read(EepromStartAddress);    // # of characters available to signs
  dataPoints = EEPROM.read(EepromStartAddress + 1);    // # of characters available to signs

  pinMode(button1pin, INPUT);

  DEBUGln("Initializing radio.");
  initRadio();

  DEBUGln("Initializing Display.");
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  display.clearDisplay();
  display.display();
  oledDisplayText("Ready!");
  delay(250);

  // Ensure we start with a blank screen on the display
  currSignIndex = NUMSIGNS-1;
  buttonEvent();

  DEBUGln("End setup()");
}

// Send type 0 packet (start new sign)
void sendNewSign()
{
  DEBUGln("Sending new sign packet");

  memset(&xmitStruct,'\0', sizeof(xmitStruct));
  xmitStruct.pktType = '0';
  
  if (radio.sendWithRetry(NODE_MEETINGLIGHT_DISPLAY, &xmitStruct, sizeof(xmitStruct), MAX_RETRIES, MAX_TIMEOUT))
    DEBUGln("Xmit new sign packet - ACK received")
  else
    DEBUGln("Xmit new sign packet - ACK not received")
}

// Send type 1 packet - change color
void sendColor()
{
  DEBUGln("Sending new color packet");

  memset(&xmitStruct,'\0', sizeof(xmitStruct));
  xmitStruct.pktType = '1';
  xmitStruct.red = colorList[currSignIndex][0];
  xmitStruct.green= colorList[currSignIndex][1];
  xmitStruct.blue = colorList[currSignIndex][2];
  
  if (radio.sendWithRetry(NODE_MEETINGLIGHT_DISPLAY, &xmitStruct, sizeof(xmitStruct), MAX_RETRIES, MAX_TIMEOUT))
    DEBUGln("Xmit send color - ACK received")
  else
    DEBUGln("Xmit send color - ACK not received") 
}

// Send type 2 packet - place a character
void sendCharacter(char c, int row, int col)
{
  int i, indx;
  byte initIndex[dataPoints];
  
  memset(&xmitStruct,'\0', sizeof(xmitStruct));
  xmitStruct.pktType = '2';
  xmitStruct.symbol = c;
  xmitStruct.row = row;
  xmitStruct.col = col;

  indx = findIniters(c);
  for (i=0; i<8; ++i)
    xmitStruct.vector[i] = EEPROM.read(EepromStartAddress + 2 + (indx * (dataPoints+1) + 1 + i)); 
  
  if (radio.sendWithRetry(NODE_MEETINGLIGHT_DISPLAY, &xmitStruct, sizeof(xmitStruct), MAX_RETRIES, MAX_TIMEOUT))
    DEBUGln("Xmit send character - ACK received")
  else
    DEBUGln("Xmit send character - ACK not received") 
}

// Send type 3 packet - place a border around the sign
void sendBorder()
{
  DEBUGln("Sending border packet")
  memset(&xmitStruct,'\0', sizeof(xmitStruct));
  xmitStruct.pktType = '3';

  if (radio.sendWithRetry(NODE_MEETINGLIGHT_DISPLAY, &xmitStruct, sizeof(xmitStruct), MAX_RETRIES, MAX_TIMEOUT))
    DEBUGln("Xmit send character - ACK received")
  else
    DEBUGln("Xmit send character - ACK not received")   
}

// Send type 4 packet - sign complete
void sendDone()
{
  DEBUGln("Sending done packet")
  memset(&xmitStruct,'\0', sizeof(xmitStruct));
  xmitStruct.pktType = '4';

  if (radio.sendWithRetry(NODE_MEETINGLIGHT_DISPLAY, &xmitStruct, sizeof(xmitStruct), MAX_RETRIES, MAX_TIMEOUT))
    DEBUGln("Xmit send character - ACK received")
  else
    DEBUGln("Xmit send character - ACK not received")   
}

int buttonEvent()
{
  int signLen, i;
 
  Serial.println("Button Event");
  while (digitalRead(button1pin) == LOW)  // Run out button push buffer
    {}
  
  // Increment sign index
  currSignIndex = ++currSignIndex % NUMSIGNS;
  oledDisplayText(oledSigns[currSignIndex]);
  // Reset character locations
  currentRow=0, currentColumn= 0;
  //lightLEDs();

  sendNewSign();
  sendColor();

  signLen = strlen(signs[currSignIndex]);
  for (i=0; i < signLen; ++i)
  {
    //DEBUG("Char=["); DEBUG(signs[currSignIndex][i]); DEBUGln("]");
    if (signs[currSignIndex][i] == '+')  // \n
    {
      ++currentRow;
      currentColumn= 0;
    }
    else
    {
      sendCharacter(signs[currSignIndex][i], currentRow, currentColumn);
      ++currentColumn;
    }
  }

  sendBorder();
  sendDone();
}
  
void loop() 
{
  if (digitalRead(button1pin) == LOW)
  {
    Serial.println("Got a button event");
    buttonEvent();
  }
}

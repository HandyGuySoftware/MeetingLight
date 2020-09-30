// Set EEPROM storage on the "button" device for MeetingLight system

#include <EEPROM.h>

const unsigned int EepromStartAddress = 10;
const unsigned int EepromSignature = 0x33;

#define NUMCHARS 18
#define BYTESPERCHAR 8

char charList[NUMCHARS] = {'O', 'K', 'E', 'N', 'T', 'R', 'V', 'I', 'D', 'C', 'A', 'L', 'U', '.', ' ', '+', 'M', 'X'};
byte initers[NUMCHARS][BYTESPERCHAR] = { 
                        { 0, 12, 18, 18, 18, 18, 12, 0},  // O
                        { 0, 18, 20, 24, 24, 20, 18, 0},  // K
                        { 0, 30, 16, 28, 28, 16, 30, 0},  // E
                        { 0, 18, 26, 22, 18, 18, 18, 0},  // N
                        { 0, 30, 12, 12, 12, 12, 12, 0},  // T
                        { 0, 28, 18, 18, 28, 20, 18, 0},  // R
                        { 0, 17, 17, 17, 17, 10, 4, 0},   // V
                        { 0, 12, 12, 12, 12, 12, 12, 0},  // I
                        { 0, 28, 18, 18, 18, 18, 28, 0},  // D
                        { 0, 14, 16, 16, 16, 16, 14, 0},  // C
                        { 0, 12, 18, 18, 30, 18, 18, 0},  // A
                        { 0, 16, 16, 16, 16, 16, 30, 0},  // L
                        { 0, 18, 18, 18, 18, 18, 30, 0},  // U
                        { 63, 63, 63, 63, 63, 63, 63, 63},  // .
                        { 0, 0, 0, 0, 0, 0, 0, 0},  // ' '
                        { 0, 0, 0, 0, 0, 0, 0, 0},  // '+' - Special \n terminator
                        { 0, 18, 30, 18, 18, 18, 18, 0},  // 'M'
                        { 0, 15, 15, 15, 15, 15, 15, 0}  // 'X'
                       };

void setup()
{
  int i, j, currAddress;
  byte b;
  
  Serial.begin(115200);

  currAddress = EepromStartAddress;
  Serial.print("Writing number of characters: "); Serial.println(sizeof(charList));
  EEPROM.update(currAddress, sizeof(charList));    // # of characters in vector

  Serial.println("Writing character vector size: 8");
  EEPROM.update(++currAddress, 8);    // # of data points per character
  
  Serial.println("Writing new default values");
  for (i=0; i<NUMCHARS; ++i)
  {
    Serial.print(charList[i]); Serial.print("-");
    EEPROM.update(++currAddress, charList[i]);    // Character
    for (j=0; j<BYTESPERCHAR; ++j)
    {
      EEPROM.update(++currAddress, initers[i][j]); // Byte vector
      Serial.print(initers[i][j]); Serial.print("-");
    }
    Serial.println("");
  }

  currAddress = EepromStartAddress;
  Serial.print("Reading number of characters: ");
  b = EEPROM.read(currAddress);    // # of characters in vector
  Serial.println(b);

  Serial.print("Reading vector size: ");
  b = EEPROM.read(++currAddress);    // # of characters in vector
  Serial.println(b);

  Serial.println("Reading default values");
  for (i=0; i<NUMCHARS; ++i)
  {
    b = EEPROM.read(++currAddress);    // Character
    Serial.print((char)b); Serial.print("-");
    for (j=0; j<BYTESPERCHAR; ++j)
    {
      b = EEPROM.read(++currAddress); // Byte vector
      Serial.print(b); Serial.print("-");
    }
    Serial.println("");
  }
}
  
void loop()
  {}

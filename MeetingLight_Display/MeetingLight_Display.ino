
// Program header files
#include <Wire.h>
#include "LEDMatrix.h"

//RGB matrix panel header files
#include <RGBmatrixPanel.h>
#include <gamma.h>
#include <gfxfont.h>
#include <Adafruit_GFX.h>

#define CLK 8  // MUST be on PORTB! (Use pin 11 on Mega)
#define LAT A3
#define OE  9
#define A   A0
#define B   A1
#define C   A2

RGBmatrixPanel LEDBoard(A, B, C, CLK, LAT, OE, false);

// Some global constants to give some flexibility
#define MATRIXHEIGHT 16
#define MATRIXWIDTH 32

// Define Arduino I/O pins for button management
#define SIGNALBUTTON1 11
#define SIGNALBUTTON2 12

LedMatrixClass mtrx(MATRIXHEIGHT, MATRIXWIDTH);  // size of matrix array

#define I2C_BUFFER_SIZE 32

struct 
{
  char pktType;
  unsigned short int red, blue, green;
  char symbol;
  unsigned short int row;
  unsigned short int col;
  byte vector[8];
} xmitStruct, *pXS;

void receiveEvent(int numBytes)
{
  byte *inPtr, i2cBuff[I2C_BUFFER_SIZE];
  int i, j;
  char tmp[100];

  Serial.print("Received Transmission. numBytes="); Serial.println(numBytes);
  inPtr = i2cBuff;
  while (Wire.available())
  {
    *inPtr++ = Wire.read();
  }

  memcpy(&xmitStruct, i2cBuff, numBytes);
  Serial.print("Pkt type="); Serial.print(xmitStruct.pktType); Serial.println("");

  switch (xmitStruct.pktType)
  {
    case '0': // New Screen
      Serial.println("Clear screen.");
      mtrx.clearScreen();
      break;
    case '1': // Set Color
      Serial.println("Color String."); 
      mtrx.setColor(xmitStruct.red, xmitStruct.green, xmitStruct.blue);
      break;
    case '2': // Character
      Serial.print("Place character. "); 
      Serial.print("Setting at row: "); Serial.print(xmitStruct.row); Serial.print("  column: "); Serial.print(xmitStruct.col); Serial.println("  ");
      mtrx.placeChar(xmitStruct.vector, xmitStruct.row * 8, (xmitStruct.col * 6)+1, true);
      break;
    case '3': // Fill border
      mtrx.fillBorder();
      break;
  }
  //sendACK(xmitStruct.pktType);
  return;
}

/*
void sendACK(char pktType)
{
  byte tmp[10];

  sprintf(tmp, "ACK%c", pktType);
  Wire.write(tmp, 4);
}
*/

void setup()
{
  //Serial.begin(115200); // open the serial port at 9600 bps:
	// Set mode buttons and indicator LED
	pinMode(SIGNALBUTTON1, INPUT); // Mode setting pin

	// Initialize the LED Matrix
	LEDBoard.begin();

  // Load I2C setup
  Wire.begin(2);
  Wire.onReceive(receiveEvent); // register event
	return;
}


void loop()
{
  return;
}

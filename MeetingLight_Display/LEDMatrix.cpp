#include "LEDMatrix.h"

// Headers for Adafruit LED Matric Panel
#include <RGBmatrixPanel.h>
#include <gamma.h>
#include <gfxfont.h>
#include <Adafruit_GFX.h>

int charGrid[][6] = { {0,0}, {0,6}, {0,12}, {0,18}, {0,24}, {0,30},
                      {8,0}, {8,6}, {8,12}, {8,18}, {8,24}, {8,30} };

extern RGBmatrixPanel LEDBoard;
//extern byte **display;

LedMatrixClass::LedMatrixClass(int h, int w)
{
	colorRed = colorGreen = colorBlue = 0;
}

/*
Set an individual pixel in the matrix display
rw, cl - row/column for pixel
*/
void LedMatrixClass::setPixel(int rw, int cl)
{

	LEDBoard.drawPixel(cl, rw, LEDBoard.Color333(colorRed, colorGreen, colorBlue));

	return;
}

/*
Turn off a pixel in the display.
*/
void LedMatrixClass::clearPixel(int r, int c)
{
	LEDBoard.drawPixel(c, r, LEDBoard.Color333(0, 0, 0));

	return;
}

/*
Place a character on the matrix
cd - ClockDigit class instance
r,c - Row/column for the digit
*/
void LedMatrixClass::placeChar(byte *iVector, int r, int c, bool fill=true)
{
	int bitVal, row, col, digit;
	int crows = 8;
	int ccols = 6;
	byte *vals = iVector;
 
  //Serial.print("placeChar:");Serial.println(cObj.getSymbol());
  for (row=0; row<crows; ++row)
  {
    //Serial.print("row=");Serial.println(row);
    //Serial.print("vals[row]=");Serial.println(vals[row]);
    for (col=0; col < ccols; ++col)
    {
      //Serial.print("col=");Serial.println(col);
      digit = vals[row] & (0x01 << (ccols - col-1));
      //Serial.print("digit=");Serial.println(digit);

      if (digit != 0)
      {
        setPixel(r + row, c + col);
        //display[r + row][c + col] = 1;
      }
      else
      {
        if (fill)
          //display[r + row][c + col] = 0;
          clearPixel(r + row, c + col);
      }
    }
	}
	return;
}

void LedMatrixClass::fillBorder()
{
  int row, col;

  for (row=0; row < 16; ++row)
  {
     //display[row][0] = 1;
     //display[row][31] = 1;
     setPixel(row, 0);
     setPixel(row, 31);
  }
  for (col=0; col< 32; ++col)
  {
     //display[0][col] = 1;
     //display[15][col] = 1;
     setPixel(0, col);
     setPixel(15, col);
  }
}


void LedMatrixClass::setColor(int r, int g, int b)
{
	colorRed = r;
	colorGreen = g;
	colorBlue = b;

	return ;
}

void LedMatrixClass::clearScreen()
{
	LEDBoard.fillScreen(LEDBoard.Color333(0, 0, 0));
}

void LedMatrixClass::fillScreen(int r=7, int g=7, int b=7)
{
	LEDBoard.fillScreen(LEDBoard.Color333(r, g, b));
}

void LedMatrixClass::saveColor()
{
	saveColorRed = colorRed;
	saveColorGreen = colorGreen;
	saveColorBlue = colorBlue;
}

void LedMatrixClass::restoreColor()
{
	colorRed = saveColorRed;
	colorGreen = saveColorGreen;
	colorBlue = saveColorBlue;
}

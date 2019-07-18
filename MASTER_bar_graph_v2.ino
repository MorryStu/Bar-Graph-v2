/******************************************************************
  This is an example for the Adafruit RA8875 Driver board for TFT displays
  ---------------> http://www.adafruit.com/products/1590
  The RA8875 is a TFT driver for up to 800x480 dotclock'd displays
  It is tested to work with displays in the Adafruit shop. Other displays
  may need timing adjustments and are not guanteed to work.
  Adafruit invests time and resources providing this open
  source code, please support Adafruit and open-source hardware
  by purchasing products from Adafruit!
  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, check license.txt for more information.
  All text above must be included in any redistribution.
 ******************************************************************/
// Also borrowed code from here --> https://www.hackster.io/LightPro/tft-graphing-bar-charts-185436 //
#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_RA8875.h"
#define RA8875_INT 3
#define RA8875_CS 10
#define RA8875_RESET 9
Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);
uint16_t tx, ty;
// Global Variables
float enginetemp;
float oilpressure;
float boostpressure;
float exhuasttemp;
int tempPin = 0;
int oilPin = 1;
int boostPin = 2;
int exhuastPin = 3;

int valueBlock[4];
int posBlock[4];
int prevPosBlock[4];
// Editable Variables
bool proDebug = 1;                          // TODO - Debugging? 0 = No, 1 = Yes
bool displayValues = true;                  // EDIT - Display Value of Blocks?


int graphRange = 1024;                      // EDIT - Max possible number of block, eg Potentiometer = from 0 to 1024, so 1024
int numberOfBlocks = 4;                     // EDIT - Number of Blocks to display

// calculate values
const int numberOfMarks = (numberOfBlocks * 2); // calculates amount of lines that mark x axis
const int originX = 1; // the origin of the graph on the x axis
const int originY = 400; // the origin of the graph on the y axis
const int sizeX = 780; // the size of the chart on the x axis (lenght)
const int sizeY = 300; // the size of the chart on the y axis (height)

int boxSize = (sizeX / numberOfMarks); // calculates the size between the marks
int mark[] = {boxSize, (boxSize * 2), (boxSize * 3), (boxSize * 4), (boxSize * 5), (boxSize * 6), (boxSize * 7), (boxSize * 8)}; // calculates the position of the marks


int numberSize = (sizeY / 10); // calculates the size between the numbers
int number[] = {numberSize, (numberSize * 2), (numberSize * 3), (numberSize * 4), (numberSize * 5), (numberSize * 6)}; // calculates the position of the numbers

int numberValue = (graphRange / 10); // calculates the value per number
int val[] = {graphRange, (numberValue * 5), (numberValue * 4), (numberValue * 3), (numberValue * 2), numberValue}; // calculates the value of each number


void drawGraph()
{ Serial.begin(9600);
  Serial.println("RA8875 start");

  //Initialize the display using 'RA8875_480x80', 'RA8875_480x128', 'RA8875_480x272' or 'RA8875_800x480'
  if (!tft.begin(RA8875_800x480))
  {
    Serial.println("RA8875 Not Found!");
    while (1);
  }

  Serial.println("Found RA8875");

  tft.displayOn(true);
  tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
  tft.PWM1out(255);
  tft.textMode();

  tft.fillScreen(RA8875_BLACK);


}


void graph()
{
  // get the values of the sensors
  valueBlock[0] = analogRead(A0);
  valueBlock[1] = analogRead(A1);
  valueBlock[2] = analogRead(A2);
  valueBlock[3] = analogRead(A3);
  tft.displayOn(true);
  tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
  tft.PWM1out(255);
  tft.textMode();
  tft.textSetCursor(10, 10);
  tft.textColor(RA8875_WHITE, RA8875_BLACK);
  tft.textEnlarge(8);
  tft.textWrite("  Temp   Oil  Boost  EGT");
  char Tempstring[30];
  char Oilstring[30];
  char Booststring[30];
  char Exhuaststring[30];
  dtostrf(enginetemp, 2, 0, Tempstring);
  dtostrf(oilpressure, 2, 0, Oilstring);
  dtostrf(boostpressure, 2, 0, Booststring);
  dtostrf(exhuasttemp, 2, 0, Exhuaststring);
  tft.displayOn(true);
  tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
  tft.PWM1out(255);
  tft.textMode();
  tft.textColor(RA8875_CYAN, RA8875_BLACK);
  tft.textEnlarge(2);
  tft.textSetCursor(80, 399);
  tft.textWrite(Tempstring);
  tft.textWrite(" *C");
  tft.textColor(RA8875_YELLOW, RA8875_BLACK);
  tft.textEnlarge(2);
  tft.textSetCursor(270, 399);
  tft.textWrite(Oilstring);
  tft.textWrite(" Psi");
  tft.textEnlarge(2);
  tft.textColor(RA8875_GREEN, RA8875_BLACK);
  tft.textSetCursor(460, 399);
  tft.textWrite(Booststring);
  tft.textWrite(" Psi");
  tft.textEnlarge(2);
  tft.textColor(RA8875_WHITE, RA8875_BLACK);
  tft.textSetCursor(650, 399);
  tft.textWrite(Exhuaststring);
  tft.textWrite(" *C");

  delay(1);
  if (proDebug)

  {
    Serial.println(valueBlock[0]);
    Serial.println(valueBlock[1]);
    Serial.println(valueBlock[2]);
    Serial.println(valueBlock[3]);

    //Serial.println("");
  }

  // map the sensor values to the graph size
  for (int i = 0; i < numberOfBlocks; i++)
  {
    posBlock[i] = map(valueBlock[i], 0, graphRange, originY, (originY - sizeY));
  }

  if (proDebug)
  {
    Serial.println(posBlock[0]);
    Serial.println(posBlock[1]);
    Serial.println(posBlock[2]);
    Serial.println(posBlock[3]);
    Serial.println("");
  }

  // draw the blocks - draw only if value differs
  for (int i = 0; i < numberOfBlocks; i++)
  {
    if (posBlock[i] > (prevPosBlock[i] + 2) || posBlock[i] < (prevPosBlock[i] - 2))
    {
      prevPosBlock[i] = posBlock[i];
      tft.fillRect((mark[i * 2] + 1), (originY - sizeY), (boxSize - 1), sizeY, RA8875_BLACK);
      delay(10);
      tft.fillRect((mark[i * 2] + 1), posBlock[i], (boxSize - 1), (originY - posBlock[i]), RA8875_RED);
    }
  }

  // display values of sensors if mode selected
  if (displayValues)
  {
    for (int i = 0; i < numberOfMarks; i += 2)
    {
      tft.textSetCursor(10, 10);
      tft.textColor(RA8875_RED, RA8875_WHITE);
      tft.setTextSize(2);
      //tft.textWrite(valueBlock[i / 2]);
    }
  }
}

void setup()
{

  {
    Serial.begin(9600);
    while (!Serial) {};
  }
  {
    delay(500);

    drawGraph(); // draws the graph
  }
}
void loop()
{
  graph(); // graphs the values on the graph
  delay(500);
  enginetemp = analogRead(tempPin);
  enginetemp = (5.0 * enginetemp * 100.0) / 1024.0;
  oilpressure = analogRead(oilPin);
  oilpressure = (5.0 * oilpressure * 100.0) / 1024. / 5.8; //Display 0-70Psi
  boostpressure = analogRead(boostPin);
  boostpressure = (5.0 * boostpressure * 100.0) / 1024. / 14; //Display 0-30Psi
  exhuasttemp = analogRead(exhuastPin);
  exhuasttemp = (5.0 * exhuasttemp * 100.0) / 1024.*2; //Display 0-800

}

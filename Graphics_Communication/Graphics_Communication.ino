/***************************************************
  This is our GFX example for the Adafruit ILI9341 Breakout and Shield
  ----> http://www.adafruit.com/products/1651

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/


#include "SPI.h"
//#include "ILI9341_t3.h"
//#include "font_Arial.h"
#include "ILI9341_t3n.h"
#include "ili9341_t3n_font_Arial.h"

#define TFT_DC  9
#define TFT_RST 8
#define TFT_CS 10
#define TFT_MOSI 11
#define TFT_MISO 12
#define TFT_SCLK 13

#define HWSERIAL Serial1
#define Serial1_RX 0
#define Serial1_TX 1

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
//ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK, TFT_MISO);
ILI9341_t3n tft = ILI9341_t3n(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK, TFT_MISO);

// Note: you can now set the SPI speed to any value
// the default value is 30Mhz, but most ILI9341 displays
// can handle at least 60Mhz and as much as 100Mhz
//#define DISPLAY_SPICLOCK 30000000
#define DISPLAY_SPICLOCK 60000000
//#define DISPLAY_SPICLOCK 100000000
#define DISPLAY_SPICLOCK_READ 2000000


const byte numChars = 32;
// array for use when receiving
char receivedChars[numChars];
// temporary array for use when parsing
char tempChars[numChars];
// variables to hold the parsed data
char messageFromPC[numChars] = {0};
int integerX = 0;
int integerY = 0;
float floatFromPC = 0.0;

boolean newData = false;

void setup() {
  tft.begin(DISPLAY_SPICLOCK, DISPLAY_SPICLOCK_READ);
  tft.setRotation(3);
  //tft.setRotation(0);
  tft.fillScreen(ILI9341_BLACK);

  Serial.begin(9600);
  //  Serial.begin(19200);
  //  Serial.begin(28800);
  //  Serial.begin(38400);
  //  Serial.begin(57600);
  //  Serial.begin(76800);
  //  Serial.begin(115200);
  //  Serial.begin(230400);
  //  Serial.begin(460800);
  //  Serial.begin(576000);
  //  Serial.begin(921600);

  HWSERIAL.setRX(Serial1_RX);
  HWSERIAL.setTX(Serial1_TX);

  //  HWSERIAL.begin(9600);
  //  HWSERIAL.begin(19200);
  //  HWSERIAL.begin(28800);
  //  HWSERIAL.begin(38400);
  //  HWSERIAL.begin(57600);
  //  HWSERIAL.begin(76800);
  //  HWSERIAL.begin(115200);
  //  HWSERIAL.begin(230400);
  //  HWSERIAL.begin(460800);
  //  HWSERIAL.begin(576000);
  HWSERIAL.begin(921600);
}

void loop(void) {
  
  if (newData == false) {
    recvWithStartEndMarkers();
    parser();
  }

}

void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  Serial.println("---Receive With Start End Markers");
  while (HWSERIAL.available() > 0 && newData == false) {
    rc = HWSERIAL.read();

    if (recvInProgress == true) {

      if (strcmp(messageFromPC, "FillScreen") == 0 || strcmp(messageFromPC, "fillscreen") == 0) {
        tft.fillScreen(ILI9341_BLACK);
        testText();
        //testProportionalText();
      }

      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        receivedChars[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }

    else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}

void parser() {
  
  Serial.println("---Parser");
  if (newData == true) {
    strcpy(tempChars, receivedChars);
    // this temporary copy is necessary to protect the original data
    //   because strtok() used in parseData() replaces the commas with \0
    parseData();
    //<Function1,10,0xFFFF,0.0>
    if (strcmp(messageFromPC, "FillScreen") == 0 || strcmp(messageFromPC, "fillscreen") == 0) {
      tft.fillScreen(ILI9341_BLACK);
      testText();
      //testProportionalText();
    }
    if (strcmp(messageFromPC, "Function1") == 0) {
      testCircles(integerX, integerY);
      monitor_showParsedData();
      display_parsed_transmission();
    }
    newData = false;

  }
}

void parseData() {      // split the data into its parts

  Serial.println("---Parsing Data");
  char * strtokIndx; // this is used by strtok() as an index

  strtokIndx = strtok(tempChars, ",");     // get the first part - the string
  strcpy(messageFromPC, strtokIndx); // copy it to messageFromPC

  strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
  integerX = atoi(strtokIndx);     // convert this part to an integer

  strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
  integerY = atoi(strtokIndx);     // convert this part to an integer
  integerY = strtol(strtokIndx, NULL, 16);

  strtokIndx = strtok(NULL, ",");
  floatFromPC = atof(strtokIndx);     // convert this part to a float
}

void monitor_showParsedData() {
  Serial.print("Message ");
  Serial.println(messageFromPC);
  Serial.print("IntegerX ");
  Serial.println(integerX);
  Serial.print("IntegerY ");
  Serial.println(integerY);
  Serial.print("Float ");
  Serial.println(floatFromPC);
}

unsigned long display_parsed_transmission() {
  tft.fillScreen(ILI9341_BLACK);
  unsigned long start = micros();
  //tft.setTextColor(ILI9341_BLUE);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
  tft.println("Message ");
  tft.setTextColor(ILI9341_BLUE);  tft.setTextSize(1);
  tft.println(messageFromPC);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
  tft.println("IntegerX ");
  tft.setTextColor(ILI9341_BLUE);  tft.setTextSize(1);
  tft.println(integerX);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
  tft.println("integerY ");
  tft.setTextColor(ILI9341_BLUE);  tft.setTextSize(1);
  tft.println(integerY, HEX);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
  tft.println("Float ");
  tft.setTextColor(ILI9341_BLUE);  tft.setTextSize(1);
  tft.println(floatFromPC);
  tft.println();
  return micros() - start;
}

unsigned long testFillScreen() {
  unsigned long start = micros();
  tft.fillScreen(ILI9341_BLACK);
  tft.fillScreen(ILI9341_RED);
  tft.fillScreen(ILI9341_GREEN);
  tft.fillScreen(ILI9341_BLUE);
  tft.fillScreen(ILI9341_BLACK);
  return micros() - start;
}

unsigned long testText() {
  tft.fillScreen(ILI9341_BLACK);
  unsigned long start = micros();
  tft.setCursor(0, 0);

  tft.setTextColor(ILI9341_GREEN);
  //tft.setTextSize(5);
  //tft.println("12345678");
  tft.setTextSize(2);
  tft.println("1234567890123456789012345");
  tft.setTextSize(1);
  tft.println("28  52 ----------------WIDE------------------ 52  28");
  tft.println("| 123456789012345678901234567890123456789012345678 |");
  tft.println("| 123456789012345678901234567890123456789012345678 |");
  tft.println("| 123456789012345678901234567890123456789012345678 |");
  tft.println("| 123456789012345678901234567890123456789012345678 |");
  tft.println("| 123456789012345678901234567890123456789012345678 |");
  tft.println("| 123456789012345678901234567890123456789012345678 |");
  tft.println("| 123456789012345678901234567890123456789012345678 |");
  tft.println("| 123456789012345678901234567890123456789012345678 |");
  tft.println("| 123456789012345678901234567890123456789012345678 |");
  tft.println("| 123456789012345678901234567890123456789012345678 |");
  tft.println("| 123456789012345678901234567890123456789012345678 |");
  tft.println("L 123456789012345678901234567890123456789012345678 L");
  tft.println("O 123456789012345678901234567890123456789012345678 O");
  tft.println("N 123456789012345678901234567890123456789012345678 N");
  tft.println("G 123456789012345678901234567890123456789012345678 G");
  tft.println("| 123456789012345678901234567890123456789012345678 |");
  tft.println("| 123456789012345678901234567890123456789012345678 |");
  tft.println("| 123456789012345678901234567890123456789012345678 |");
  tft.println("| 123456789012345678901234567890123456789012345678 |");
  tft.println("| 123456789012345678901234567890123456789012345678 |");
  tft.println("| 123456789012345678901234567890123456789012345678 |");
  tft.println("| 123456789012345678901234567890123456789012345678 |");
  tft.println("| 123456789012345678901234567890123456789012345678 |");
  tft.println("| 123456789012345678901234567890123456789012345678 |");
  tft.println("| 123456789012345678901234567890123456789012345678 |");
  tft.println("  123456789012345678901234567890123456789012345678  ");
  tft.println("28  52 ----------------WIDE------------------ 52  28");

  return micros() - start;
}

unsigned long testProportionalText() {
  tft.fillScreen(ILI9341_BLACK);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);  tft.setFont(Arial_10);
  tft.println("Hello World!");
  tft.setTextColor(ILI9341_YELLOW); tft.setFont(Arial_16);
  tft.println(1234.56);
  tft.setTextColor(ILI9341_RED);    tft.setFont(Arial_24);
  tft.println(0xDEADBEEF, HEX);
  tft.println();
  tft.setTextColor(ILI9341_GREEN);
  tft.setFont(Arial_40);
  tft.println("Groop");
  tft.setFont(Arial_16);
  tft.println("I implore thee,");
  tft.setFont(Arial_10);
  tft.println("my foonting turlingdromes.");
  tft.println("And hooptiously drangle me");
  tft.println("with crinkly bindlewurdles,");
  tft.println("Or I will rend thee");
  tft.println("in the gobberwarts");
  tft.println("with my blurglecruncheon,");
  tft.println("see if I don't!");
  tft.setFontAdafruit();
  return micros() - start;
}

unsigned long testLines(uint16_t color) {
  unsigned long start, t;
  int           x1, y1, x2, y2,
                w = tft.width(),
                h = tft.height();

  tft.fillScreen(ILI9341_BLACK);

  x1 = y1 = 0;
  y2    = h - 1;
  start = micros();
  for (x2 = 0; x2 < w; x2 += 6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for (y2 = 0; y2 < h; y2 += 6) tft.drawLine(x1, y1, x2, y2, color);
  t     = micros() - start; // fillScreen doesn't count against timing

  tft.fillScreen(ILI9341_BLACK);

  x1    = w - 1;
  y1    = 0;
  y2    = h - 1;
  start = micros();
  for (x2 = 0; x2 < w; x2 += 6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for (y2 = 0; y2 < h; y2 += 6) tft.drawLine(x1, y1, x2, y2, color);
  t    += micros() - start;

  tft.fillScreen(ILI9341_BLACK);

  x1    = 0;
  y1    = h - 1;
  y2    = 0;
  start = micros();
  for (x2 = 0; x2 < w; x2 += 6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for (y2 = 0; y2 < h; y2 += 6) tft.drawLine(x1, y1, x2, y2, color);
  t    += micros() - start;

  tft.fillScreen(ILI9341_BLACK);

  x1    = w - 1;
  y1    = h - 1;
  y2    = 0;
  start = micros();
  for (x2 = 0; x2 < w; x2 += 6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for (y2 = 0; y2 < h; y2 += 6) tft.drawLine(x1, y1, x2, y2, color);

  return micros() - start;
}

unsigned long testFastLines(uint16_t color1, uint16_t color2) {
  unsigned long start;
  int           x, y, w = tft.width(), h = tft.height();

  tft.fillScreen(ILI9341_BLACK);
  start = micros();
  for (y = 0; y < h; y += 5) tft.drawFastHLine(0, y, w, color1);
  for (x = 0; x < w; x += 5) tft.drawFastVLine(x, 0, h, color2);

  return micros() - start;
}

unsigned long testRects(uint16_t color) {
  unsigned long start;
  int           n, i, i2,
                cx = tft.width()  / 2,
                cy = tft.height() / 2;

  tft.fillScreen(ILI9341_BLACK);
  n     = min(tft.width(), tft.height());
  start = micros();
  for (i = 2; i < n; i += 6) {
    i2 = i / 2;
    tft.drawRect(cx - i2, cy - i2, i, i, color);
  }

  return micros() - start;
}

unsigned long testFilledRects(uint16_t color1, uint16_t color2) {
  unsigned long start, t = 0;
  int           n, i, i2,
                cx = tft.width()  / 2 - 1,
                cy = tft.height() / 2 - 1;

  tft.fillScreen(ILI9341_BLACK);
  n = min(tft.width(), tft.height()) - 1;
  for (i = n; i > 0; i -= 6) {
    i2    = i / 2;
    start = micros();
    tft.fillRect(cx - i2, cy - i2, i, i, color1);
    t    += micros() - start;
    // Outlines are not included in timing results
    tft.drawRect(cx - i2, cy - i2, i, i, color2);
  }

  return t;
}

unsigned long testFilledCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  int x, y, w = tft.width(), h = tft.height(), r2 = radius * 2;

  tft.fillScreen(ILI9341_BLACK);
  start = micros();
  for (x = radius; x < w; x += r2) {
    for (y = radius; y < h; y += r2) {
      tft.fillCircle(x, y, radius, color);
    }
  }

  return micros() - start;
}

unsigned long testCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  int           x, y, r2 = radius * 2,
                      w = tft.width()  + radius,
                      h = tft.height() + radius;

  // Screen is not cleared for this one -- this is
  // intentional and does not affect the reported time.
  start = micros();
  for (x = 0; x < w; x += r2) {
    for (y = 0; y < h; y += r2) {
      tft.drawCircle(x, y, radius, color);
    }
  }

  return micros() - start;
}

unsigned long testTriangles() {
  unsigned long start;
  int           n, i, cx = tft.width()  / 2 - 1,
                      cy = tft.height() / 2 - 1;

  tft.fillScreen(ILI9341_BLACK);
  n     = min(cx, cy);
  start = micros();
  for (i = 0; i < n; i += 5) {
    tft.drawTriangle(
      cx    , cy - i, // peak
      cx - i, cy + i, // bottom left
      cx + i, cy + i, // bottom right
      tft.color565(0, 0, i));
  }

  return micros() - start;
}

unsigned long testFilledTriangles() {
  unsigned long start, t = 0;
  int           i, cx = tft.width()  / 2 - 1,
                   cy = tft.height() / 2 - 1;

  tft.fillScreen(ILI9341_BLACK);
  start = micros();
  for (i = min(cx, cy); i > 10; i -= 5) {
    start = micros();
    tft.fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
                     tft.color565(0, i, i));
    t += micros() - start;
    tft.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
                     tft.color565(i, i, 0));
  }

  return t;
}

unsigned long testRoundRects() {
  unsigned long start;
  int           w, i, i2,
                cx = tft.width()  / 2 - 1,
                cy = tft.height() / 2 - 1;

  tft.fillScreen(ILI9341_BLACK);
  w     = min(tft.width(), tft.height()) - 1;
  start = micros();
  for (i = 0; i < w; i += 6) {
    i2 = i / 2;
    tft.drawRoundRect(cx - i2, cy - i2, i, i, i / 8, tft.color565(i, 0, 0));
  }

  return micros() - start;
}

unsigned long testFilledRoundRects() {
  unsigned long start;
  int           i, i2,
                cx = tft.width()  / 2 - 1,
                cy = tft.height() / 2 - 1;

  tft.fillScreen(ILI9341_BLACK);
  start = micros();
  for (i = min(tft.width(), tft.height()) - 1; i > 20; i -= 6) {
    i2 = i / 2;
    tft.fillRoundRect(cx - i2, cy - i2, i, i, i / 8, tft.color565(0, i, 0));
  }

  return micros() - start;
}

void benchmark_test() {
  //delay(250);
  //while (!Serial) ; // wait for Arduino Serial Monitor
  //  Serial.println("ILI9341 Test!");

  //  read diagnostics (optional but can help debug problems)
  //  uint8_t x = tft.readcommand8(ILI9341_RDMODE);
  //  Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
  //  x = tft.readcommand8(ILI9341_RDMADCTL);
  //  Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
  //  x = tft.readcommand8(ILI9341_RDPIXFMT);
  //  Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
  //  x = tft.readcommand8(ILI9341_RDIMGFMT);
  //  Serial.print("Image Format: 0x"); Serial.println(x, HEX);
  //  x = tft.readcommand8(ILI9341_RDSELFDIAG);
  //  Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX);

  //  Serial.println(F("Benchmark                Time (microseconds)"));

  //Serial.print(F("Screen fill              "));
  //Serial.println(testFillScreen());
  //delay(200);
  //Serial.print(F("Text                     "));
  //Serial.println(testText());
  //delay(600);
  //Serial.print(F("Proportional Text        "));
  //Serial.println(testProportionalText());
  //delay(600);
  //Serial.print(F("Lines                    "));
  //Serial.println(testLines(ILI9341_CYAN));
  //delay(200);
  //Serial.print(F("Horiz/Vert Lines         "));
  //Serial.println(testFastLines(ILI9341_RED, ILI9341_BLUE));
  //delay(200);
  //Serial.print(F("Rectangles (outline)     "));
  //Serial.println(testRects(ILI9341_GREEN));
  //delay(200);
  //Serial.print(F("Rectangles (filled)      "));
  //Serial.println(testFilledRects(ILI9341_YELLOW, ILI9341_MAGENTA));
  //delay(200);
  //Serial.print(F("Circles (filled)         "));
  //Serial.println(testFilledCircles(10, ILI9341_MAGENTA));
  //Serial.print(F("Circles (outline)        "));
  //Serial.println(testCircles(10, ILI9341_WHITE));
  //delay(200);
  //Serial.print(F("Triangles (outline)      "));
  //Serial.println(testTriangles());
  //delay(200);
  //Serial.print(F("Triangles (filled)       "));
  //Serial.println(testFilledTriangles());
  //delay(200);
  //Serial.print(F("Rounded rects (outline)  "));
  //Serial.println(testRoundRects());
  //delay(200);
  //Serial.print(F("Rounded rects (filled)   "));
  //Serial.println(testFilledRoundRects());
  //delay(200);
  //Serial.println(F("Done!"));

}

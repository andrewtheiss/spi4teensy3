/*  OctoWS2811 Rainbow.ino - Rainbow Shifting Test
    http://www.pjrc.com/teensy/td_libs_OctoWS2811.html
    Copyright (c) 2013 Paul Stoffregen, PJRC.COM, LLC

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.


  Required Connections
  --------------------
    pin 2:  LED Strip #1    OctoWS2811 drives 8 LED Strips.
    pin 14: LED strip #2    All 8 are the same length.
    pin 7:  LED strip #3
    pin 8:  LED strip #4    A 100 ohm resistor should used
    pin 6:  LED strip #5    between each Teensy pin and the
    pin 20: LED strip #6    wire to the LED strip, to minimize
    pin 21: LED strip #7    high frequency ringining & noise.
    pin 5:  LED strip #8
    pin 15 & 16 - Connect together, but do not use
    pin 4 - Do not use
    pin 3 - Do not use as PWM.  Normal use is ok.
    pin 1 - Output indicating CPU usage, monitor with an oscilloscope,
            logic analyzer or even an LED (brighter = CPU busier)
*/

#include <OctoWS2811.h>

// Total 332
// 332 - 109 = 223
// 109 , 109 , 109
const int ledsPerStrip = 332;
const int ledsPerStripMirrored = 166;

DMAMEM int displayMemory[ledsPerStrip*6];
int drawingMemory[ledsPerStrip*6];

const int config = WS2811_GRB | WS2811_800kHz;

OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);

int rainbowColors[360];

// Display the first and slowly generate the next
int singleColorA[50];
int singleColorSwitchCycleCount = 1000;
int singleColorHue = 300;
int singleColorTargetHue = 350;

void setup() {
  pinMode(1, OUTPUT);
  digitalWrite(1, HIGH);
  for (int i=0; i<360; i++) {
    int hue = i;
    int saturation = 100;
    int lightness = 50;
    // pre-compute the 180 rainbow colors
    rainbowColors[i] = makeColor(hue, saturation, lightness);    
  }

  for (int i = 0; i < 50; i++) {
    int saturation = 100;
    int lightness = 50;
    singleColorA[i] = makeColor(singleColorHue, saturation, i);
    
  }
  digitalWrite(1, LOW);
  leds.begin();
}


void loop() {
  //rainbow(10, 2500);
  //rainbowMirrored(6600);
  fadeSingleColorMirrored(12500);
}


// phaseShift is the shift between each row.  phaseShift=0
// causes all rows to show the same colors moving together.
// phaseShift=180 causes each row to be the opposite colors
// as the previous.
//
// cycleTime is the number of milliseconds to shift through
// the entire 360 degrees of the color wheel:
// Red -> Orange -> Yellow -> Green -> Blue -> Violet -> Red
//
void rainbow(int phaseShift, int cycleTime)
{
  int color, x, y, wait;

  wait = cycleTime * 1000 / ledsPerStrip;
  for (color=0; color < 360; color++) {
    digitalWrite(1, HIGH);
    for (x=0; x < ledsPerStrip; x++) {
      for (y=0; y < 8; y++) {
        int index = (color + x + y*phaseShift/2) % 360;
        leds.setPixel(x + y*ledsPerStrip, rainbowColors[index]);
      }
    }
    leds.show();
    digitalWrite(1, LOW);
    delayMicroseconds(wait);
  }
}

// Figure out patterns 
// Single hue -> empty -> single hue
void rainbowMirrored(int cycleTime) {
// go forwards and backwards with the same code until we meet
  
  int color, x, y, wait;

  wait = cycleTime * 1000 / ledsPerStrip;
  for (color=0; color < 360; color++) {
    digitalWrite(1, HIGH);
    for (x=0; x < ledsPerStripMirrored; x++) {
      for (y=0; y < 8; y++) {
        int index = (color + x) % 360;
        leds.setPixel(x + y*ledsPerStrip, rainbowColors[index]);
        leds.setPixel((ledsPerStripMirrored * 2) - x + y*ledsPerStrip, rainbowColors[index]);
      }
    }
    leds.show();
    digitalWrite(1, LOW);
    delayMicroseconds(wait);
  } 
}


// Single color fade from 100 -> 0 lightness
void fadeSingleColorMirrored(int cycleTime) {
    
  int color, x, y, wait;

  singleColorHue++;
  for (int i = 0; i < 50; i++) {
    singleColorA[i] = makeColor(singleColorHue, 100, i);
  }

  
  wait = cycleTime * 1000 / ledsPerStrip;
  for (color=0; color < 50; color++) {
    digitalWrite(1, HIGH);
    for (x=0; x < ledsPerStripMirrored; x++) {
      for (y=0; y < 8; y++) {
        int index = (color + x) % 50;
        leds.setPixel(x + y*ledsPerStrip, singleColorA[index]);
        leds.setPixel((ledsPerStripMirrored * 2) - x + y*ledsPerStrip, singleColorA[index]);
      }
      
      for (y=0; y < 8; y++) {
        int index = (color + x) % 50;
        leds.setPixel(ledsPerStripMirrored + y*ledsPerStrip, singleColorA[index]);
      }
    }
    leds.show();
    digitalWrite(1, LOW);
    delayMicroseconds(wait);
  } 
}


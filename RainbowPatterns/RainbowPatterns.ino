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
#include <avr/io.h>
#include <avr/interrupt.h>

// Brightness settings
int numberBrightnessLevels = 10; //set this to number of bightness levels
float brightnessLevels[] = {1.0f, 0.5f, 0.25f, 0.04f, 0.01f}; //Set to maximum brightness
volatile int brightnessNumber = 4;

// Pattern Details
int patternSolidColors[] = {0, 60, 120, 180, 240, 300};
volatile int patternSolidColor = 0;
int patternSolidColorCount = 6;

// Button Ports
// Note: These should not conflict with LED push items
int buttonBrightness = 3; //button #1 for brightness
int buttonPattern = 9; //button #2 for pattern cycling
int buttonInterAnimationId = 10; //button #3 for changing pattern bank


//Animation 
volatile int pattern = 0;
int maxPatterns = 2;

// InterAnimationId
volatile int interAnimationSelection = 0;
int interAnimationIdSizePerPattern[] = {5, 3, patternSolidColorCount};  // SHOULD NEVER BE MORE THAN maxPatterns

//variables to keep track of the timing of recent interrupts
volatile bool buttonPatternIsPressed = false;
volatile unsigned long buttonPatternPressedTime = 0;
long buttonPatternPressedTimeDelay = 180;
volatile bool buttonInterAnimationIsPressed = false;
volatile unsigned long buttonInterAnimationPressedTime = 0;
long buttonInterAnimationPressedTimeDelay = 180;
volatile bool buttonBrightnessIsPressed = false;

#include <OctoWS2811.h>

const int ledsPerStrip = 106 * 4;

DMAMEM int displayMemory[ledsPerStrip*6];
int drawingMemory[ledsPerStrip*6];

const int config = WS2811_GRB | WS2811_800kHz;

OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);

int rainbowColors[180];
int rainbowColorsHalf[180];
int rainbowColorsQuarter[180];
int rainbowColorsLow[180];
int rainbowColorsVeryLow[180];

void setup() {

  //pin setups
  pinMode(buttonBrightness, INPUT_PULLUP);     //define pin 3 as input
  pinMode(buttonPattern, INPUT_PULLUP);     //define pin 8 as input
  pinMode(buttonInterAnimationId, INPUT_PULLUP);     //define pin 9 as input
  
  
  pinMode(1, OUTPUT);
  digitalWrite(1, HIGH);
  for (int i=0; i<180; i++) {
    int hue = i * 2;
    int saturation = 100;
    int lightness = 50;
    // pre-compute the 180 rainbow colors
    rainbowColors[i] = makeColor(hue, saturation, lightness);
    lightness = 25;
    rainbowColorsHalf[i] = makeColor(hue, saturation, lightness);
    lightness = 12;
    rainbowColorsQuarter[i] = makeColor(hue, saturation, lightness);
    lightness = 4;
    rainbowColorsLow[i] = makeColor(hue, saturation, lightness);
    lightness = 1;
    rainbowColorsVeryLow[i] = makeColor(hue, saturation, lightness);
  }
  digitalWrite(1, LOW);
  leds.begin();

  attachInterrupt(buttonPattern,  cyclePattern, FALLING);
  attachInterrupt(buttonInterAnimationId, cycleInterAnimationPattern, FALLING);
  //attachInterrupt(buttonBrightness, brightnessButtonPressed, RISING);
}
/*
// Update Brightness
void brightnessButtonPressed() {
  // If the button is pressed, toggle button on state and run button pressed code
  if ((digitalRead(buttonBrightness)== LOW) && !buttonBrightnessIsPressed) { 
    brightnessControl();
  } else {
    buttonBrightnessIsPressed = false;
  }
}
*/
// Switch between patterns
void cyclePattern() {
  // If the button is pressed, toggle button on state and run button pressed code

  // Check for time delay between button presses
  unsigned long latestTime = millis();
  bool latestTimeWithinDelay = false;
  if ((latestTime - buttonPatternPressedTimeDelay) > buttonPatternPressedTime) {
    latestTimeWithinDelay = true;
  }
  
  if ((digitalRead(buttonPattern)== LOW) && !buttonPatternIsPressed && latestTimeWithinDelay) {

    // Change pattern
    buttonPatternPressedTime = latestTime;
    if ((pattern + 1) >= maxPatterns) {
      pattern = 0;
    } else {
      pattern++;
    }
    
    buttonPatternIsPressed = true;
  } else {
    buttonPatternIsPressed = false;
  }
}

void cycleInterAnimationPattern() {
  
  // Check for time delay between button presses
  unsigned long latestTime = millis();
  bool latestTimeWithinDelay = false;
  if ((latestTime - buttonInterAnimationPressedTimeDelay) > buttonInterAnimationPressedTime) {
    latestTimeWithinDelay = true;
  }

  if ((digitalRead(buttonInterAnimationId) == LOW) && !buttonInterAnimationIsPressed && latestTimeWithinDelay) {
    
    buttonInterAnimationPressedTime = latestTime;
    if ((interAnimationSelection + 1) >= interAnimationIdSizePerPattern[pattern]) {
      interAnimationSelection = 0;
    } else {
      interAnimationSelection++;
    }
    buttonInterAnimationIsPressed = true;
  } else {
    buttonInterAnimationIsPressed = false;
  }
}

void loop() {
  switch(pattern) {
    case 0:
    renderRainbowAnimation();
      break;
    case 1:
     renderSolidAnimation();
      break;
    default:
      solid(140);
      break;
  }
}


// Pattern #0
void renderRainbowAnimation() {
  switch (interAnimationSelection) {
    default: 
      rainbow(10, 2500);
      break;
  }
}

// Pattern #1
void renderSolidAnimation() {
  switch (interAnimationSelection) {
    case 0:
      solid(50);
      break;
    case 1:
      solid(175);
      break;
    case 2:
      solid(150);
      break;
    default: 
      solid(110);
      break;
  }
}

//brightness button interrupt
void brightnessControl() {
  //buttonBrightnessIsPressed = true;

  if (brightnessNumber < numberBrightnessLevels) {
    brightnessNumber++;
  }
  else {
    brightnessNumber = 0;
  }
}

int adjustForBrightness(int input) {
  float adjustedBright = float(input) * brightnessLevels[brightnessNumber];
  return round(adjustedBright);
}

// Cycle Solid Color
void solid(int colorToSet)
{
  int x, y, wait;
  wait = 1;
  digitalWrite(1, HIGH);
  for (x=0; x < ledsPerStrip; x++) {
    for (y=0; y < 8; y++) {
      leds.setPixel(x + y*ledsPerStrip, rainbowColors[colorToSet]); 
    }
  }
  leds.show();
  digitalWrite(1, LOW);
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
  for (color=0; color < 180; color++) {
    digitalWrite(1, HIGH);
    for (x=0; x < ledsPerStrip; x++) {
      for (y=0; y < 8; y++) {
        int index = (color + x + y*phaseShift/2) % 180;

        switch( interAnimationSelection) {
          case 0:
        leds.setPixel(x + y*ledsPerStrip, rainbowColors[index]);
            break;
           case 1:
        leds.setPixel(x + y*ledsPerStrip, rainbowColorsHalf[index]);
            break;
          case 2:
        leds.setPixel(x + y*ledsPerStrip, rainbowColorsQuarter[index]);
           break;
          case 3:
        leds.setPixel(x + y*ledsPerStrip, rainbowColorsLow[index]);
            break;
          case 4:
        leds.setPixel(x + y*ledsPerStrip, rainbowColorsVeryLow[index]);
            break;
            case 5:
        leds.setPixel(x + y*ledsPerStrip, 0);
        break;
        }
      }
    }
    leds.show();
    digitalWrite(1, LOW);
  }
}


// Two daisychained 74HC595N shift registers steer 16 LEDs. The pattern of the leds is given in the initialiser of the global variable named 'leds'.
// After latching, the MSB is in the first shift register and the LSB in the second.
// The LEDs light up very much like the famous Night Rider's KITT car. When the pattern hits the final or first LED, the direction of the shifting
// changes from up to down or vice versa.
// The shift register attached to the Arduino gets the LSB first, and shifts it to the second when the MSB is shifted in it.
// Only after that the latch is done.
// Pin 4 is the data pin. It goes into pin 14 of the first 74HC595N named DS.
// Pin 5 is the latch pin. It goes into pins 12 of both shift registers named RCLK.
// Pin 6 is the clock pin. It goes into pins 11 of both shift registers named CRCLK.
// To daisychain the two 74HC595N, connect pin 9 named Q_H' of the first to pin 14 named DS on the second shift register.
// The output pins of the shift registers is fed to the 16 LEDs via 1K resistors.
//
// So far, there is a previous project doing this. I realised that the shift registers can be used for multiplexing too.
// The current code uses the second shift register to multiplex one character of a 7 segment LED display on a common annode LED display; the
// first shift register determines which LEDs of the 7 segments are lit.
// The code has some extra looping using a 'test' variable. Without this the MSB character is much brighter than the others because it is given
// much more time to light up; the code is lighting the display in very quick succession to give the impression of continuous lighting.
// The output of the second shift register is used to drive four transistors which may or may not allow current to flow to ground based, closing
// the circuit. The output of the first shift register flows through a current limiting resistor of 1K, the original array of 16 stand-alone LEDs
// and the display LEDs are in parallel with the standalone LEDs.
// The 4 character 7 segment display shows a counter going up from zero to 9999, starting over again at that point.
// The value of 1 is displayed as 0001 but this is changed easily to a blank.
//
// I have run this code on an Arduino UNO, but it is likely to work on other Arduino devices with little changes. Feel free
// to share your experiences.
// Happy coding!
// Kind regards, Go4it from Ottersoft
// January 9th 2016

int gDataPin = 4;
int gLatchPin = 5;
int gClockPin = 6;

char gToSerial[512] = {0};

int gDelayCount1 = 0;
int gDisplayCounter = 0;
char gDisplayString[5] = {0};

byte gDigitFont[10] =
  {
    B11111100, // 0
    B01100000, // 1
    B11011010, // 2
    B11110010, // 3
    B01100110, // 4
    B10110110, // 5
    B10111110, // 6
    B11100000, // 7
    B11111110, // 8
    B11110110  // 9
  };

byte GetMsb(int aData)
{
  return (aData >> 8) & 0xFF;
}
 
byte GetLsb(int aData)
{
  return aData & 0xFF;
}
 
void setup() 
{
  pinMode(gLatchPin, OUTPUT);
  pinMode(gDataPin,  OUTPUT);  
  pinMode(gClockPin, OUTPUT);

  Serial.begin(57600);
  sprintf(&gDisplayString[0], "%04i", gDisplayCounter);
}
 
void loop() 
{
  gToSerial[0] = 0;

  gDelayCount1++;
  
  if (gDelayCount1 > 3)
  {
    gDelayCount1 = 0;
    gDisplayCounter = (gDisplayCounter + 1) % 10000;
    
    sprintf(gDisplayString, "%04i", gDisplayCounter);
  }

  sprintf(&gToSerial[strlen(gToSerial)], " gDisplayCounter= %i gDisplayString= %s gDelayCount1= %8i\r\n",
    gDisplayCounter, gDisplayString, gDelayCount1);

  for(int test = 0; test < 100; test++)
  for(int element = 0; element < 4; element++)
  {
    int digit= gDisplayString[3-element] - '0';
    byte digitBits = gDigitFont[digit];
    
    updateShiftRegister((digitBits << 8) | (1 << element));
  }

  Serial.print(&gToSerial[0]);
}
 
void updateShiftRegister(int aInput)
{
   digitalWrite(gLatchPin, LOW);
   shiftOut(gDataPin, gClockPin, LSBFIRST, GetLsb(aInput));
   shiftOut(gDataPin, gClockPin, LSBFIRST, GetMsb(aInput));

   //sprintf(&gToSerial[strlen(gToSerial)], " MSB= 0x%02X", GetMsb(aInput));
   //sprintf(&gToSerial[strlen(gToSerial)], " LSB= 0x%02X", GetLsb(aInput));
   
   digitalWrite(gLatchPin, HIGH);
}


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
// I have run this code on an Arduino UNO, but it is likely to work on other Arduino devices with little changes. Feel free
// to share your experiences.
// Happy coding!
// Kind regards, Go4it from Ottersoft
// January 8th 2016

int gDataPin = 4;
int gLatchPin = 5;
int gClockPin = 6;

unsigned int gLeds = 0b1110000111;
bool gShiftUp = true;
char gToSerial[1024] = {0};
unsigned int gBounce = (1<<15) | 1;

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
}
 
void loop() 
{
  updateShiftRegister(gLeds);

  gLeds = gShiftUp ? (gLeds << 1) : (gLeds >> 1);
 
  if ((gLeds & gBounce) != 0)
  {
    gShiftUp = !gShiftUp;
  }

  delay(300);
  sprintf(&gToSerial[0], "gLeds= 0x%0X gShiftUp= %i MS_bit= %i, LS_bit=%i gBounce= %u\r\n", gLeds, gShiftUp, (gLeds & (1 << 3)) ? 1:0, (gLeds & 1) ? 1:0, gBounce);
  Serial.print(&gToSerial[0]);
}
 
void updateShiftRegister(int aInput)
{
   digitalWrite(gLatchPin, LOW);
   shiftOut(gDataPin, gClockPin, LSBFIRST, GetLsb(aInput));
   shiftOut(gDataPin, gClockPin, LSBFIRST, GetMsb(aInput));

   sprintf(&gToSerial[strlen(gToSerial)], " MSB= 0x%02X", GetMsb(aInput));
   sprintf(&gToSerial[strlen(gToSerial)], " LSB= 0x%02X", GetLsb(aInput));
   
   digitalWrite(gLatchPin, HIGH);
}


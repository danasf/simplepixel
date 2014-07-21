/**
* Simple WS2812B / Neopixel Driver
*
*
* Dana Sniezko & Alex
* Coded at Hacker School (http://www.hackerschool.com), Summer 2014
* MIT License
*
*
**/

#define NUM_LEDS 15        // number of LEDs
#define BYTES (NUM_LEDS*3) // each LED has 3 color channels, NUM_LED*3
#define PORT PORTD         // PORTD, see http://www.arduino.cc/en/Reference/PortManipulation
#define LEDPIN PORTD6      // pin 6 on Arduino
#define DEBUG TRUE         // Debug is on

uint8_t* leds = NULL;

void setup() { 

  if(DEBUG) {  Serial.begin(9600); }
  delay(1000);                      // wait a second! 
  DDRD = DDRD | B01000000;          // set pin 6 on PortD to output
  leds = (uint8_t*) malloc(BYTES);  // allocate a memory block, great!
  memset(leds,0,BYTES);             // zero out this memory space 

}


void loop() {
  
 // while x < number of LEDS
 for (int x=0; x < NUM_LEDS; x++) {
  setColor(x,0,255,0); 

  updateStrip();
  debug(x); 
  delay(30);  
}
for (int x=0; x < NUM_LEDS; x++) {
  uint8_t *p = leds + x*3;
  setColor(x,255,0,0); 
  updateStrip();
  debug(x); 
  delay(30); 
}

for (int x=0; x < NUM_LEDS; x++) {
  uint8_t *p = leds + x*3;
  setColor(x,0,0,255); 

  updateStrip();
       // debug
  Serial.print(x);
  Serial.print(":");
  Serial.print(*p++);
  Serial.print(",");
  Serial.print(*p++);
  Serial.print(",");
  Serial.println(*p);  
  delay(30);  
  
}

}


// make a color from RGB values
void setColor(uint16_t pixel,uint8_t r, uint8_t g, uint8_t b) {
  uint8_t *p = leds + pixel*3;
  *p++ = r;
  *p++ = g;
  *p = b;
}

// debug function, prints out buffer at position x
void debug(uint16_t x) {
  uint8_t *p = leds + x*3;
  Serial.print(x);
  Serial.print(":");
  Serial.print(*p++);
  Serial.print(",");
  Serial.print(*p++);
  Serial.print(",");
  Serial.println(*p);

}

void updateStrip() {
  noInterrupts(); // disable interrupts, we need fine control
  volatile uint8_t *p = leds; // pointer
  volatile uint8_t current = *p++; // get current byte, point to next byte
  volatile uint8_t high = PORT | _BV(LEDPIN); // high = PORTD6 is high 
  volatile uint8_t low =  PORT & ~_BV(LEDPIN); // all are low
  volatile uint8_t temp = low; // start temp at low
  volatile uint8_t bits = 8; // 8 bits per color chanel
  volatile uint8_t bytes = BYTES; // bytes in data

    /**
    * To send a 1, we need:
    * 13 cycles on, 7 off
    *
    * To send a 0, we need:
    * 6 cycles on, 14 off 
    **/
    asm volatile(
    "sendbit:"  "\n\t"               // Label for sending a bit
    "st %a[portptr],%[high]"  "\n\t" // 2 cycles, set port HIGH (T=2), needed for sending either 0 or 1
    "sbrc %[cur],7" "\n\t"           // if MSB a 1, do the next instruction, else skip (1-2 cycle) (T=4)
    "mov %[temp],%[high]" "\n\t"     // move high into temp, 1 cycle (T=4)
    "dec %[bits]" "\n\t"             // decrement bit count, 1 cycle (T=5)
    "nop" "\n\t"                     // do nothing for 1 cycle (T=6)
    "st %a[portptr],%[temp]"  "\n\t" // move temp val into port, 2 cycles (T=8)
    "mov %[temp],%[low]"  "\n\t"     // move low into temp, 1 cycle (T=9)
    "breq newbyte"  "\n\t"           // if bit count is 0, go to next byte, 1 or 2 cycles? (T=10)
    "rol %[cur]"  "\n\t"             // shift current byte left, 1 cycle (T=11)
    "nop" "\n\t"                     // do nothing for 1 cycle (T=12)
    "nop" "\n\t"                     // do nothing for 1 cycle (T=13)
    "st %a[portptr],%[low]" "\n\t"   // 2 cycles, sets port LOW (T=15)
    "nop" "\n\t"                     // do nothing for 1 cycle (T=16)
    "nop" "\n\t"                     // do nothing for 1 cycle (T=17)
    "nop" "\n\t"                     // do nothing for 1 cycle (T=18)
    "rjmp sendbit"  "\n\t"           // jump back to the bit label (T=20)
    "newbyte:"  "\n\t"               // label for next byte (T=10)
    "ldi %[bits],8" "\n\t"           // reset bit count to 8 bits (T=11)
    "st %a[portptr],%[low]" "\n\t"   // 2 cycles, sets PORT to LOW (T=12)
    "dec %[bytes]"  "\n\t"           // remove 1 from byte count, 1 cycle (T=13)
    "ld  %[cur],%a[p]+" "\n\t"       // advance the pointer, load as current byte, 2 cycles (T=15)
    "nop" "\n\t"                     // do nothing for 1 cycle 1 (T=16)
    "nop" "\n\t"                     // do nothing for 1 cycle 1 (T=17)
    "nop" "\n\t"                     // do nothing for 1 cycle 1 (T=18)
    "brne sendbit"  "\n\t"           // (T=20) if byte count is not 0, send another bit!
    ::
    // input operands
    // https://stackoverflow.com/questions/57483/what-are-the-differences-between-pointer-variable-and-reference-variable-in-c/57492#57492
    [portptr] "e" (&PORT),
    [bits] "r" (bits),
    [bytes] "r" (bytes),
    [temp] "r" (temp),
    [high] "r" (high),
    [low] "r" (low),
    [p] "e" (p),
    [cur] "r" (current)
    );
    delayMicroseconds(60); // latch data
    interrupts(); // we can enable interrupts again
}
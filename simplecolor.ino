/**
*  Simple WS2812B / Neopixel Driver
*  Dana Sniezko & Alex Segura
*  Coded at Hacker School (http://www.hackerschool.com), Summer 2014
*
*  General resources:
*  http://www.avr-asm-tutorial.net/avr_en/beginner/
*  http://www.avrbeginners.net/
*  http://www.arduino.cc/
*  https://learn.adafruit.com/adafruit-neopixel-uberguide/overview
**/

#define NUM_LEDS 15        // number of LEDs
#define BYTES (NUM_LEDS*3) // each LED has 3 color channels, NUM_LED*3
#define PORT PORTD         // PORTD, see http://www.arduino.cc/en/Reference/PortManipulation
#define LEDPIN PORTD6      // pin 6 on Arduino
#define DEBUG true         // Debug is on

uint8_t* leds = NULL;

void setup() { 

  if(DEBUG) {  Serial.begin(9600); }
  delay(1000);                      // wait a second! 
  DDRD = DDRD | B01000000;          // set pin 6 on PortD to output
  leds = (uint8_t*) malloc(BYTES);  // allocate a memory block, great!
  memset(leds,0,BYTES);             // zero out this memory space 

}


void loop() {
 seqFill(255,0,0,30); // seq fill red
 seqFill(0,255,0,30); // seq fill green
 seqFill(0,0,255,30); // seq fill blue
 scanner(255,0,0,40); // larson scanner
 sine(0.05,10);
 //rainbow(0.01,10); // rainbows .... to implement  
}

// sequential fill
void seqFill(uint8_t r, uint8_t g, uint8_t b, uint8_t wait) {
  // loop through
  for (int x=0; x < NUM_LEDS; x++) {
    if(DEBUG) {  debug(x); }
    setColor(x,r,g,b); 
    updateStrip();
    delay(wait); 
  }
}

// quick fill
void quickFill(uint8_t r, uint8_t g, uint8_t b, uint8_t wait) {
  // loop through
  for (int x=0; x < NUM_LEDS; x++) {
    if(DEBUG) {  debug(x); }
    setColor(x,r,g,b); 
  }
  updateStrip();
  delay(wait); 
}

// Larson Scanner
void scanner(uint8_t r, uint8_t g, uint8_t b, uint8_t wait) {
  for (int x=0; x < NUM_LEDS; x++) {
    clear();
    setColor(x,r,g,b);
    updateStrip();
    delay(wait); 
  }
  for (int x=NUM_LEDS; x > 0; x--) {
    clear();
    setColor(x,r,g,b);
    updateStrip();
    delay(wait); 
  }
}

// Sine wave
void sine(float rt, uint8_t wait) {
  float in,r;
  for(in = 0; in < PI*2; in = in+rt) {
    for(int i=0; i < NUM_LEDS; i++) {
      // 3 offset sine waves make a rainbow
      r = sin(i+in) * 127 + 128;
      setColor(i,(uint8_t)r,0,0);
    }
    updateStrip();
    delay(wait);
  }
}

// blank out strip buffer
void clear() {
  memset(leds,0,BYTES);  
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

// make a color from RGB values, these strips are wired GRB so store in that order
void setColor(uint16_t pixel,uint8_t r, uint8_t g, uint8_t b) {
  uint8_t *p = leds + pixel*3;
  *p++ = g;
  *p++ = r;
  *p = b;
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
  "sendbit:"  "\n\t"                 // Label for sending a bit
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
  "newbyte:"  "\n\t"                 // label for next byte (T=10)
    "ldi %[bits],8" "\n\t"           // reset bit count to 8 bits (T=11)
    "st %a[portptr],%[low]" "\n\t"   // 2 cycles, sets PORT to LOW (T=12)
    "dec %[bytes]"  "\n\t"           // remove 1 from byte count, 1 cycle (T=13)
    "ld  %[cur],%a[p]+" "\n\t"       // advance the pointer, load as current byte, 2 cycles (T=15)
    "nop" "\n\t"                     // do nothing for 1 cycle 1 (T=16)
    "nop" "\n\t"                     // do nothing for 1 cycle 1 (T=17)
    "nop" "\n\t"                     // do nothing for 1 cycle 1 (T=18)
    "brne sendbit"  "\n\t"           // (T=20) if byte count is not 0, send another bit!
  : 
  // outputs NONE
  :
  // inputs 
  // helpful resource: http://www.nongnu.org/avr-libc/user-manual/inline_asm.html
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
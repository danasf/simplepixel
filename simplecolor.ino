#define NUM_LEDS 15 // number of LEDs
#define BYTES (NUM_LEDS*3)
#define BITS 8 // number of bits per color
#define PORT PORTD
#define LEDPIN PORTD6 


uint8_t* leds = NULL;
void setup() { 
  Serial.begin(9600);
   // wait a second! 
  delay(1000);
   // set pin 6 on PortD to output
  DDRD = DDRD | B01000000;

  if(leds = (uint8_t*) malloc(BYTES)) {
    memset(leds,0,BYTES); 
  }
}


void loop() {
  
 for (int x=0; x < NUM_LEDS; x++) {
  uint8_t *p = leds + x*3;
  setColor(x,0,255,0); 

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
for (int x=0; x < NUM_LEDS; x++) {
  uint8_t *p = leds + x*3;
  setColor(x,255,0,0); 

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

// cycle through pixels and update
void updateStrip() {
  noInterrupts();
  // for each led in array
  volatile uint8_t *p = leds;
    volatile uint8_t val = *p++; // get val, point to next byte
    volatile uint8_t high = PORT | _BV(LEDPIN); 
    volatile uint8_t low =  PORT & ~_BV(LEDPIN);
    volatile uint8_t temp = low;
    volatile uint8_t bits = BITS;
    volatile uint8_t bytes = BYTES;

        // send a 1
        // 13 cycles on, 7 off
        // send a 0
        // 6 cycles on, 14 off
    asm volatile(
    "sendbit:\n\t" // Label for sending a bit
    "st %a[portptr],%[high]\n\t" // 2 cycles, set pin HIGH (T=2)
    "sbrc %[cur],7\n\t" // is MSB a 1 (1-2 cycle) (T=4)
    "mov %[temp],%[high]\n\t" // 1 cycle (T=4)
    "dec %[bits]\n\t" // 1 cycle (T=5)
    "nop\n\t"         // 1 cycle (T=6)
    "st %a[portptr],%[temp]\n\t" // 2 cycle (T=8)
    "mov %[temp],%[low]\n\t" // 1 cycle (T=9)
    "breq newbyte\n\t" // 1 or 2 cycles, if bit == 0, go to new byte (T=?)
    "rol %[cur]\n\t" // 1 cycle (T=11)
    "nop\n\t" // 1 (T=12)
    "nop\n\t" // 1 (T=13)
    "st %a[portptr],%[low]\n\t" // 2 cycles, set pin LOW (T=15)
    "nop\n\t" // 1 (T=16)
    "nop\n\t" // 1 (T=17)
    "nop\n\t" // 1 (T=18)
    "rjmp sendbit\n\t" // 2, otherwise jump back to the bit (T=20)
    "newbyte:\n\t" // label for next byte (T=10)
    "ldi %[bits],8\n\t" // reset bit count to 8 bits (T=11)
    "st %a[portptr],%[low]\n\t" // 2 cycles, set pin LOW (T=12)
    "dec %[bytes]\n\t" // remove 1 from byte count (T=13)
    "ld  %[cur],%a[p]+\n\t" //2 cycles (T=15)
    "nop\n\t" // 1 (T=16)
    "nop\n\t" // 1 (T=17)
    "nop\n\t" // 1 (T=18)
    "brne sendbit\n\t" // (T=20) if byte count is not 0, send another bit
    ::
    // input operands
    [port] "I" (_SFR_IO_ADDR(PORT)),
    [pin] "I" (LEDPIN),
    [portptr] "e" (&PORT),
    [bits] "r" (bits),
    [bytes] "r" (bytes),
    [temp] "r" (temp),
    [high] "r" (high),
    [low] "r" (low),
    [p] "e" (p),
    [cur] "r" (val)
    );
delayMicroseconds(60);
interrupts();
}
/*
Experiments in Assembly Language and bit-banging WS2812B/Neopixel Lights
Dana Sniezko, 2014
Public Domain
*/

#define PORT PORTD
#define BITS 8
// 24 bits * number of LEDs == all to white
#define NUM_BITS 24*15
#define LEDPIN PORTD6

void setup() {

   // wait a second!
   delay(1000);

   // set bit 6 (aka Arduino pin 6) on PortD to output
   DDRD = DDRD | B01000000;

}

void loop() {
    // if you uncomment the for loops this should make 1 LED turn blue

    // no interrupts, we need fine control over timing
    noInterrupts();


    /*
    Sending a 1
    0.80uS HIGH, 0.45uS LOW
    we need ~13 cycles of HIGH
    and ~7 cycles of LOW
    */
    asm volatile(
    "sendbit:\n\t" // Label for sending a bit
    "sbi %0,%1\n\t" // 2 cycles, set pin HIGH
    "rjmp .+0\n\t" //  2 cycles,don't do anything
    "rjmp .+0\n\t" //  2 cycles, don't do anything
    "rjmp .+0\n\t" //  2 cycles, don't do anything
    "rjmp .+0\n\t" //  2 cycles, don't do anything
    "rjmp .+0\n\t" //  2 cycles, don't do anything
    "nop\n\t" // don't do anything, ON for 0.813 uS
    "cbi %0,%1\n\t" // 2 cycles, set pin LOW
    "rjmp .+0\n\t" //  2 cycles, don't do anything 
    "dec %2\n\t" // 1 cycle, subtract 1 from bit
    "breq newbyte\n\t" // 1 or 2 cycles, if bit == 0, go to new byte
    "rjmp sendbit\n\t" // 2, otherwise jump back to the bit
    "newbyte:\n\t" // label for next byte
    "ldi %2,8\n\t" // reset bit count to 8 bits 
    "dec %3\n\t" // remove 1 from byte count
    "brne sendbit\n\t" // if byte count is not 0, send another bit
    ::
    // input operands
    "I" _SFR_IO_ADDR((PORT)),
    "I" (LEDPIN),
    "r" (BITS),
    "r" (NUM_BITS)
    );
    // delay > 50 microseconds to LATCH!
    delayMicroseconds(60);
    
    // okay, we're done, can enable interrupts again
    interrupts();
    
}

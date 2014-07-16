  /* 
    Experiments in Assembly Language and bit-banging WS2812B/Neopixel Lights
    
    Dana Sniezko, 2014 
    Public Domain

    If you need lights:
    https://www.adafruit.com/products/1138
    
    If you're looking for a full-featured LED library try Adafruit Neopixel or FastLED!
    
    Arduinos typically run at 16Mhz, one clock-instruction every 0.0625 uS
    
    WS2812B datasheet shows these lights run at 800Khz. 
    http://www.mikrocontroller.net/attachment/180459/WS2812B_preliminary.pdf
    
    Each LED pixel is wired wired Green-Red-Blue. We need 20 Arduino instruction cycles to pass 1 bit of data to them!
    
    Sending a 0
    0.40uS HIGH, 0.85uS LOW
    ~6 cycles of HIGH
    and ~14 cycles of LOW
    
    Sending a 1
    0.80uS HIGH, 0.45uS LOW
    We need ~13 cycles of HIGH
    and ~7 cycles of LOW
    
    These values have +/-0.15uS tolerance.
    
    To latch values
    50.00 uS+ LOW

  */

  /* 
    We're using PORTD, see these resources for more about ports and port manipulation
    http://arduino.cc/en/Hacking/PinMapping168
    http://www.arduino.cc/en/Reference/PortManipulation
  */
  #define PORT PORTD

  /* 
    LED pin, bit 6 of PORTD
    also Digital Pin 6 in Arduino
  */
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
    
    // send 16 bits of 0s, 0
    //  for(int i=0; i < 8*2; i++) {

      /*
      Sending a 0
      0.40uS HIGH, 0.85uS LOW
      ~6 cycles of HIGH
      and ~14 cycles of LOW
      */
      
     /* GCC-AVR inline assembly, OMG, super useful resource:
        http://www.nongnu.org/avr-libc/user-manual/inline_asm.html
        Also, Amtel ASM guide http://www.atmel.com/Images/doc1022.pdf
        You could use rjmp .+0 instead of nop to make this less verbose
     */
     asm volatile(
     "sbi %0,%1\n\t" // 2 cycles, SET pin HIGH
     "nop\n\t"       // don't do anything 
     "nop\n\t"       // don't do anything
     "nop\n\t"       // don't do anything 
     "nop\n\t"       // don't do anything, ON for 0.375 uS
     "cbi %0,%1\n\t" // 2 cycles LOW , SET pin LOW
     "nop\n\t"      // don't do anything for 12 cycles
     "nop\n\t"      // don't do anything 
     "nop\n\t"      // don't do anything 
     "nop\n\t"      // don't do anything 
     "nop\n\t"      // don't do anything 
     "nop\n\t"      // don't do anything 
     "nop\n\t"      // don't do anything 
     "nop\n\t"      // don't do anything 
     "nop\n\t"      // don't do anything 
     "nop\n\t"      // don't do anything 
     "nop\n\t"      // don't do anything 
     "nop\n\t"      // don't do anything 
     "nop\n\t"      // don't do anything 
     "nop\n\t"      // don't do anything, OFF for 0.875 uS
     :: 
      // inputs, the port and led pin
     "I" _SFR_IO_ADDR((PORT)),
     "I" (LEDPIN)  
     );
   // }

    // send 8 bits of 1, aka 255
   // for(int i=0; i < 8*1; i++) {

    /*
    Sending a 1
    0.80uS HIGH, 0.45uS LOW
    we need ~13 cycles of HIGH
    and ~7 cycles of LOW
    */
    asm volatile(
    "sbi %0,%1\n\t" // 2 cycles, set pin HIGH
    "nop\n\t"       // don't do anything for 11 cycles
    "nop\n\t"       // don't do anything
    "nop\n\t"       // don't do anything 
    "nop\n\t"       // don't do anything
    "nop\n\t"       // don't do anything
    "nop\n\t"       // don't do anything
    "nop\n\t"       // don't do anything
    "nop\n\t"       // don't do anything
    "nop\n\t"       // don't do anything
    "nop\n\t"       // don't do anything
    "nop\n\t"       // don't do anything, ON for 0.813 uS
    "cbi %0,%1\n\t" // 2 cycles, set pin LOW
    "nop\n\t"       // don't do anything for 5 cycles
    "nop\n\t"       // don't do anything
    "nop\n\t"       // don't do anything 
    "nop\n\t"       // don't do anything
    "nop\n\t"       // don't do anything, OFF for 0.438 uS
    :: 
    // input operands
    "I" _SFR_IO_ADDR((PORT)),
    "I" (LEDPIN)  
    );
  //  }
    // delay > 50 microseconds to LATCH!
    delayMicroseconds(100);
    
    // okay, we're done, can enable interrupts again
    interrupts();
    
  }
Simple Pixel
==========
Experiments with hand-tuned ASM on Arduino with addressable LEDs.

Written at [Hacker School](http://www.hackerschool.com/), Summer 2014.

This library attempts to control popular WS2812B (aka [Neopixel](https://www.adafruit.com/search?q=neopixel) LEDs). It has been tested to work on 16Mhz ATMega168 and ATMega328 based Arduino boards. Use Digital Pin 6.

If you're looking for a more flexible library, please check out AdaFruit Neopixel or FastLED!

=======

* simple.ino, really, really simple. Looking at timing, send a 0 or 1.
* simplecolor.ino, send rgb color values


Learning Resources
=======

AVR / Arduino

* [AVR ASM tutorials](http://www.avr-asm-tutorial.net/avr_en/beginner/)
* [AVR Beginners](http://www.avrbeginners.net/)
* [Inline AVR ASM](http://www.nongnu.org/avr-libc/user-manual/inline_asm.html)
* [Arduino Home](http://www.arduino.cc/)

Neopixels / WS2812B LEDs

* [Adafruit Neopixel Uberguide](https://learn.adafruit.com/adafruit-neopixel-uberguide/overview) and [Adafruit Neopixel](https://github.com/adafruit/Adafruit_NeoPixel) library
* [Bit-banging Neopixels](http://www.instructables.com/id/Bitbanging-step-by-step-Arduino-control-of-WS2811-/)
* [Advanced Secrets of Neopixels](http://wp.josh.com/2014/05/13/ws2812-neopixels-are-not-so-finicky-once-you-get-to-know-them/)
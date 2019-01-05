# ATTINY85 RC Receiver Signal Decoder

[![GitHub](https://img.shields.io/github/license/mashape/apistatus.svg)](https://en.wikipedia.org/wiki/MIT_License)

An RC receiver signal decoder using an [AVR ATTINY85 microcontroller](https://www.microchip.com/wwwproducts/en/ATtiny85). The decoder uses half of the 8-bit resolution for the 1ms between minimum (1ms) and maximum (2ms) RC receiver pulse.

## Features
  - To resolve the PWM pulse pin change interrupts are used to be able to do timing between rising and falling edge of the RC receiver signal
  - Timer1 is used with a prescaler of 64 (CK/64) which results in timesteps of 8 microseconds. Thus, for the 2 milliseconds pulse 250 steps are available
  - Since the signal operates only within half of that period the resulting resolution is approx. half of the 8-bit (modern RC controllers operate at 12-bit stick resolution, so still some work needs to be done here)
  - An LED attached to LED_PORT indicates the RC control stick position (use a 200&Omega; resistor for the LED)
    - The function *LED_Control(60)* would light the LED if RC control stick of the corresponding channel is at 60% or more
  - The pins for receiver and LED can be configured by the corresponding macros (RC_RECEIVER_PORT, LED_PORT). Do not use VCC, GND and RESET pins
  - Oscillator calibration:
    - The factory calibration for one specific ATTINY85 was 148 (0x94 read from chip via ATMEL STUDIO "Device Programming")
    - This setup gave a measured frequency of 8,196MHz which is slightly more than 2% off the target speed
    - Using the OSCCAL register the clock speed can be calibrated
    - The code currently uses *OSCCAL = 142;* which resulted in 7,96MHz and peaks of 8MHz

## Example Breadboard Configuration

Obviously, before use, the code **main.c** needs to be programmed on the device via any ISP programmer.

![](images/Fritzing_Layout.png)

## Prerequisites
 - For the code to work the CPU needs to run at 8 MHz (this is NOT the factory default)
 - The fuse **CKDIV8** has to be unset, otherwise the CPU would run at 1 MHz

<!--- HTML code used in order to be able to resize image -->
<img src="images/Fuse_Settings.png" alt="drawing" width="600"/>

**Setting of fuses in ATMEL Studio 7 - CKDIV8 unchecked for 8 MHz clock speed**

## Additional information
 - Official MICROCHIP [ATTINY85 datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-2586-AVR-8-bit-Microcontroller-ATtiny25-ATtiny45-ATtiny85_Datasheet.pdf)

<br></br>

Distributed under the MIT license. See [LICENSE](https://github.com/chiefenne/ATTINY85-Servo-Control/blob/master/LICENSE) for more information.



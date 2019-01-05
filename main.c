/*
 * ATTINY85_RC_Receiver.c
 *
 * Created: 01.01.2019 20:06:29
 * Author : Andreas
 * Description: Read and interpret signals from RC Receivers
 */ 

// the clock speed on ATTINY85 is approx. 8MHz
// if fuse CKDIV8 is set (factory default), a prescaler of 8 is used which results in a 1MHz clock
// for this code CKDIV8 needs to be unset as the code relies on 8MHz CPU speed
// the actual frequency can be measured at PB4 if the CKOUT fuse is set


//             ATTINY85 PIN Configuration (for more details see datasheet page 2)
//                  -------
//  RESET / PB5 ---| o     |--- VCC
//   ADC3 / PB3 ---|       |--- PB2 / SCK
//          PB4 ---|       |--- PB1 / MISO
//          GND ---|       |--- PB0 / MOSI
//                  -------

// includes
#include <avr/io.h>
#include <avr/interrupt.h>


// *****************************
// ***** global variables ******
// *****************************

volatile uint8_t pulse_ready = 1;
volatile uint8_t count;

#define LED_PORT PB4
#define RC_RECEIVER_PORT PB0

#define LED_ON PORTB |= (1 << LED_PORT)
#define LED_OFF PORTB &= ~(1 << LED_PORT)

#define RISING_EDGE PINB & (1 << RC_RECEIVER_PORT)

void Calibrate_OSCILLATOR(void){

    // function for calibrating the oscillator frequency
    
    // this program and corresponding timer setup requires that overflow to happens
    // therefore it is required to stay below 8MHz nominal frequency
    // specifically if the frequency is higher than 8MHz unwanted overflows would occur and destroy the software intend
    
    // Factory calibration for this very ATTINY85 was 148 (0x94 read from chip via ATMEL STUDIO "Device Programming")
    // this gave a measured frequency of 8,196MHz
    // frequency was measured with oscilloscope via PB4 (CKOUT fuse needs to be set for this)
    
    // OSCCAL needs to be calibrated per chip
    OSCCAL = 142;                            // this gave 7,96MHz (peaks of 8MHz) for value ranges see datasheet page 52
}

void Init_PORT(void) {
    
    // this function initializes the pins which are used as output

    DDRB |= (1 << LED_PORT);                 // set led port as output
    DDRB &= ~(1 << RC_RECEIVER_PORT);        // set receiver port as input

}

void Init_INTERRUPTS(){
    
    // this function initializes the interrupts
    
    PCMSK |= (1 << PCINT0);                  // Pin Change Enable Mask for PB0 (pin 5) (datasheet page 52)
    
    sei();                                   // enable interrupts (MANDATORY)

}

void LED_Control(float position){
    
    // this function switches the LED on depending on RC signal
    // position variable indicates 0%-100% stick position
    
    float loc;

    loc = position / 100 * 125;
    
    if ( count - 125 >= (int)loc ) {
        LED_ON;
    }
    else {
        LED_OFF;
    }
}


int main(void)
{

    Calibrate_OSCILLATOR();
    Init_PORT();
    Init_INTERRUPTS();

    while (1)
    {
                
        if ( pulse_ready) {
            
            pulse_ready = 0;

            LED_Control(10);                 // percent RC stick when LED shall switched on
            
            GIFR = (1 << PCIF);              // clear Pin Change Interrupt Flag  (datasheet page 52)
            GIMSK |= (1 << PCIE);            // Pin Change Interrupt Enable (datasheet page 51)

        }

    }
}

ISR(PCINT0_vect){

    // interrupt service routine for pin change interrupt

    // if PINB is HIGH, a rising edge interrupt has happened
    // if PINB is LOW, a falling edge interrupt has happened

    if ( RISING_EDGE ){       // check if rising edge pin change interrupt (beginning of servo pulse)
        
        TCNT1 = 0;                           // reset counter
        TCCR1 = (1 << CS12) | (1 << CS11) | (1 << CS10);   // start timer1 with prescaler CK/64 --> 250 steps per 2ms
        
        return;
        
    }

    // only reached when falling edge detected (end of servo pulse)
    count = TCNT1;                           // take timer value to local variable
    TCCR1 = 0;                               // stop timer
    GIMSK &= ~(1 << PCIE);                   // Pin Change Interrupt Disable (datasheet page 51)
    
    pulse_ready=1;

}


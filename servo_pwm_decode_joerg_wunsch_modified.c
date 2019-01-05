
/*
 * This is the main C source file for the demo.
 */
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include <avr/sleep.h>

volatile uint16_t pwm_incoming;
volatile uint8_t pwm_received = 1;

# define F_CPU 1000000ul

/* Minimal PWM pulse width is 920 us. */
# define MIN_PWM_VAL ((920ul * F_CPU) / 16 / 1000000ul)
/* Maximal PWM pulse width is 2120 us */
# define MAX_PWM_VAL ((2120ul * F_CPU) / 16 / 1000000ul)

void
ioinit(void)
{

  PCMSK = (1 << 4);
  GIFR = (1 << PCIF);  // clear flag
  GIMSK = (1 << PCIE);

  DDRB = (1 << PB1);
  PORTB = 0;

  sei();
}

ISR(PCINT0_vect)
{
  uint8_t tcnt1;

  if (PINB & (1 << 4))
    {
      /* Start timer 1 with a prescaler of 16. */
      TCNT1 = 0;
      TCCR1 = (1 << CS12) | (1 << CS10);
      return;
    }

  /* Stop timer 1, current value is pulse width. */
  tcnt1 = TCNT1;
  TCCR1 = 0;
  GIMSK &= ~(1 << PCIE);

  pwm_incoming = tcnt1;
  pwm_received = 1;
}


int
main(void) {

    ioinit();

    while (1) {
        if (pwm_received) {
            
	        pwm_received = 0;
      
	        GIFR = (1 << PCIF);   // clear flag
	        GIMSK |= (1 << PCIE);
	    }
    }
}
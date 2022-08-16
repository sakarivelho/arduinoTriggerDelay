
/**************************************
      SET DELAY & EXTENSION HERE
***************************************/
/**************************************/
/**************************************/
/**************************************/
//Pulsedelayer
#define PULSEDELAY 50 //Delay for output 1 [ms]
#define DELAYEDPULSELENGTH 10 // Pulse length of output 1 [ms]

//Pulse extension
#define PULSEEXTENSION 500 // Extension for output 2 [ms]
/***************************************/
/**************************************/
/**************************************/
/**************************************/

const uint8_t inputPin = 2;
//Trigger input: D2
//Trigger output (delayed): D8
//Trigger output (extended): D6

//Ticks to millisecond for timer2. Actual value 249 but arduino clone oscilator seems to be off by .4% Might vary between models, with crystal oscillator value should be always 249
#define TIMER2TICKSTOMILLIS 248

volatile bool trigged = false;
volatile bool extensionTrigged = false;
volatile uint16_t timer2InterruptCount = 0;
//const uint16_t t1_load = 0;
//const uint16_t t1_comp = (MILLISECOND * PULSEDELAY)-1;

void setup() {
  //Disable global interrupts
  cli();
  //Set inputPin to input with pullup
  DDRD &= ~(1 << inputPin);
  PORTD |= (1 << inputPin);
  // pinMode(INPUTPIN, INPUT_PULLUP);
  //Rising edge interrupt for inputPin;
  EICRA |= (1 << ISC01);
  EICRA |= (1 << ISC00);
  // attachInterrupt(digitalPinToInterrupt(INPUTPIN), ISR_risingEdge, RISING);
  //Enable interrupts for INT0 (for inputPin)
  EIMSK |= (1 << INT0);
  //Set D8 pin to output
  DDRB = B00000001;
  //Set D8 pin low
  PORTB = B00000000;
  //Set D6 pin to output
  DDRD = B01000000;
  //Set D7 pin low
  PORTD = B00000000;

  //TIMER2 config for pulse extension
  //Reset timer2 control register A
  TCCR2A = 0;
  //Prescaler value to 64
  TCCR2B |= (1 << CS22);
  TCCR2B &= ~(1 << CS21);
  TCCR2B &= ~(1 << CS20);
  //Set timer2 compare value to equal one millisecond so we get one interrupt per millisecond
  TCNT2 = 0;
  OCR2A = TIMER2TICKSTOMILLIS;
    //turn on ctc mode
  TCCR2A |= (1 << WGM21);
  //Enable global interrupts
  sei();
}

void loop() {
  if (trigged && extensionTrigged) {
    trigged = false;
    extensionTrigged = false;
    //Disable timer2 interrupts
    TIMSK2 &= ~(1 <<  OCIE2A);
    timer2InterruptCount = 0;
    //Enable digital pin interrupt
    EIMSK |= (1 << INT0);
  }
}

//External interrupt
ISR(INT0_vect) {
  //Set d6 high
  PORTD = B01000000;
  //Start timer2 to count for pulsedelayer
  //Set timer2 count to 0
  TCNT2 = 0;
  //Clear output compare flag
  TIFR2 |= (1 << OCF2A);
  //Enable timer2 interrupts
  TIMSK2 = (1 << OCIE2A);
  //Disable digital pin interrupt
  EIMSK &= ~(1 << INT0);
}

//Timer2 interrupt
ISR(TIMER2_COMPA_vect) {
  timer2InterruptCount++;
  if ( timer2InterruptCount == PULSEEXTENSION) {
    PORTD = B00000000;
    extensionTrigged = true;
  }
  if(timer2InterruptCount == PULSEDELAY) {
    PORTB = B00000001;
    }
   if(timer2InterruptCount == PULSEDELAY + DELAYEDPULSELENGTH ) {
    PORTB = B00000000;
    trigged = true;
    }
}

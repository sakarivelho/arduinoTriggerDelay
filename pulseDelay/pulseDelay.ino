/**************************************
      SET DELAY HERE
*/

#define PULSEDELAY 50 //Value in milliseconds, min 1 max 1000


/**************************************/

#define INPUTPIN 2

//Trigger input: D2
//Trigger output: D8


//Ticks to one millisecond, clock speed 16mhz, prescaler 256
#define MILLISECOND 62.5

volatile bool trigged = false;
const uint16_t t1_load = 0;
const uint16_t t1_comp = MILLISECOND * PULSEDELAY;

void setup() {
  //Disable interrupts
  cli();

  pinMode(INPUTPIN, INPUT_PULLUP);
  //Set D8 pin to output
  DDRB = B00000001;
  //Set D8 pin low
  PORTB = B00000000;
  delay(10);
  attachInterrupt(digitalPinToInterrupt(INPUTPIN), ISR_risingEdge, RISING);
  Serial.println(t1_comp);
  //Reset timer1 control register A
  TCCR1A = 0;
  //Prescaler value to 256
  TCCR1B |= (1 << CS12);
  TCCR1B &= ~(1 << CS11);
  TCCR1B &= ~(1 << CS10);

  //Reset Timer1 and set to compare val
  TCNT1 = t1_load;
  OCR1A = t1_comp;
  //Enable interrupts
  sei();
}

void loop() {
  if (trigged) {
    trigged = false;
    //Disable digital pin interrupt
    detachInterrupt(digitalPinToInterrupt(INPUTPIN));
    delay(500);
    //Enable digital pin interrupt
    attachInterrupt(digitalPinToInterrupt(INPUTPIN), ISR_risingEdge, RISING);
    //Set D8 pin low
    PORTB = B00000000;
  }
}

//External interrupt
void ISR_risingEdge() {
  //Set timer count to 0
  TCNT1 = t1_load;
  //clear output compare a match flag so interrupt won't fire before we reach t1_load count
  TIFR1 |= (1 << OCF1A);
  //Enable Timer interrupts
  TIMSK1 = (1 << OCIE1A);
}

//Timer interrupt
ISR(TIMER1_COMPA_vect) {
  //Set D8 pin high
  PORTB = B00000001;
  trigged = true;
  //Disable Timer interrupts
  TIMSK1 &= ~(1 <<  OCIE1A);
}

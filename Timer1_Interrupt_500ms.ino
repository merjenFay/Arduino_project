void setup() {
  // put your setup code here, to run once:
  pinMode(8, OUTPUT);
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= 4;
  TCCR1B |= 1 << WGM12;
  TCNT1 = 0;
  OCR1A = 31249;
  TIMSK1 = (1 << OCIE1A);
  sei();

}

void loop() {
  // put your main code here, to run repeatedly:
  //Nothing to do here
// PORTB |= 1;
// delay(1000);
// PORTB &= 0xFE;
// delay(1000);
}

ISR(TIMER1_COMPA_vect)
{
  TCNT1 = 0;
  PORTB ^= 1;
}




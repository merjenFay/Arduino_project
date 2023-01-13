int counter = 0;
int state_counter =0;
#define ON 1
#define OFF 0
#define STATE_UNDEFINED '0'
#define STATE_ON '1'
#define STATE_OFF '2'
#define STATE_BLINKING '3'
#define WAITING_TIME 6


#define SET_PIN(x) (1<<x)
#define RESET_PIN(x) ~(1<<x)
bool led_state = OFF;


void blinking_function(int period);
void on_functionPORTB(int b);
void OFF_functionPORTB(int b);

unsigned char ProgramState = 0;




void setup() {
  // put your setup code here, to run once:
  DDRB |= SET_PIN(1);
  TCCR1A = 0;

  TCCR1B = 0;
  PORTB |= SET_PIN(1);
  led_state = ON;
  TCCR1B |= 4;
  //TCCR1B |= 1 << WGM12;
  TCNT1 = 0;
  OCR1A = 62499;
  TIMSK1 = (1 << OCIE1A);

  sei();

  /* Begin the program in ON state*/
  ProgramState = STATE_ON;

  
  
}

void loop() {
  
 switch (ProgramState)
  {
    case STATE_ON:
      {
        on_functionPORTB(1);
        if(state_counter==WAITING_TIME)
        {
          ProgramState = STATE_BLINKING;
                  state_counter=0;

        }
        break;
      }
    case STATE_OFF:
      {
        OFF_functionPORTB(1);
        if(state_counter==WAITING_TIME)
        {
          ProgramState = STATE_ON;
                  state_counter=0;

        }
        
        break;
      }
    case STATE_BLINKING:
      {
        blinking_function(1);
        if(state_counter==WAITING_TIME)
        {
          ProgramState = STATE_OFF;
                  state_counter = 0;

        }
        
        break;
      }

    default:
      {

      } break;

  }
}

ISR(TIMER1_COMPA_vect)
{
  counter++;
  state_counter++;
}

void blinking_function(int period)
{
  if ((counter >= period) && (led_state == ON))
  {
    OFF_functionPORTB(1);
  }
  else if ((counter >= (2 * period)) && (led_state == OFF))
  {
    counter = 0;
    on_functionPORTB(1);
  }
}

void on_functionPORTB(int b)
{
  PORTB |= SET_PIN(b);
  led_state = ON;
}

void OFF_functionPORTB(int b)
{
  PORTB &= RESET_PIN(b);
  led_state = OFF;
}

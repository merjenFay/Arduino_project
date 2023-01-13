/*
  1. RED_LED on for X seconds (others off)
  2. Be ready state, on for 1 second
  3. Yellow_LED ON for 1 second (others off)
  4. Green_LED ON for X seconds (others off)
  5. repeat
*/

int state_counter = 0;
int counter =0;
#define ON 1
#define OFF 0
#define STATE_RED 1
#define STATE_YELLOW 2
#define STATE_GREEN 3
#define STATE_BE_READY 4
#define BE_READY_TIME 1
#define ON_TIME 5



#define SET_PIN(x) (1<<x)
#define RESET_PIN(x) ~(1<<x)




unsigned char ProgramState = 0;


void Init_Timer();
void Green_led(void);
void Yellow_led(void);
void Red_led(void);
void be_ready(void);



void setup() {
  // put your setup code here, to run once:
  DDRB |= SET_PIN(1);
  DDRB |= SET_PIN(2);
  DDRB |= SET_PIN(3);

  /* Initialize the timer */
  Init_Timer();

  /* Begin the program in RED state*/
  ProgramState = STATE_RED;
}

void loop() 
{
  /* declare a local variable, initialized with an undefined state */



  switch (ProgramState)
  {

    case STATE_RED:
      {
        Red_led();
        if(state_counter == ON_TIME)
        {
            ProgramState = STATE_BE_READY;
            state_counter =0;
        }
        break;  
      }
      

    

      case STATE_BE_READY:
      {
        be_ready();
        if(state_counter == BE_READY_TIME )
        {
            ProgramState = STATE_GREEN;
            state_counter =0;
        }
        break;  
        
      }
     

      

    case STATE_GREEN:
      {
          Green_led();
          
          if(state_counter == ON_TIME )
          {
              ProgramState = STATE_YELLOW;
              state_counter =0;
          }
        break;
      }

      case STATE_YELLOW:
      {
        Yellow_led();
        if(state_counter == BE_READY_TIME)
        {
            ProgramState = STATE_RED;
            state_counter =0;
        }
      
        break;
      }

      


    default:
      {
        break;
      }
  }


}

  ISR(TIMER1_COMPA_vect)
  {
    counter++;
    state_counter++;
  }


  void Init_Timer(void)
  {
    TCCR1A = 0;

    TCCR1B = 0;
    PORTB |= SET_PIN(1);
    
    TCCR1B |= 4;
    //TCCR1B |= 1 << WGM12;
    TCNT1 = 0;
    OCR1A = 62499;
    TIMSK1 = (1 << OCIE1A);

    sei();
  }


  void Red_led(void)
  {
    //turn on Red
    PORTB |= SET_PIN(3);
    // turn off others
    PORTB &= RESET_PIN(2);
    PORTB &= RESET_PIN(1);

  }


  void Yellow_led(void)
  {
    //turn on yellow
    PORTB |= SET_PIN(2);
    // turn off others
    PORTB &= RESET_PIN(3);
    PORTB &= RESET_PIN(1);
  }

  void Green_led(void)
  {
    //turn on green
    PORTB |= SET_PIN(1);
    // turn off others
    PORTB &= RESET_PIN(2);
    PORTB &= RESET_PIN(3);
  }

  void be_ready(void)
  {
    PORTB |= SET_PIN(3);
    PORTB |= SET_PIN(2);
    //turn off green
    PORTB &= RESET_PIN(1);
    
  }

    
  

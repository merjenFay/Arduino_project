// set counters to 0
int state_counter = 0;
int counter = 0;

#define STATE_N_S_PASSING 1
#define STATE_E_W_PASSING 2
#define STATE_NEUTRAL 3
#define STATE_SERVICE 4

#define SERVICE_REQUEST_ON  0x53
#define SERVICE_REQUEST_OFF 0x4F



#define SET_PIN(x) (1<<x)
#define RESET_PIN(x) ~(1<<x)

#define ON 1
#define OFF 0


#define DEBUG OFF


unsigned char ProgramState = 0;
unsigned char ServiceState = 0;



//declare functions
void Init_Timer();
void N_S_Green_EW_RED(void);
void N_S_Yellow_EW_RED(void);
void N_S_Red_EW_GREEN(void);
void E_W_Yellow_NS_RED(void);
void readDiagnosticServiceOFFRequest(void);
void readDiagnosticServiceRequest(void);
void service(void);

void N_S_passing(void);
void E_W_passing(void);
void neutral(void);

//function for reading diagnostic request to turn on service state
void readDiagnosticServiceRequest(void)
{
  unsigned char received;

  if (Serial.available() > 0)
  {
    received = Serial.read();
//if there is request to start service then change the state.
    if (received == SERVICE_REQUEST_ON)
    {
      state_counter = 0;
      ProgramState = STATE_SERVICE;
    }
  }
}
//function to turn off service state
void readDiagnosticServiceOFFRequest(void)
{
  unsigned char received;

  if (Serial.available() > 0)
  {
    received = Serial.read();

    if (received == SERVICE_REQUEST_OFF)
    {
      state_counter = 0;

      ProgramState = STATE_NEUTRAL;

    }
  }
}

//function for blinking state
void service(void)
{ Serial.println(counter);
  if (counter < 1)
  {
    PORTD |= SET_PIN(2); // N-S YELLOW
    PORTD |= SET_PIN(7); // E-W YELLOW
  }
  else if (counter < 2)
  {
    PORTD &= RESET_PIN(2);
    PORTD &= RESET_PIN(7);

  }
  else
  {
    counter = 0;
  }


}



void setup() {
  // set the pins input or output as needed:
  Serial.begin(9600);
  DDRB |= SET_PIN(4); //N-S RED
  DDRB |= SET_PIN(5); // N-S PEDESTRIAN RED
  DDRD |= SET_PIN(2); // N-S YELLOW
  DDRD |= SET_PIN(3);  // N-S  GREEN
  DDRD |= SET_PIN(4); // N-S PEDESTRIAN GREEN
  DDRD |= SET_PIN(5); //E-W RED
  DDRD |= SET_PIN(6); // E-W PEDESTRIAN RED
  DDRD |= SET_PIN(7); // E-W YELLOW
  DDRB |= SET_PIN(0); //E-W GREEN
  DDRB |= SET_PIN(1); // E-W PEDESSTRIAN GREEN
  DDRB &= RESET_PIN(2); // N_S PEDESTRIAN BUTTON
  DDRB &= RESET_PIN(3);// E-W PEDESTRIAN BUTTON
  
  /* Initialize the timer */
  Init_Timer();

  /* Begin the program in neutral state*/
  ProgramState = STATE_NEUTRAL;
}

void loop()
{
  /* declare the variable  initialized with neutral state */
  switch (ProgramState)
  {

    case STATE_NEUTRAL:
      {
        neutral();
        readDiagnosticServiceRequest();
#if (DEBUG == ON)
        Serial.println("I am in STATE_NEUTRAL");
#endif

        if (state_counter >= 3)
        {
          state_counter = 0;
          ProgramState = STATE_N_S_PASSING;
        }
        break;

      }

    case STATE_N_S_PASSING:
      {

        N_S_passing();
        readDiagnosticServiceRequest();
#if (DEBUG == ON)
        Serial.println("I am in STATE_N_S_PASSING");
#endif
        //Blue(E_W)pedestrian button pressed or timer expired
        if ( (state_counter >= 15) || ( (PINB & (1 << 2)) != 0 ) )
        {
          state_counter = 0;
          ProgramState = STATE_E_W_PASSING;
        }
        break;
      }

    case STATE_E_W_PASSING:
      {
        E_W_passing();
        readDiagnosticServiceRequest();
#if (DEBUG == ON)
        Serial.println("I am in STATE_E_W_PASSING");
#endif
        //RED(N_S)pedestrian button pressed or timer expired
        if ( (state_counter >= 15) || ( ( PINB & (1 << 3)) != 0 ) )
        {
          state_counter = 0;
          ProgramState = STATE_N_S_PASSING;

        }
        break;
      }

    case STATE_SERVICE:
      {
        service();
// turn off service and go to neutral state only if it is in service state
        readDiagnosticServiceOFFRequest();
#if (DEBUG == ON)
        Serial.println("I am in Service");
#endif
        break;
      }

    default:
      {
        break;
      }
  }


}

//start compare match timer
ISR(TIMER1_COMPA_vect)
{

  state_counter++;
  counter++;
}


//configure timer registers
void Init_Timer(void)
{
  
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= 4; // select clkI/O/256 prescaler

  TCNT1 = 0; // set Timer1 to 0
  OCR1A = 62499; // set compare match value (Output Compare Register 1 A Low and High byte)
  TIMSK1 = (1 << OCIE1A);//Timer/Counter 1, Output Compare A Match Interrupt Enable

  sei();//enable interupts to start
}


// function to open north south line
void N_S_passing(void)
{//keep north south line green for 10 seconds while east west line is red 
  if (state_counter < 10)
  {
    N_S_Green_EW_RED();
  }
  else
  {// after that keep yellow lights on for 5 seconds
    N_S_Yellow_EW_RED();
    if (state_counter >= 15)
    {
      //move on with red light
      N_S_Red_EW_GREEN();

    }
  }
}

void E_W_passing(void)
{//keep east west line green for 10 seconds while north south line is red 
  if (state_counter < 10)
  {
    N_S_Red_EW_GREEN();
  }
  else
  { // after that keep yellow lights on for 5 seconds
    E_W_Yellow_NS_RED();

    if (state_counter >= 15)
    {
      //move on with red light
      N_S_Green_EW_RED();
    }
    
  }
  

}

void neutral(void)
{
  //turn on all lights
  PORTB |= SET_PIN(4); //N-S RED
  PORTB |= SET_PIN(5);  // N-S PEDESTRIAN RED
  PORTD |= SET_PIN(2); // N-S YELLOW
  PORTD |= SET_PIN(3); // N-S  GREEN
  PORTD |= SET_PIN(4); // N-S PEDESTRIAN GREEN
  PORTD |= SET_PIN(5); //E-W RED
  PORTD |= SET_PIN(6); // E-W PEDESTRIAN RED
  PORTD |= SET_PIN(7); // E-W YELLOW
  PORTB |= SET_PIN(0); //E-W GREEN
  PORTB |= SET_PIN(1); // E-W PEDESSTRIAN GREEN


}

void N_S_Red_EW_GREEN(void)
{
  //turn on Red on n-s line
  PORTB |= SET_PIN(4); //N-S RED
  PORTB |= SET_PIN(5); // N-S PEDESTRIAN RED
  PORTB |= SET_PIN(0); //E-W GREEN
  PORTB |= SET_PIN(1); // E-W PEDESSTRIAN GREEN
  // turn off others
  PORTD &= RESET_PIN(2); // N-S YELLOW
  PORTD &= RESET_PIN(3); //N-S GREEN
  PORTD &= RESET_PIN(4); // N-S PEDESTRIAN GREEN
  PORTD &= RESET_PIN(5); //E-W RED
  PORTD &= RESET_PIN(6); // E-W PEDESTRIAN RED
  PORTD &= RESET_PIN(7); // E-W YELLOW
}

void N_S_Yellow_EW_RED(void)
{
  //turn on yellow
  PORTD |= SET_PIN(2); //N-S yellow
  PORTB |= SET_PIN(5); // N-S PEDESTRIAN RED
  PORTD |= SET_PIN(6); // E-W PEDESTRIAN RED
  PORTD |= SET_PIN(5); //E-W RED
  // turn off others
  PORTB &= RESET_PIN(4); //N-S RED
  PORTD &= RESET_PIN(3); //N-S GREEN
  PORTD &= RESET_PIN(4); //N-S PEDESTRIAN GREEN
  PORTB &= RESET_PIN(0); //E-W GREEN
  PORTB &= RESET_PIN(1); // E-W PEDESSTRIAN GREEN
  PORTD &= RESET_PIN(7); // E-W YELLOW
}


void N_S_Green_EW_RED(void)
{
  //turn on green
  PORTD |= SET_PIN(3); //N-S GREEN
  PORTD |= SET_PIN(4); //N-S PEDESTRIAN GREEN
  PORTD |= SET_PIN(6); // E-W PEDESTRIAN RED
  PORTD |= SET_PIN(5); //E-W RED
  // turn off others
  PORTB &= RESET_PIN(4); //N-S RED
  PORTB &= RESET_PIN(5); //N-S PEDESTRIAN RED
  PORTD &= RESET_PIN(2); //N-S YELLOW
  PORTB &= RESET_PIN(0); //E-W GREEN
  PORTB &= RESET_PIN(1); // E-W PEDESSTRIAN GREEN
  PORTD &= RESET_PIN(7); // E-W YELLOW
}



void E_W_Yellow_NS_RED(void)
{
  //turn on e-w yellow
  PORTD |= SET_PIN(7); // E-W YELLOW
  PORTD |= SET_PIN(6); // E-W PEDESTRIAN RED
  PORTB |= SET_PIN(4); //N-S RED
  PORTB |= SET_PIN(5); // N-S PEDESTRIAN RED
  //turn off others
  PORTB &= RESET_PIN(0); //E-W GREEN
  PORTB &= RESET_PIN(1); // E-W PEDESSTRIAN GREEN
  PORTD &= RESET_PIN(5); //E-W RED
  PORTD &= RESET_PIN(2); // N-S YELLOW
  PORTD &= RESET_PIN(3); //N-S GREEN
  PORTD &= RESET_PIN(4); // N-S PEDESTRIAN GREEN
}

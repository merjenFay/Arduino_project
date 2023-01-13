#define PIN_PWM 3
byte pinvalue=0;

void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_PWM, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i = 0;i<255;i++)
  {
    pinvalue++;
    delay(10);
      analogWrite(PIN_PWM, pinvalue);

  }
  for(int i=255;i>0;i--)
  {
    pinvalue--;
    delay(10);  
      analogWrite(PIN_PWM, pinvalue);

  }
  
  
}

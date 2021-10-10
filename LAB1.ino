
//www.elegoo.com
//2016.12.08
/*
 James Whitney
 EECE 5520
 10/10/21
 */
int ledPinG = 13; 
int ledPinY = 12;
int ledPinR = 11; //leds set to 13-11
int buttonApin = 2; //button set to 2 because of interrupt
int buzzer = 1; //button set to 1
volatile int count = 0; //counting variable, starts at 0 as to ensure the display does not go off
volatile byte state = LOW; //used to swap from the starting mode to the main loop
int clk = 10; //74HC595  pin 10 SHCP
int latch = 9; //74HC595  pin 9 STCP
int data = 8; //74HC595  pin 8 DS //clk, latch and data of the hc set to 10-8
int seg1 = 7;
int seg2 = 6;
int seg3 = 5;
int seg4 = 4; // 7 segment display set to 7-4
int segments[4] {7, 6, 5, 4};
int digits[4];
unsigned char table[] =
{0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f}; //Display digital data //all 3 arrays used for the displaying proper digits

void setup()
{
  pinMode(ledPinR, OUTPUT);
  pinMode(ledPinG, OUTPUT);
  pinMode(ledPinY, OUTPUT); //leds set to output
  pinMode(buttonApin, INPUT_PULLUP); //button set as an input
  attachInterrupt(digitalPinToInterrupt(buttonApin), modeswap, LOW); //attaching interrupt to when the button is pressed
  pinMode(buzzer, OUTPUT); //buzzer set as output
  pinMode(latch, OUTPUT);
  pinMode(clk, OUTPUT);
  pinMode(data, OUTPUT); //3 pins of the HC set as output
  pinMode (seg1, OUTPUT);
  pinMode (seg2, OUTPUT);
  pinMode (seg3, OUTPUT);
  pinMode (seg4, OUTPUT);
  digitalWrite(seg1, HIGH);
  digitalWrite(seg2, HIGH);
  digitalWrite(seg3, HIGH);
  digitalWrite(seg4, HIGH); //segments set as outputs and then turned off to start
 
  cli();//stop interrupts 

  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei();//allow interrupts 
}//interrupt set-up code provided by professor

ISR(TIMER1_COMPA_vect)
{ //timer1 interrupt 1Hz toggles pin 13 (LED)
  //generates pulse wave of frequency 1Hz/2 = 0.5kHz (takes two cycles for full wave- toggle high then toggle low)
  if (count > 0)
    count--; //as long as the count is above 0, it decreases by 1 every second
}

void Display() //gets the value of the count and splits it into 4 values, it's thousand value, hundred, ten and one, and puts it into an array
{
  digits[0] = count / 1000; 
  digits[1] = count / 100;
  digits[2] = count / 10; 
  digits[3] = count % 10;
  printDigits(); //calls a function to print the digits
}

void printDigits() //goes through the digit array and prints it to its respective segment, has a delay of 5ms to ensure all segments are legible at the same time
{
  int i = 0;
  for (i = 0; i < 4; i++)
  {
    shutDown(); //turns off all the segments, the program is running so fast that it is not perceptible to us and it appears that all are on at once
    digitalWrite(latch, LOW);
    shiftOut(data, clk, MSBFIRST, table[digits[i]]); //writes the bits to the data and then sends them out to the display
    digitalWrite(segments[i], LOW);
    digitalWrite(latch, HIGH);
    delay(5);
  }
}

void shutDown() //turns off all 4 displays at once, ironically done to make us able to see the proper display
{
  digitalWrite(seg1, HIGH);
  digitalWrite(seg2, HIGH);
  digitalWrite(seg3, HIGH);
  digitalWrite(seg4, HIGH);
}

void modeswap() //when button interupt is triggered, swaps from starting state to main state 
{
  state = !state;
}

void buzz() //program for buzzing the speaker given by the tutorial
{
  int i = 0;
  for (i = 0; i < 80; i++)
  {
    digitalWrite(buzzer, HIGH);
    delay(1);//wait for 1ms
    digitalWrite(buzzer, LOW);
    delay(1);//wait for 1ms
  }
}

void wait() //function to run while waiting for the light state to change, called to keep the display looking as it should
{
  Display (); //calls display to print digits
  if (count <= 3) //if count is under 3 sections, calls the speaker to buzz
    buzz();
}

void loop() //main loop
{
  while (state == LOW) //while in initial state (until button is presseD)
  {
    digitalWrite(ledPinR, HIGH);
    delay(1000);
    digitalWrite(ledPinR, LOW);
    delay(1000); //flips red led on and off until button is pressed 
  }
  while (true) //stays in main loop for rest of time
  {
    digitalWrite(ledPinR, HIGH); //starts with red led on, starts count at 20
    count = 20;
    while (count != 0)
      wait(); //waits for count to get to 0, wait calls display and buzz functions
    digitalWrite(ledPinR, LOW); //after count reaches 0, led swaps from red to green, count goes to 20
    digitalWrite(ledPinG, HIGH);
    count = 20;
    while (count != 0)
      wait(); //repeats 
    digitalWrite(ledPinG, LOW); //swaps from green to yellow
    digitalWrite(ledPinY, HIGH);
    count = 3; //count goes to 3 as the yellow light count is set to 3 rather than 20
    while (count != 0)
      wait();
    digitalWrite(ledPinY, LOW); //yellow goes off, loops back to turning red on
  }
}

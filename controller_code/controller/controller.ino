#include <Wire.h>

#define i2cAddr             0x07
#define cmdSuccess          0xFF


#define CMD_POWER_SET       0xCC

float power = 20;
int pins[7] = {11,6,2,4,5,9,13};
int gnds[4] = {12,8,7,A0};
int decimal = 3;



int num_array[10][7] = {  { 1,1,1,1,1,1,0 },    // 0
                          { 0,1,1,0,0,0,0 },    // 1
                          { 1,1,0,1,1,0,1 },    // 2
                          { 1,1,1,1,0,0,1 },    // 3
                          { 0,1,1,0,0,1,1 },    // 4
                          { 1,0,1,1,0,1,1 },    // 5
                          { 1,0,1,1,1,1,1 },    // 6
                          { 1,1,1,0,0,0,0 },    // 7
                          { 1,1,1,1,1,1,1 },    // 8
                          { 1,1,1,0,0,1,1 }};   // 9


// Output value should be between 0 and 320, inclusive.
void analogWrite25k(int pin, int value)
{
    switch (pin) {
        case 9:
            OCR1A = value;
            break;
        case 10:
            OCR1B = value;
            break;
        default:
            // no other pin will work
            break;
    }
}

void setup()
{
    Wire.begin(i2cAddr);
    Wire.onReceive(receiveData); // register event
    Wire.onRequest(sendData);
    Serial.begin(9600);
    Serial.println("msg");
    // Configure Timer 1 for PWM @ 25 kHz.
    TCCR1A = 0;           // undo the configuration done by...
    TCCR1B = 0;           // ...the Arduino core library
    TCNT1  = 0;           // reset timer
    TCCR1A = _BV(COM1A1)  // non-inverted PWM on ch. A
           | _BV(COM1B1)  // same on ch; B
           | _BV(WGM11);  // mode 10: ph. correct PWM, TOP = ICR1
    TCCR1B = _BV(WGM13)   // ditto
           | _BV(CS10);   // prescaler = 1
    ICR1   = 320;         // TOP = 320

    // Set the PWM pins as output.
    pinMode(10, OUTPUT);

    for (int pin=0; pin < 7; pin++) {
      pinMode(pins[pin], OUTPUT);
    }
    for (int pin=0; pin < 4; pin++) {
      pinMode(gnds[pin], OUTPUT);
      digitalWrite(gnds[pin], HIGH);
    }
    pinMode(decimal, OUTPUT);
}

void receiveData(int byteCount) {
  while(Wire.available())    // slave may send less than requested
  {
    //Serial.print(Wire.read());
    
    int cmdCode = Wire.read();
    if(cmdCode == CMD_POWER_SET){
      Serial.print("Command PWR Set to :");
      int dec = int(Wire.read());
      float ones = float(Wire.read());
      float temppower = dec + (ones/10);
      Serial.println(temppower);
      power = temppower;
    } 
  }
}

// callback for sending data
void sendData() {
  Wire.write(10);
}

void showNumber(float number){
    
   int tens = (int)number/10;
   int ones = (int)number%10;
   int temp = (int) number;
   int miniones = (number-temp)*10;
   displayDigit(tens, 0);
   displayDigit(ones, 1);
   displayDigit(miniones, 2);
}

void displayDigit(int digit, int pos){

     digitalWrite(gnds[pos], LOW);
     if(pos == 1){
      digitalWrite(decimal, HIGH);
     }
     else {
      digitalWrite(decimal, LOW);
     }
     for (int j=0; j < 7; j++) {
       digitalWrite(pins[j], num_array[digit][j]);
     }
     delay(4);
     digitalWrite(gnds[pos], HIGH);
    
}

void setPower(float power){
  int pwm = map(power, 0, 99.9, 0, 320);
  analogWrite25k(10, pwm);
  showNumber(power);
}

void loop()
{
    setPower(power);
}

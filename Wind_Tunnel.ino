#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#define PIN_SENSE 3 //where we connected the fan sense pin. Must be an interrupt capable pin (2 or 3 on Arduino Uno)
#define DEBOUNCE 0 //0 is fine for most fans, crappy fans may require 10 or 20 to filter out noise
#define FANSTUCK_THRESHOLD 500 //if no interrupts were received for 500ms, consider the fan as stuck and report 0 RPM
#define FANBUTTON_PIN 6
//Interrupt handler. Stores the timestamps of the last 2 interrupts and handles debouncing

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);
    
unsigned long volatile ts1 = 0;
unsigned long volatile ts2 = 0;

void tachISR() {
    unsigned long m=millis();
    if((m-ts2)>DEBOUNCE){
        ts1=ts2;
        ts2=m;
    }
}
//Calculates the RPM based on the timestamps of the last 2 interrupts. Can be called at any time.
unsigned long calcRPM(){
    if(millis()-ts2<FANSTUCK_THRESHOLD&&ts2!=0){
        return (60000/(ts2-ts1))/2;
    }else return 0;
}


//configure Timer 1 (pins 9,10) to output 25kHz PWM
void setupTimer1(){
    //Set PWM frequency to about 25khz on pins 9,10 (timer 1 mode 10, no prescale, count to 320)
    TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
    TCCR1B = (1 << CS10) | (1 << WGM13);
    ICR1 = 320;
    OCR1A = 0;
    OCR1B = 0;
}
//configure Timer 2 (pin 3) to output 25kHz PWM. Pin 11 will be unavailable for output in this mode
void setupTimer2(){
    //Set PWM frequency to about 25khz on pin 3 (timer 2 mode 7, prescale 8, count to 79)
    TIMSK2 = 0;
    TIFR2 = 0;
    TCCR2A = (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);
    TCCR2B = (1 << WGM22) | (1 << CS21);
    OCR2A = 79;
    OCR2B = 0;
}
//equivalent of analogWrite on pin 9
void setPWM1A(float f){
    f=f<0?0:f>1?1:f;
    OCR1A = (uint16_t)(320*f);
}
//equivalent of analogWrite on pin 10
void setPWM1B(float f){
    f=f<0?0:f>1?1:f;
    OCR1B = (uint16_t)(320*f);
}
//equivalent of analogWrite on pin 3
void setPWM2(float f){
    f=f<0?0:f>1?1:f;
    OCR2B = (uint8_t)(79*f);
}

void setup(){
    //enable outputs for Timer 1
    pinMode(9,OUTPUT); //1A
    pinMode(10,OUTPUT); //1B
    setupTimer1();
    //enable outputs for Timer 2
    pinMode(3,OUTPUT); //2
    setupTimer2();
    //note that pin 11 will be unavailable for output in this mode!
    //example...
    setPWM1A(0.5f); //set duty to 50% on pin 9
    setPWM1B(0.2f); //set duty to 20% on pin 10
    setPWM2(0.8f); //set duty to 80% on pin 3
    pinMode(PIN_SENSE,INPUT_PULLUP); //set the sense pin as input with pullup resistor
    attachInterrupt(digitalPinToInterrupt(PIN_SENSE),tachISR,FALLING); //set tachISR to be triggered when the signal on the sense pin goes low
    Serial.begin(9600); //enable serial so we can see the RPM in the serial monitor
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    pinMode(FANBUTTON_PIN, INPUT_PULLUP);
}

void loop(){
    delay(500);
    lcd.clear();
    lcd.print("Fan: ");
    lcd.print(calcRPM());
}
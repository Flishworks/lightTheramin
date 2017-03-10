//Light Theremin
//Avi Kazen
//2.2017


#define FASTADC 1
//defines for setting and clearing register bits
//This block came from here:http://forum.arduino.cc/index.php?topic=6549.15
//used to make micro run faster, since I need high speed for volume modulation
//and analog reads in between
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

int freq=1000; //in hertz
int volume=40; //1-100
int period; //
unsigned int i;
unsigned long tOne;
unsigned long tTwo;
int senseFreq;
int  senseVol;


void setup() {
  #if FASTADC
 // set prescale to 16
 // again, for the speed increase
   sbi(ADCSRA,ADPS2) ;
   cbi(ADCSRA,ADPS1) ;
   cbi(ADCSRA,ADPS0) ;
  #endif

  pinMode(3, OUTPUT);
  Serial.begin(9600);

  //calculate all starting variables
  senseFreq=analogRead(A0); //measured as 630-980
  senseVol=analogRead(A1); //measured as 640-950
  freq=map(senseFreq,630,980,500,4000); //software calibration of sensor values
  volume=constrain(map(senseVol,640,950,100,0),0,100);
  period=(1000000/freq); //convert frequency to half wavelenth (seconds/period)
  
}

void loop() {

  //for one half of the sound wavelength
  tTwo=micros()+period; //mark the timing
  while (tOne<tTwo){ 
      PORTD |= 0x8;//digitalWrite(3,HIGH); //faster way to flip bits
      delayMicroseconds(volume); //bit bang PWM the sound wave for volume control
      PORTD &= ~0x8;//digitalWrite(3,LOW);
      delayMicroseconds(100-volume); //bit bang PWM the sound wave for volume control
      tOne=micros();
  }

  //for the other half of the wavelength
  tTwo=micros()+period; //mark the timing
  
  //perform all calculations during this "off" cycle of the wave to avoid throwing off the timing
  senseFreq=analogRead(A0); //measured as 630-980
  senseVol=analogRead(A1); //measured as 640-950
  freq=map(senseFreq,630,980,500,4000); //software calibration of sensor values
  volume=constrain(map(senseVol,640,950,100,0),0,100);
  period=(1000000/freq); //convert frequency to half wavelenth (seconds/period)
  
  //now wait until we complete the second half of the wavelength
  while (tOne<tTwo){ 
    tOne=micros(); //nothing but counting
  }
  
}

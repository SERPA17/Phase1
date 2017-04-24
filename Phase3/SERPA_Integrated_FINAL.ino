#include <SoftwareSerial.h>
#include <math.h>
#include <Wire.h>
#include "VoiceRecognitionV3.h"
#include "RTClib.h"

const int ledPin = 4;
const int ldrpin = A1;
const int sensorPin = A0;

int count=0;
int bount=0;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

uint8_t records[7]; // save record
uint8_t buf[64];

RTC_DS3231 rtc;

VR myVR(2,3);    

#define in 12
#define out 11
#define relay1 7
#define relay2 8
#define lighton    (0)
#define lightoff   (1) 
#define fanon      (2)
#define fanoff     (3)
#define ledon      (4)
#define ledoff     (5)


  void IN()
{
    count++;
    bount=count;
    Serial.print("Person in room" );
    Serial.print(count);
    delay(1000);   
}

  void IN1()
{
    count=bount;
    bount++;
    Serial.print("Person in room" );
    Serial.print(bount);
    delay(1000); 
 }

 
  void OUT()
{
   if(bount>0)
    bount--;
    Serial.print("Person In Room:");
    Serial.print(bount);
    delay(1000);  
}

  void on()
{
    count=bount;
    digitalWrite(relay2,LOW);
    delay(300);
}

  void off()
{  
    count=0;
    digitalWrite(relay2, HIGH); 
    delay(300);
}

  void fon()
{
  digitalWrite(relay1,LOW);
  delay(100);
}

  void foff()
{
  digitalWrite(relay1,HIGH);
  delay(100);
}


void printSignature(uint8_t *buf, int len)
{
  int i;
  for(i=0; i<len; i++){
    if(buf[i]>0x19 && buf[i]<0x7F){
      Serial.write(buf[i]);
    }
    else{
      Serial.print("[");
      Serial.print(buf[i], HEX);
      Serial.print("]");
    }
  }
}


void printVR(uint8_t *buf)
{
  Serial.println("VR Index\tGroup\tRecordNum\tSignature");

  Serial.print(buf[2], DEC);
  Serial.print("\t\t");

  if(buf[0] == 0xFF){
    Serial.print("NONE");
  }
  else if(buf[0]&0x80){
    Serial.print("UG ");
    Serial.print(buf[0]&(~0x80), DEC);
  }
  else{
    Serial.print("SG ");
    Serial.print(buf[0], DEC);
  }
  Serial.print("\t");

  Serial.print(buf[1], DEC);
  Serial.print("\t\t");
  if(buf[3]>0){
    printSignature(buf+4, buf[3]);
  }
  else{
    Serial.print("NONE");
  }
  Serial.println("\r\n");
}


  
double Thermistor(int RawADC)
{
  double Temp;
  Temp = log(10000.0*((1024.0/RawADC-1))); 
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
  Temp = Temp - 260.15;            
  //Temp = (Temp * 9.0)/ 5.0 + 32.0; // Convert Celcius to Fahrenheit
  return Temp;
}

void setup()
{
  myVR.begin(9600);
  Serial.begin(115200);
  Serial.println("Elechouse Voice Recognition V3 Module");
  Serial.print("Visitor Counter");
  delay(2000);
  Serial.println();
  
  pinMode(in, INPUT);
  pinMode(out, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(relay1,OUTPUT);
  pinMode(relay2, OUTPUT);
  
  Serial.print("Person In Room:");
  Serial.print(count);
  Serial.println();
  
  if (! rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  
  if(myVR.clear() == 0)
  {
    Serial.println("Recognizer cleared.");
  }
  else
  {
    Serial.println("Not find VoiceRecognitionModule.");
    Serial.println("Please check connection and restart Arduino.");
    while(1);
  }
  
  if(myVR.load((uint8_t)lighton) >= 0){
    Serial.println("Lighton loaded");
  }
  
  if(myVR.load((uint8_t)lightoff) >= 0){
    Serial.println("Lightoff loaded");
  }
  if(myVR.load((uint8_t)fanon) >= 0){
    Serial.println("fanon loaded");
  }
  
  if(myVR.load((uint8_t)fanoff) >= 0){
    Serial.println("fanoff loaded");
  }
  
  if(myVR.load((uint8_t)ledon) >= 0){
    Serial.println("ledon loaded");
  }
  
  if(myVR.load((uint8_t)ledoff) >= 0){
    Serial.println("ledoff loaded");
  }
  
}

void loop()
{ 
    
    int ret;
    int readVal;
    double temp;
    int rate;
    
    DateTime now = rtc.now();
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
     
  if(digitalRead(in))
 {
  if(bount ==0)
   {
       IN(); 
   }
  else if(bount >0)
   {
       IN1();
   }
 }
  if(digitalRead(out))
  {
  OUT();
  }
  
  if(bount<=0)
  {
    digitalWrite(relay2, HIGH);
    Serial.print("Nobody");
    Serial.print("Light Is Off");
    Serial.println();
    delay(300);
  }
  else if(count>0 && now.hour()>8 || now.hour()<19)
  {
    digitalWrite(relay2, LOW);
  }
   
    
  ret = myVR.recognize(buf, 50);
  if(ret>0){
    switch(buf[1]){
      case lighton:
        on();
        break;
      case lightoff:
        off();
        break;
      case fanon:
        fon();
        break;
      case fanoff:
        foff();
        break;
      case ledon:
        digitalWrite(ledPin,HIGH);
        break;
      case ledoff:
        digitalWrite(ledPin,LOW);
        break;      
      default:
        Serial.println("Record function undefined");
        break;
    }
   
    printVR(buf);
}

  readVal = analogRead(sensorPin);
  temp =  Thermistor(readVal);
  Serial.print("Temprature :  ");    
  Serial.print(temp);
  Serial.println();
 
  if(temp >= 38 )
  {
        if(buf[1]!= fanoff)
        {
          fon();
        }
  }
  else
  {
        if(buf[1]!= fanon)
        {
        foff();  
        } 
        
  }

  
    rate = analogRead(ldrpin);
    Serial.println(rate);
    
    if(rate <= 100)
    {
    digitalWrite(ledPin,LOW);
    Serial.print("ledoff");
    Serial.println();
    
    }
    else
    {
    digitalWrite(ledPin,HIGH);
    Serial.println("ledon");
      
    }
    
  
  delay(800);
  
  
}

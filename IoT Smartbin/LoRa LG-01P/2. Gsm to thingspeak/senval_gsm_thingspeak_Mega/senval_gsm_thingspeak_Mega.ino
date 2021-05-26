#include <SPI.h>
#include <String.h>
#include <MQ135.h>
#include "GSM800Mega.h"

int LED=13;

uint8_t datasend[50];    //Storage  longtitude,latitude and altitude

#define dht_dpin A0 // Use A0 pin to connect the data line of DHT11
byte bGlobalErr;
char dht_dat[5];

#define mq135_pin A1
MQ135 gasSensor = MQ135(mq135_pin); // MQ135 digital pin connected to pin number A2
float ppm=0;

bool PIR_flag = false;
int pir_count;

int G_sharp ;

char url[80];

void setup()
{
  InitDHT();
  // initialize digital pin  as an output.
  pinMode(LED, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(2), pir_isr , FALLING);
  // initialize both serial ports:
  Serial.begin(115200);
  while (!Serial);
  gsm.Init();
  
  Serial.println("Ready to send!");
}

void loop()
{ 
  ReadDHT();
  
  ppm = gasSensor.getPPM();
  
  if(PIR_flag == true)
  {
    pir_count++;
    PIR_flag = false;
  }

  float volts = analogRead(A2);//*0.0048828125;   
  float distance = 65*pow(volts, -1.10);  
  G_sharp = distance*100;
  
  sprintf(url, "api.thingspeak.com/update?api_key=7JFN99ST9V99MUVX&field4=%d.%d&field5=%d.%d&field6=%d&field7=%d&field8=%d",dht_dat[0],dht_dat[1],dht_dat[2],dht_dat[3],(int)ppm,pir_count,G_sharp); // change api key accordingly
  Serial.println((char *)url);
  gsm.SendData(url);
  Serial.println("Sent via gsm");

  delay(16000);
}

void ReadDHT() // Read Temperature and Humidity value 
{ 
    bGlobalErr=0;
    byte dht_in;
    byte i;
    
     //pinMode(dht_dpin,OUTPUT);
    digitalWrite(dht_dpin,LOW);//pull low data line to send signal.
    delay(30);//Add a delay higher than 18ms so DHT11 can detect the start signal
    
    digitalWrite(dht_dpin,HIGH);
    delayMicroseconds(40);
    pinMode(dht_dpin,INPUT);
    // delayMicroseconds(40);
    dht_in=digitalRead(dht_dpin);//Get A0 state
    //   Serial.println(dht_in,DEC);
    if(dht_in){
       bGlobalErr=1;
       return;
       }
    delayMicroseconds(80);//Get DHT11 response , pull low data lineDHT11
    dht_in=digitalRead(dht_dpin);
    
    if(!dht_in){
       bGlobalErr=2;
       return;
       }
    delayMicroseconds(80);//
    for (i=0; i<5; i++)//Ger Temperature and Humidity value
       dht_dat[i] = read_dht_dat();
           
    pinMode(dht_dpin,OUTPUT);
    digitalWrite(dht_dpin,HIGH);
    byte dht_check_sum =
           dht_dat[0]+dht_dat[1]+dht_dat[2]+dht_dat[3];//Calculate check sum
    if(dht_dat[4]!= dht_check_sum)//Error when check sum dismatch
       {bGlobalErr=3;}
};

byte read_dht_dat(){
  byte i = 0;
  byte result=0;
  for(i=0; i< 8; i++){
      while(digitalRead(dht_dpin)==LOW);//
      delayMicroseconds(30);//
      if (digitalRead(dht_dpin)==HIGH)
     result |=(1<<(7-i));//
    while (digitalRead(dht_dpin)==HIGH);//
    }
  return result;
}

void InitDHT() // Initiate DHT11
{
    pinMode(dht_dpin,OUTPUT);//
    digitalWrite(dht_dpin,HIGH);//Set A0 to output high
}

// PIR
void pir_isr()
{
  PIR_flag = true;
}

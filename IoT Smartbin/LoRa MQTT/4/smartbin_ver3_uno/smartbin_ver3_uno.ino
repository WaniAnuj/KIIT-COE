/*******************************************************************************************
 * Product :- Iot Smart Bin 
 * Version :- 2
 * Author :- Swapnil Bawa
 * Co-Author :- Prathamesh Satardekar
 * Co-Author :- Aditya Pappu 
 * Co-Author :- Anuj Wani
 * This code will take input from dht 11 , MQ135, PIR and sharp sensor and upload the data on
 * thingspeak.
 ******************************************************************************************/


//#include <TimerOne.h>
#include "GSM800.h"
#include <SoftwareSerial.h>
#include <MQ135.h>
#include <DHT.h>
//#include <SharpDistSensor.h>

#define dht11_PIN A0                       // dht 11 pin connected to pin number 6
#define mq135_pin A1                      // MQ135 digital pin connected to pin number A2
#define shp_pin A2                        // Sharp sensor connected on pin number A0
const byte mediumFilterWindowSize = 1;

GSM800 gsm(6, 7); //Rx and Tx
DHT dht(A0, DHT11);
MQ135 gasSensor = MQ135(A1);
//SharpDistSensor sensor(A3, mediumFilterWindowSize);

int G_humidity = 0, upload_flag=1;
int G_temp = 0, temp;
int G_sharp ;
int pir_count,d_ini=0;
char url[100];
float ppm=0;
bool PIR_flag = false;
unsigned long previous , present;

void pir_isr()
{
  PIR_flag = true;
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting...");
  attachInterrupt(digitalPinToInterrupt(2), pir_isr , FALLING);
//  float rzero = gasSensor.getRZero();
//  temp = sensor.getDist();
//  temp = sensor.getDist();
  delay(12000);
  gsm.Init();
  previous = millis();
  present = millis();
}


void loop()
{
  if(PIR_flag == true)
  {
    pir_count++;
    PIR_flag = false;
  }
  
  if ( present > 60000 || upload_flag == 1)
  {
    upload_flag = 0;
    Serial.println("begining uploading");
    previous = millis() ;    
    ppm = gasSensor.getPPM();
    int chk = dht.read(dht11_PIN);
    G_humidity = dht.readHumidity();
    G_temp = dht.readTemperature();
  float volts = analogRead(A2);//*0.0048828125;   
  float distance = 65*pow(volts, -1.10);  
  G_sharp = distance*100;
//    G_sharp = 1.3* (900 - sensor.getDist())/10;
    if(G_sharp > 100)
      G_sharp = 0;
    Serial.println("Sending all sensors data");
    sprintf(url, "api.thingspeak.com/update?api_key=7JFN99ST9V99MUVX&field1=%d&field2=%d&field3=%d&field4=%d&field5=%d", pir_count, G_temp , G_humidity, int(ppm), G_sharp ); // change api key accordingly
    gsm.SendData(url);
    Serial.println(url);
   }


   present = millis() - previous;
 /*  Serial.print("Next upload in : ");
   Serial.print(60 - (present/1000));
   Serial.println(" sec");
   delay(1000);
*/
}

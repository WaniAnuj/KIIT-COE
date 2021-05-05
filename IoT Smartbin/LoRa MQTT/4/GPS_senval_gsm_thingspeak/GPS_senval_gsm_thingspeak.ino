#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <String.h>
#include <MQ135.h>
#include "GSM800.h"
//#include <NeoSWSerial.h>

TinyGPS gps;
RH_RF95 rf95;

SoftwareSerial  ss(3, 4); // Arduino RX, TX to conenct to GPS module.
GSM800 gsm(6, 7); /*Tx of sim800l to pin6 of Arduino uno and 
                    Rx of sim800l to pin7 of Arduino uno*/

static void smartdelay(unsigned long ms);

int LED=13;

//char DeviceID[10]="a840418"; //DeviceID or Device identifier in GPSWOX.com

String datastring1="";
String datastring2="";
String datastring3="";
uint8_t datasend[50];    //Storage  longtitude,latitude and altitude

char gps_lon[20]={"\0"};  //Storage GPS info
char gps_lat[20]={"\0"}; //Storage latitude
char gps_alt[20]={"\0"}; //Storage altitude

#define dht_dpin A0 // Use A0 pin to connect the data line of DHT11
byte bGlobalErr;
char dht_dat[5];

#define mq135_pin A1
MQ135 gasSensor = MQ135(mq135_pin); // MQ135 digital pin connected to pin number A2
float ppm=0;

bool PIR_flag = false;
int pir_count;

int G_sharp ;

int upload_flag = 1;
char url[80];
unsigned long previous , present;

void setup()
{
  InitDHT();
  // initialize digital pin  as an output.
  pinMode(LED, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(2), pir_isr , FALLING);
  // initialize both serial ports:
  Serial.begin(115200);  // Serial to print out GPS info in Arduino IDE
  ss.begin(9600);       // SoftSerial port to get GPS data. 
  while (!Serial);

   if (!rf95.init()) {  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  
  /* Set frequency is 868MHz,SF is 7,SB is 125KHz,CR is 4/5 and Tx power is 20dBm */
  rf95.setFrequency(868);
  rf95.setSpreadingFactor(7);
  rf95.setSignalBandwidth(125E3);
  rf95.setCodingRate4(5);
  rf95.setTxPower(20,false);

  gsm.Init();
  
  Serial.println("Ready to send!");
  previous = millis();
  present = millis();
}

void loop()
{
  float flat, flon,falt;
  unsigned long age;
  gps.f_get_position(&flat, &flon, &age);
  falt=gps.f_altitude();  //get altitude       
  flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6;//save six decimal places 
  flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6;
  falt == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : falt, 2;//save two decimal places
  datastring1 +=dtostrf(flat, 0, 6, gps_lat); 
  datastring2 +=dtostrf(flon, 0, 6, gps_lon);
  datastring3 +=dtostrf(falt, 0, 2, gps_alt);
  
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
  
//  if(flon!=1000.000000)
//  {
  
//  strcat(gps_lon,DeviceID);
//  strcat(gps_lon,",");

switch (bGlobalErr)
   {
     case 0:
//       Serial.print("Current humdity = ");
//       Serial.print(dht_dat[0], DEC);
//       Serial.print(".");
//       Serial.print(dht_dat[1], DEC);
//       Serial.print("%  ");
//       Serial.print("temperature = ");
//       Serial.print(dht_dat[2], DEC);
//       Serial.print(".");
//       Serial.print(dht_dat[3], DEC);
//       Serial.println("C  ");
//       strcat(gps_lon,",");
//       strcat(gps_lon,gps_lat); 
//       strcat(gps_lon,","); 
//       strcat(gps_lon,gps_alt);
//       strcat(gps_lon,",");
//       strcat(gps_lon,dht_dat[0]);
//       strcat(gps_lon,"."); 
//       strcat(gps_lon,dht_dat[1]);
//       strcat(gps_lon,",");
//       strcat(gps_lon,dht_dat[2]);
//       strcat(gps_lon,".");
//       strcat(gps_lon,dht_dat[3]);
//       strcat(gps_lon,",");
//       strcat(gps_lon,ppm);
//       strcat(gps_lon,",");
//       strcat(gps_lon,pir_count);
//       strcat(gps_lon,",");
//       strcat(gps_lon,G_sharp);
//       strcat(gps_lon,",");
//       strcat((char *)datasend,gps_lon);
       sprintf(datasend,"%d,%d,%d,%d.%d,%d.%d,%d,%d,%d,",gps_lon,gps_lat,gps_alt,dht_dat[0],dht_dat[1],dht_dat[2],dht_dat[3],(int)ppm,pir_count,G_sharp);
       Serial.println((char *)datasend);

       
       // send data
//       if ( millis()-present > 25000 || upload_flag == 1)
//        {
            rf95.send(datasend, sizeof(datasend));
            rf95.waitPacketSent();
            Serial.println("Sent via LoRa");
            sprintf(url, "api.thingspeak.com/update?api_key=7JFN99ST9V99MUVX&field1=%d&field2=%d&field3=%d&field4=%d.%d&field5=%d.%d&field6=%d&field7=%d&field8=%d",gps_lon,gps_lat,gps_alt,dht_dat[0],dht_dat[1],dht_dat[2],dht_dat[3],(int)ppm,pir_count,G_sharp); // change api key accordingly
            Serial.println((char *)url);
            upload_flag = 0;
            gsm.SendData(url);
            Serial.println("Sent via gsm");
//        }
//       present = millis();
delay(20000);
       break;
     case 1:
        Serial.println("Error 1: DHT start condition 1 not met.");
        break;
     case 2:
        Serial.println("Error 2: DHT start condition 2 not met.");
        break;
     case 3:
        Serial.println("Error 3: DHT checksum error.");
        break;
     default:
        Serial.println("Error: Unrecognized code encountered.");
        break;
    }
  
   // Now wait for a reply
  receivepacket();
//  }
  smartdelay(3000);
}

//If the packet arrive LG01, LG01 will send a ACK and here will receive it and turn on the led.  
void receivepacket(){
    uint8_t indatabuf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(indatabuf);
    
    if (rf95.waitAvailableTimeout(3000))
     { 
       // Should be a reply message for us now   
       if (rf95.recv(indatabuf, &len))
         {
         Serial.println((char*)indatabuf);        
         digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
         }
         else 
         {
          digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
          Serial.println("receive failed!");
         }
    }
    else
    {
      // Serial print "No reply, is track_GPS_server running or overranging the receive distance?" if don't get the reply .
      //Serial.println("No reply, is track_GPS_Server running or overranging the receive distance?");
      digitalWrite(LED, LOW);    // turn the HEART_LED off by making the voltage LOW
    }
}

static void smartdelay(unsigned long ms)
{
  
  unsigned long start = millis();
  do 
  {
    while (ss.available())
    {
      //ss.print(Serial.read());
      gps.encode(ss.read());
    }
  } while (millis() - start < ms);
}

// DHT
void InitDHT() // Initiate DHT11
{
    pinMode(dht_dpin,OUTPUT);//
    digitalWrite(dht_dpin,HIGH);//Set A0 to output high
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

// PIR
void pir_isr()
{
  PIR_flag = true;
}

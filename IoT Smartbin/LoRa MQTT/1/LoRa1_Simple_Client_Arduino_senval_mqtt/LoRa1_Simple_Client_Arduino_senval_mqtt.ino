#include <SPI.h>
#include <RH_RF95.h>
#include <String.h>
#include <MQ135.h>

RH_RF95 rf95;

int LED=13;

int node_id = 4652;

#define dht_dpin A0 // Use A0 pin to connect the data line of DHT11
byte bGlobalErr;
char dht_dat[5];

#define mq135_pin A1
MQ135 gasSensor = MQ135(mq135_pin); // MQ135 digital pin connected to pin number A2
float ppm=0;
uint8_t datasend[80];
bool PIR_flag = false;
int pir_count;

int G_sharp ;

void setup()
{
  InitDHT();
  // initialize digital pin  as an output.
  pinMode(LED, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(2), pir_isr , FALLING);
  // initialize both serial ports:
  Serial.begin(9600);  // Serial to print out GPS info in Arduino IDE
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
  rf95.setSyncWord(0x14);
  rf95.setTxPower(20,false);
  
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
  
       sprintf(datasend,"<%d>field4=%d.%d&field5=%d.%d&field6=%d&field7=%d&field8=%d",node_id,dht_dat[0],dht_dat[1],dht_dat[2],dht_dat[3],(int)ppm,pir_count,G_sharp);
       Serial.println((char *)datasend);
  
       rf95.send(datasend, sizeof(datasend));  
       rf95.waitPacketSent();

  receivepacket();
  delay(1000);
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
//       Serial.println(dht_in,DEC);
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

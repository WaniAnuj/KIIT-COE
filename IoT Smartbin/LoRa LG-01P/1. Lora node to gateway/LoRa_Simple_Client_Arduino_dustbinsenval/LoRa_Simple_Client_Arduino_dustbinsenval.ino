#include <SPI.h>
#include <RH_RF95.h>
#include <MQ135.h>

// Singleton instance of the radio driver
RH_RF95 rf95;

//The parameter are pre-set for 868Mhz used. If user want to use lower frenqucy 433Mhz.Better to set 
//rf95.setSignalBandwidth(31250);
//rf95.setCodingRate4(8);
float frequency = 868.0;
uint8_t datasend[100];
int LED=13;

#define dht_dpin A0 // Use A0 pin to connect the data line of DHT11
byte bGlobalErr;
char dht_dat[5];

#define mq135_pin A1
MQ135 gasSensor = MQ135(mq135_pin); // MQ135 digital pin connected to pin number A2
float ppm=0;

bool PIR_flag = false;
int pir_count;

int G_sharp ;

void setup() 
{
  pinMode(LED, OUTPUT);
  InitDHT();
  attachInterrupt(digitalPinToInterrupt(2), pir_isr , FALLING);
  Serial.begin(9600);
  //while (!Serial) ; // Wait for serial port to be available
  Serial.println("Start LoRa Client");
  if (!rf95.init())
    Serial.println("init failed");
  // Setup ISM frequency
  rf95.setFrequency(frequency);
  // Setup Power,dBm
  rf95.setTxPower(13);

  // Setup Spreading Factor (6 ~ 12)
  rf95.setSpreadingFactor(7);
  
  // Setup BandWidth, option: 7800,10400,15600,20800,31250,41700,62500,125000,250000,500000
  //Lower BandWidth for longer distance.
  rf95.setSignalBandwidth(125000);
  
  // Setup Coding Rate:5(4/5),6(4/6),7(4/7),8(4/8) 
  rf95.setCodingRate4(5);

  rf95.setSyncWord(0x34);
  /*
  //Different Combination for distance and speed examples: 
  Example 1: Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. Default medium range
    rf95.setSignalBandwidth(125000);
    rf95.setCodingRate4(5);
    rf95.setSpreadingFactor(7);
  Example 2: Bw = 500 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. Fast+short range
    rf95.setSignalBandwidth(500000);
    rf95.setCodingRate4(5);
    rf95.setSpreadingFactor(7);
  Example 3: Bw = 31.25 kHz, Cr = 4/8, Sf = 512chips/symbol, CRC on. Slow+long range
    rf95.setSignalBandwidth(31250);
    rf95.setCodingRate4(8);
    rf95.setSpreadingFactor(9);
  Example 4: Bw = 125 kHz, Cr = 4/8, Sf = 4096chips/symbol, CRC on. Slow+long range
    rf95.setSignalBandwidth(125000);
    rf95.setCodingRate4(8);
    rf95.setSpreadingFactor(12); 
  */
}

void loop()
{
  Serial.println("Sending Sensor value to LoRa Server");
  
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
  
  switch (bGlobalErr)
   {
     case 0:
       Serial.print("Current humdity = ");
       Serial.print(dht_dat[0], DEC);
       Serial.print(".");
       Serial.print(dht_dat[1], DEC);
       Serial.print("%  ");
       Serial.print("temperature = ");
       Serial.print(dht_dat[2], DEC);
       Serial.print(".");
       Serial.print(dht_dat[3], DEC);
       Serial.println("C  ");

       sprintf(datasend,"%d.%d,%d.%d,%d,%d,%d,",dht_dat[0],dht_dat[1],dht_dat[2],dht_dat[3],(int)ppm,pir_count,G_sharp);
       Serial.println((char *)datasend);
  
       // send data
       rf95.send(datasend, sizeof(datasend));  
       rf95.waitPacketSent();
       receivepacket();
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
  delay(5000);
}

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

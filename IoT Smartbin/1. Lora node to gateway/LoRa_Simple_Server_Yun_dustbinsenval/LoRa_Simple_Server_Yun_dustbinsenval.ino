//For product: LG01. 
#define BAUDRATE 115200

//If you use Dragino Yun Mesh Firmware , uncomment below lines. 
//#define BAUDRATE 250000

#include <Console.h>
#include <SPI.h>
#include <RH_RF95.h>

// Singleton instance of the radio driver
RH_RF95 rf95;

char tem[20]="\0";           //Storage tempreture
char hum[20]="\0";           //Storage humidity
char ppm[20]="\0";           //Storage CO2 air quality
char pir[20]="\0";           //Storage pir
char dst[20]="\0";           //Storage distance

int led = A2;
float frequency = 868.0;

void setup() 
{
  pinMode(led, OUTPUT);     
  Bridge.begin(BAUDRATE);
  Console.begin();
//  while (!Console) ; // Wait for console port to be available
  Console.println("Start Sketch");
  if (!rf95.init())
    Console.println("init failed");
  // Setup ISM frequency
  rf95.setFrequency(frequency);
  // Setup Power,dBm
  rf95.setTxPower(13);
  
  // Setup Spreading Factor (6 ~ 12)
  rf95.setSpreadingFactor(7);
  
  // Setup BandWidth, option: 7800,10400,15600,20800,31200,41700,62500,125000,250000,500000
  rf95.setSignalBandwidth(125000);
  
  // Setup Coding Rate:5(4/5),6(4/6),7(4/7),8(4/8) 
  rf95.setCodingRate4(5);

  rf95.setSyncWord(0x34);
  
  Console.print("Listening on frequency: ");
  Console.println(frequency);
}

void loop()
{
  receivepacket();
}

void receivepacket() {
   if (rf95.available())
  {
    // received a packet
    Console.print("Get new data: ");

    int i = 0,j=0,code[5];
    int m1=0,m2=0,m3=0,m4=0,m5=0; 
    uint8_t buf[100];
    char message[100]="\0";
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len)){
      Console.print((char*)buf);
      strcpy(message,(char *)buf);
      while(i<50)
      {
      if(message[i]==',')
      {
        code[j]=i;
        j++;
        }
        i++;
      }
    }
    for(int k=0;k<code[0];k++)
    {
      hum[m1]=message[k];//get longtitude
      m1++;
    }
     for(int k=code[0]+1;k<code[1];k++)
    {
      tem[m2]=message[k];//get latitude
      m2++;
    }
     for(int k=code[1]+1;k<code[2];k++)
    {
      ppm[m3]=message[k];//get altitude
      m3++;
    }
    for(int k=code[2]+1;k<code[3];k++)
    {
      pir[m4]=message[k];//get tempreture
      m4++;
    }
    for(int k=code[3]+1;k<code[4];k++)
    {
      dst[m5]=message[k];//get humidity
      m5++;
    }
    
    Console.print("  with RSSI: ");
    Console.print(rf95.lastRssi(), DEC);
    Console.print("the tempreture is ");Console.print(tem);Console.println(" C");
    Console.print("the humidity is ");Console.print(hum);Console.println("%");
    Console.print("the CO2 presence is ");Console.print(ppm);Console.println(" ppm");
    Console.print("the PIR count is ");Console.println(pir);
    Console.print("the garbage level is ");Console.print(dst);Console.println(" cm");
    
    
    uint8_t data[] = "Gateway receive GPS data";
    rf95.send(data, sizeof(data));
    rf95.waitPacketSent();
  }
}

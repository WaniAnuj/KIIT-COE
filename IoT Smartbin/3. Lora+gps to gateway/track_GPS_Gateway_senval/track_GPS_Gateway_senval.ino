#include <Console.h> 
#include <Process.h>
#include <SPI.h>
#include <RH_RF95.h>

RH_RF95 rf95;

char DeviceID[20]="\0"; //Storage DeviceID or Device identifier
char lon[20]="\0";           //Storage longtitude
char lat[20]="\0";           //Storage latitude
char alt[20]="\0";           //Storage altitude
char tem[20]="\0";           //Storage tempreture
char hum[20]="\0";           //Storage humidity
char ppm[20]="\0";           //Storage CO2 air quality
char pir[20]="\0";           //Storage pir
char dst[20]="\0";           //Storage distance



void getTimeStamp();     //LG01 will call the Linux "date" command and get the time stamp
void receivepacket();    //Processing receive message and store it in the appropriate array
void run_send_gps_data();//LG01 will call the Linux "send_gps_data.sh" command and write the GPS data in GPSWOX.com

void setup() { 
  Bridge.begin(115200);  
  Console.begin();
  while (!Console);
  
   if (!rf95.init()) {// Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
    Console.println("Starting LoRa failed!");
    while (1);
  }
  /* Set frequency is 868MHz,SF is 7,SB is 125KHz,CR is 4/5 and Tx power is 20dBm */
  rf95.setFrequency(868);
  rf95.setSpreadingFactor(7);
  rf95.setSignalBandwidth(125E3);
  rf95.setCodingRate4(5);
  rf95.setTxPower(20,false);
  
  Console.println("Ready to receive!");
}
void loop(){
 receivepacket();
}

void getTimeStamp() {
  Process time;
  // date is a command line utility to get the date and the time 
  // in different formats depending on the additional parameter 
  time.begin("date");
  time.addParameter("+%D-%T");  // parameters: D for the complete date mm/dd/yy
                                //             T for the time hh:mm:ss    
  time.run();  // run the command

  // read the output of the command
  while(time.available()>0) {
    char c = time.read();
    Console.print(c);
  }
}

void run_send_gps_data() {//send_gps_data.sh -d DeviceID -l latitude -n longtitude -a altitude
  Process p;    // Create a process and call it "p"
  p.begin("send_gps_data.sh");
  p.addParameter("-d");
  p.addParameter(DeviceID);
  p.addParameter("-l"); 
  p.addParameter(lat);
  p.addParameter("-n"); 
  p.addParameter(lon);
  p.addParameter("-a"); 
  p.addParameter(alt);
  p.run();    // Run the process and wait for its termination
}

//Receiver LoRa packets and forward it 
void receivepacket() {
   if (rf95.available())
  {
    // received a packet
    Console.print("Get new data: ");

    int i = 0,j=0,code[7];
    int m1=0,m2=0,m3=0,m4=0,m5=0,m6=0,m7=0,m8=0;   
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
      lon[m1]=message[k];//get longtitude
      m1++;
    }
     for(int k=code[0]+1;k<code[1];k++)
    {
      lat[m2]=message[k];//get latitude
      m2++;
    }
     for(int k=code[1]+1;k<code[2];k++)
    {
      alt[m3]=message[k];//get altitude
      m3++;
    }
    for(int k=code[2]+1;k<code[3];k++)
    {
      tem[m4]=message[k];//get tempreture
      m4++;
    }
    for(int k=code[3]+1;k<code[4];k++)
    {
      hum[m5]=message[k];//get humidity
      m5++;
    }
    for(int k=code[4]+1;k<code[5];k++)
    {
      ppm[m6]=message[k];//get CO2
      m6++;
    }
    for(int k=code[5]+1;k<code[6];k++)
    {
      pir[m7]=message[k];//get PIR count
      m7++;
    }
    for(int k=code[6]+1;k<code[7];k++)
    {
      dst[m8]=message[k];//get garbage lvl
      m8++;
    }
//    run_send_gps_data();
    
    Console.print("  with RSSI: ");
    Console.print(rf95.lastRssi(), DEC);
    Console.print("  ");getTimeStamp();
    Console.print("the longtitude is " );Console.print(lon); Console.println(" deg");
    Console.print("the latitude is ");Console.print(lat);Console.println(" deg");
    Console.print("the altitude is ");Console.print(alt);Console.println(" deg");
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

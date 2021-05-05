#include "Arduino.h"
#include "GSM800.h"
#include <SoftwareSerial.h>

String number = "";
String Msg = "";
GSM800::GSM800(int Rx, int Tx)
{
  SIM800 = new SoftwareSerial(Rx, Tx);
}

void GSM800::Init()
{

  SIM800->begin(115200);
  SIM800->println(F("ATE0")); 															//Turn echo off
  if (SIM800->available()) {
    Serial.print(SIM800->readString());
  }
  delay(50);
  SIM800->println(F("AT+CGATT?")); 														//Attach gprs
  if (SIM800->available()) {
    Serial.print(SIM800->readString());
  }
  delay(50);
  SIM800->println(F("AT+CIPSHUT")); 														//Shut exisiting connections
  if (SIM800->available()) {
    Serial.print(SIM800->readString());
  }
  delay(50);
  SIM800->println(F("AT+SAPBR=3,1,\"Contype\",\"GPRS\"")); 								//Configure bearer profile
  if (SIM800->available()) {
    Serial.print(SIM800->readString());
  }
  delay(50);
  SIM800->println(F("AT+SAPBR=3,1,\"APN\",\"www\"")); //vodafone APN
  if (SIM800->available()) {
    Serial.print(SIM800->readString());
  }
  delay(50);
  SIM800->println(F("AT+SAPBR =0,1"));													//Close GPRS Content
  if (SIM800->available()) {
    Serial.print(SIM800->readString());
  }
  delay(500);
  SIM800->println(F("AT+SAPBR =1,1"));													//Open GPRS Content
  if (SIM800->available()) {
    Serial.print(SIM800->readString());
  }
  delay(2000);
  SIM800->println(F("AT+SAPBR =2,1")); 													//Query for the GPRS Content
  if (SIM800->available()) {
    Serial.print(SIM800->readString());
  }
  delay(50);
  SIM800->println(F("AT+HTTPTERM")); 													//Terminate HTTP Service
  if (SIM800->available()) {
    Serial.print(SIM800->readString());
  }
  delay(50);
  SIM800->println(F("AT+HTTPINIT")); 													//Initialize HTTP Service
  if (SIM800->available()) {
    Serial.print(SIM800->readString());
  }
  delay(50);
  SIM800->println(F("AT+HTTPPARA=CID,1")); 												//HTTP Parameter, Bearer Profile Identifier
  if (SIM800->available()) {
    Serial.print(SIM800->readString());
  }
  delay(50);
}

void GSM800::SendData(String url)
{
  SIM800->print("AT+HTTPPARA=\"URL\",\"");
  SIM800->print(url);
  SIM800->println("\"");

  if (SIM800->available()) {
    Serial.print(SIM800->readString());
  }
  delay(500);

  SIM800->println("AT+HTTPACTION=0");
  if (SIM800->available()) {
    Serial.print(SIM800->readString());
  }
  delay(500);

  SIM800->println(F("AT+HTTPread"));
  if (SIM800->available()) {
    Serial.print(SIM800->readString());
  }
  delay(500);
}

String GSM800::RecSms()
{
  //struct MyArray recsms;
  String myArray = "";
  String buff = "";
//  String cmp;
  char* buff1;
  char* subString;
  char* cmt = "+CMT:";
  char * pars;
  //SIM800->println("AT+CMGF=1");
  SIM800->println(F("AT+CNMI=2,2,0,0,0"));
  if (SIM800->available()) {
    buff = SIM800->readString();
    //    Serial.print(buff);
    //buff1 = buff.c_str();
//    cmp = buff;
    buff1 = &buff[0];
    pars = strstr(buff1, cmt);
    if (pars != NULL)
    {
      Serial.println(buff);
//      Serial.println(cmp);
      int firstline = buff.indexOf('\"' , 5);
      //      Serial.println(firstline);
      int secondline = buff.indexOf('\"', firstline + 5);
      //      Serial.println(secondline);
      int thirdline = buff.indexOf('+', secondline + 3);
      //      Serial.println(thirdline);
      int fourthline = buff.indexOf('\n', thirdline);
      //      Serial.println(fourthline);
      int fifthline = buff.indexOf(';', fourthline);
      //      Serial.println(fifthline);
      int sixthline = buff.indexOf(';', fifthline + 3);
      //      Serial.println(sixthline);
      Msg = buff.substring(fifthline + 1, sixthline);
//      Serial.println(Msg);
      int numone = buff.indexOf('\"');
      int numtwo = buff.indexOf('+', numone);
      int numthree = buff.indexOf('\"', numtwo + 5);
      number = buff.substring(numone + 1, numthree);
//      Serial.println(number);
      myArray = number + '.' + Msg + '.';
      Serial.println(myArray);
    }
  }
  return myArray;
  delay(50);
}

void GSM800::SendSms(String num, String text)
{
  SIM800->println(F("AT+CSCS=\"GSM\""));
  if (SIM800->available()) {
    Serial.print(SIM800->readString());
  }
  delay(50);
  SIM800->println(F("AT+CMGF = 1"));
  if (SIM800->available()) {
    Serial.print(SIM800->readString());
  }
  delay(50);
  //  Serial.println(num);
  //  Serial.println(text);
  SIM800->print(F("AT+CMGS=\""));
  SIM800->print(num);
  SIM800->println(F("\"\r"));
  delay(50);
  SIM800->println(text);
  delay(50);
  SIM800->println((char)26);
  delay(50);
  if (SIM800->available()) {
    Serial.print(SIM800->readString());
  }
  delay(50);
}

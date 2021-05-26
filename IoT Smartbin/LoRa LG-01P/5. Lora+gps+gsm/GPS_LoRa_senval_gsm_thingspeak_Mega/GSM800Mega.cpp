#include "Arduino.h"
#include "GSM800Mega.h"

String number = "";
String Msg = "";

GSM800Mega::GSM800Mega()
{
  
}

void GSM800Mega::Init()
{

  Serial2.begin(9600);
  Serial2.println(F("ATE0")); 															//Turn echo off
  if (Serial2.available()) {
    Serial.print(Serial2.readString());
  }
  delay(50);
  Serial2.println(F("AT+CGATT?")); 														//Attach gprs
  if (Serial2.available()) {
    Serial.print(Serial2.readString());
  }
  delay(50);
  Serial2.println(F("AT+CIPSHUT")); 														//Shut exisiting connections
  if (Serial2.available()) {
    Serial.print(Serial2.readString());
  }
  delay(50);
  Serial2.println(F("AT+SAPBR=3,1,\"Contype\",\"GPRS\"")); 								//Configure bearer profile
  if (Serial2.available()) {
    Serial.print(Serial2.readString());
  }
  delay(50);
  Serial2.println(F("AT+SAPBR=3,1,\"APN\",\"www\"")); //vodafone APN
  if (Serial2.available()) {
    Serial.print(Serial2.readString());
  }
  delay(50);
  Serial2.println(F("AT+SAPBR =0,1"));													//Close GPRS Content
  if (Serial2.available()) {
    Serial.print(Serial2.readString());
  }
  delay(500);
  Serial2.println(F("AT+SAPBR =1,1"));													//Open GPRS Content
  if (Serial2.available()) {
    Serial.print(Serial2.readString());
  }
  delay(2000);
  Serial2.println(F("AT+SAPBR =2,1")); 													//Query for the GPRS Content
  if (Serial2.available()) {
    Serial.print(Serial2.readString());
  }
  delay(50);
  Serial2.println(F("AT+HTTPTERM")); 													//Terminate HTTP Service
  if (Serial2.available()) {
    Serial.print(Serial2.readString());
  }
  delay(50);
  Serial2.println(F("AT+HTTPINIT")); 													//Initialize HTTP Service
  if (Serial2.available()) {
    Serial.print(Serial2.readString());
  }
  delay(50);
  Serial2.println(F("AT+HTTPPARA=CID,1")); 												//HTTP Parameter, Bearer Profile Identifier
  if (Serial2.available()) {
    Serial.print(Serial2.readString());
  }
  delay(50);
}

void GSM800Mega::SendData(String url)
{
  Serial2.print("AT+HTTPPARA=\"URL\",\"");
  Serial2.print(url);
  Serial2.println("\"");

  if (Serial2.available()) {
    Serial.print(Serial2.readString());
  }
  delay(500);

  Serial2.println("AT+HTTPACTION=0");
  if (Serial2.available()) {
    Serial.print(Serial2.readString());
  }
  delay(500);

  Serial2.println(F("AT+HTTPread"));
  if (Serial2.available()) {
    Serial.print(Serial2.readString());
  }
  delay(500);
}

String GSM800Mega::RecSms()
{
  //struct MyArray recsms;
  String myArray = "";
  String buff = "";
//  String cmp;
  char* buff1;
  char* subString;
  char* cmt = "+CMT:";
  char * pars;
  //Serial2.println("AT+CMGF=1");
  Serial2.println(F("AT+CNMI=2,2,0,0,0"));
  if (Serial2.available()) {
    buff = Serial2.readString();
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

void GSM800Mega::SendSms(String num, String text)
{
  Serial2.println(F("AT+CSCS=\"GSM\""));
  if (Serial2.available()) {
    Serial.print(Serial2.readString());
  }
  delay(50);
  Serial2.println(F("AT+CMGF = 1"));
  if (Serial2.available()) {
    Serial.print(Serial2.readString());
  }
  delay(50);
  //  Serial.println(num);
  //  Serial.println(text);
  Serial2.print(F("AT+CMGS=\""));
  Serial2.print(num);
  Serial2.println(F("\"\r"));
  delay(50);
  Serial2.println(text);
  delay(50);
  Serial2.println((char)26);
  delay(50);
  if (Serial2.available()) {
    Serial.print(Serial2.readString());
  }
  delay(50);
}

GSM800Mega gsm;

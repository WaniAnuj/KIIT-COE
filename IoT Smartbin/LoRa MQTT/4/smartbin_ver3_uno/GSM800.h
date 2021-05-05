/*
  GSM800.h - Library for sending Data to cloud using SIM800.
  Created by Shantanu. S. Mhapankar, November 11, 2016.
  Released into the public domain.
*/

#ifndef GSM800_H
#define GSM800_H

#include "Arduino.h"
#include <SoftwareSerial.h>


class GSM800
{
	public:
		GSM800(int Rx, int Tx);
		void SendData(String url);
		void Init();
		SoftwareSerial *SIM800;
    	String RecSms();
    	void SendSms(String num, String text);
};

#endif

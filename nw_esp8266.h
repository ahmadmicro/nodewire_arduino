#ifndef ESPCLIENT_H
#define ESPCLIENT_H

#include <nodewire.h>
#include <ESP8266WiFi.h>
#include <nstring.h>
//https://github.com/adafruit/ESP8266-Arduino
char* server = "138.197.6.173";
extern char instance[];

char* __ssid     = NULL;
char* __password = NULL;

void setWifiParams( char* ssid, char* pwd)
{
  __ssid = ssid;
  __password = pwd;
}

class ESPClient: public NodeWire
{
   WiFiClient client;

public:
  ESPClient()
  {
    message = new Message();
    myAddress = nodeName;
    myAddress = "node01";
    cmd = _cmd;
  }
  void begin(char* address)
  {
     myAddress = address;
     begin();
  }
  void begin()
  {
    Serial.begin(38400);
    WiFi.begin(__ssid, __password);

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      ESP.wdtFeed();
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    while(1)
    {
      Serial.print(".");
      ESP.wdtFeed();
      delay(1000);
      if (client.connect(server, 10001)) {
        Serial.println("connected");
        break;
      }
    }
  }

   void checkSend()
   {
       if(WiFi.status() != WL_CONNECTED)
       {
            ESP.restart();//https://github.com/esp8266/Arduino/blob/master/doc/boards.md#minimal-hardware-setup-for-bootloading-and-usage
       }
   }

   bool transmit(nString sender, nString response)
   {
     nString r(sendBuffer);
     if(strlen(sender.theBuf)>1)
        r = sender;
     else
        r = remote;
     r+=" ";
     r+=response; r+=" ";
     r+= instance;
     r+=":";
     r+= myAddress;
     client.println(sendBuffer);
     Serial.println(sendBuffer);

     return true;
   }

   void announciate()
   {
     if(ack==0)
     {
       if(millis() - ackcount >= 5000 && strlen(sendBuffer)==0)
       {
         ackcount = millis();
         nString response(sendBuffer);//todo using sendBuffer a bad idea?
         response = "cp ThisIs ";
         response+=instance;
         response+=":";
         response+=myAddress;
         client.print(sendBuffer);
       }
     }
   }

   void serialEvent()
   {
     while (client.available()) {
        char c = client.read();
        buffer[index] = c; index++;
        if (c == '\n' || c == '\r' || index >= sizeof(buffer)-1) {
          if(index !=0)
          {
               if(strncmp(buffer,instance, strlen(instance))==0)
               {
                   strncpy(buffer, buffer+strlen(instance)+1, strlen(buffer)-strlen(instance));
               }
               Serial.println(buffer);
               messageComplete = true;
               index = 0;
               return;
          }
        }
     }
   }
};

#endif

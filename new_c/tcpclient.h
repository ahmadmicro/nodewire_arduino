#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <nodewire.h>
#include <Ethernet.h>
#include <nstring.h>

IPAddress server(138,197,6,173);
char instance[] = "sadiqahmad2";
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

class TCPClient: public NodeWire
{
  EthernetClient client;

public:
  TCPClient()
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
    Ethernet.begin(mac);
    Serial.println("connecting");
    nString r(sendBuffer);
    while(1)
    {
      Serial.print(".");
      delay(1000);
      if (client.connect(server, 10001)) {
        Serial.println("connected");
        break;
      }
    }

    wdt_enable(WDTO_8S);
  }

   void checkSend()
   {

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
     wdt_reset();
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

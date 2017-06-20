#ifndef NW_COMPRESS_H
#define NW_COMPRESS_H

#include <nodewire.h>
#include <nstring.h>


class NW_Compressed: public NodeWire
{
   char compressedBuffer[32];

public:
  NW_Compressed()
  {
    message = new Message();
    myAddress = nodeName;
    myAddress = "node01";
    cmd = _cmd;
  }

  void compress()
  {
    SplitCommand(sendBuffer);
    compressedBuffer[0] = 0xAA;

    //Commands:ThisIs 1, ack 2, nodes 3, ports 4, set 5, get 6, portvalue 7, properties 8
    if(cmd == 'ThisIs')
    {
      compressedBuffer[1] = 0x01;
    }
    else if(cmd == 'ack')
      compressedBuffer[1] = 0x02;
    else if(cmd == 'nodes')
      compressedBuffer[1] = 0x03;
    else if(cmd == 'ports')
    {
       compressedBuffer[1] = 0x04;
    }
    else if(cmd == 'set')
      compressedBuffer[1] = 0x05;
    else if(cmd == 'get')
      compressedBuffer[1] = 0x6;
    else if(cmd == 'portvalue')
    else if(cmd == 'properties')
  }

  void decompress()
  {

  }

  bool transmit(nString sender, nString response)
  {
     if(strlen(compressedBuffer)==0)
     {
       nString r(sendBuffer);
       if(strlen(sender.theBuf)>1)
          r = sender;
       else
          r = remote;
       r+=" ";
       r+=response; r+=" "; r+= myAddress;
       compress();
       return true;
     }
     return false;
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

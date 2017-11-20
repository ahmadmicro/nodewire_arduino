#ifndef NW_COMPRESS_H
#define NW_COMPRESS_H

#include <nstring.h>

#define ThisIs 1
#define ackcmd 2
#define nodes 3
#define ports 4
#define set 5
#define get 6
#define portvalue 7
#define properties 8
#define ping 9
#define  any 1
#define cp 2

class Message
{
   public:
    	char Address[10];
    	char Command;
      char Params[10];
    	char Sender;
};

class NW_Compressed
{
    protected:
      char nodeName[15];
      char nodeNum;
      char mybroadcastaddress = any;
    public:
      nString myAddress;
      int ack = 0;
      boolean messageComplete = false;
      Message* message;
      nString cmd;
      char remote = cp;
      long sendDelay = 100;
      bool abort = false;
      int namelenght;

    protected:
    	char buffer[32];
    	int index;
    	long ackcount = 500000;
      long whenlastreceived;
      char sendBuffer[32];

public:
  NW_Compressed()
  {
    message = new Message();
    myAddress = nodeName;
    myAddress = " node01 "; //spaces needed as delimiters
    nodeNum = 0;
    namelenght = strlen(nodeName);
    ackcount = millis();
    ack = 0;
  }

  void begin(char* address)
  {
     //myAddress = " ";
     myAddress= address;
     //myAddress += " ";
     begin();
  }

  void begin()
  {
     Serial.begin(38400);
  }

  bool messageArrived()
  {
    serialEvent();
    announciate();
    checkSend();
    if(messageComplete)
    {
      SplitCommand(buffer);
      if((nodeNum==message->Address[0] || mybroadcastaddress==message->Address[0] || myAddress==message->Address) && nodeNum != message->Sender)
      {
          if(cmd == ping)
          {
            ack = 0;
          }
          else if(cmd == ackcmd)
          {
              ack = 1;
          }
          else
            return true;
      }
      resetmessage();
    }
    return false;
  }

  void createPacket(char node, char cmd, char* params )
  {
    // AANnCmPr0Pr1SnChkFF
    sendBuffer[0] = 0xAA;
    sendBuffer[1] = node;
    sendBuffer[2] =  cmd;
    int p =3;
    while(params[p-3]!=0)
    {
      sendBuffer[p] = params[p-3];
      p++;
    }

    if(nodeNum!=0)
      sendBuffer[p]=nodeNum;
    else
    {
      sendBuffer[++p] = ' ';
      int i;
      for(i=0; i<=strlen(nodeName);i++)
      {
        sendBuffer[p+i] = nodeName[i];
      }
      p+=i;
      sendBuffer[p++] = ' ';
    }

    char sum = 0;
    for(int i=0;i<=p;i++)
      sum+=sendBuffer[i];
    sendBuffer[p+1] = sum;
    sendBuffer[p+2] = 0xFF;

    //Serial.println(p+2);
  }

  void announciate()
  {
    if(ack==0)
    {
      if(millis() - ackcount >= 5000 && packaet_len(sendBuffer)==0)
      {
        ackcount = millis();
        createPacket(cp, ThisIs, "");
      }
    }
  }

  void serialEvent() {
    while (Serial.available()) { //todo detect start byte 0XAA
      // get the new byte:
      char inChar = (char)Serial.read();
      // add it to the inputString:

      if(!abort)
      {
        buffer[index] = inChar; index++;
      }

      if(index < 1 && nodeNum != 0 && buffer[1]!=nodeNum && buffer[1]!=any)
        abort = true;
      if (inChar == 0xFF || index >= 32) {
        messageComplete = true; //todo: checksum before setting
        index = 0;
        abort = false;
        whenlastreceived = millis();
        return;
      }

      whenlastreceived = millis();
    }
  }

  int packaet_len(char* packet)
  {
      int i = 0;
      while(packet[i]!=0xFF && i!=32) i++;
      return i;
  }

  int packate_cmp(char* pack1, char* pack2)
  {
    int i = 0;
    while(pack1[i]!=0xFf && pack2[i]!=0xFF && i <32)
    {
        if(pack1[i]>pack2[i]) return 1;
        if(pack1[i]<pack2[i]) return -1;
    }
    return 0;
  }

  bool transmit(char sender, char* response) {
    Serial.println(packaet_len(sendBuffer));
     if(packaet_len(sendBuffer)==0)
     {
       if(sender==0)
          sender = remote;
        createPacket(sender, response[0], response+1 );
        return true;
     }
     return false;
  }

  void checkSend(){
    if(sendBuffer[0]==0xAA && Serial.available()==0 && millis()-whenlastreceived>sendDelay)
    {
      int i = 0;
      do {
        Serial.print(sendBuffer[i++], HEX);
      } while(sendBuffer[i]!=0xFF && i<32);
      Serial.print(sendBuffer[i], HEX);
      Serial.println("");

      delay(20);//wait to receive copy of sent message
      serialEvent();//not tested.  receive a copy of the message sent
      if(packate_cmp(sendBuffer,buffer)!=0)
      {
        whenlastreceived = millis();
        return;//not tested. detect collision
      }
      memset(sendBuffer, '\0', sizeof(sendBuffer));
    }
  }

  void configureZigbee()
  {

  }

  void resetmessage(){
      messageComplete = false;
      memset(buffer, '\0', sizeof(buffer));
      message->Command = message->Sender = 0;
      for(int i = 0; i<=10; i++)
      {
          message->Params[i] = 0;
          message->Address[i] = 0;
      }
  }

  void SplitCommand(char* thebuff)
  {
    //AANnCmPr0Pr1SnChkFF
    message->Command = message->Sender = 0;
    for(int i = 0; i<=10; i++)
    {
        message->Params[i] = 0;
        message->Address[i] = 0;
    }

    int pos = 1;
    if(thebuff[1]== ' ')
    {
      int i = 1;
      do {
        message->Address[i] = thebuff[i]; i++;
      } while(thebuff[i]!=' ');
      pos = i;
    }
    else
    {
      message->Address[0] = thebuff[1];
      pos = 2;
    }
    message->Command = thebuff[pos];
    cmd = message->Command;

    int par = 0;
    while(thebuff[pos]!=0xFF && par!=10)
    {
      message->Params[par] == thebuff[pos];
      par++; pos++;
    }
    message->Sender = thebuff[pos-1];
  }
};

#endif

#ifndef BNODE_H
#define BNODE_H

#include <board.h>
#include <node.h>
#include <EEPROM.h>
class bNode: public Node
{
  protected:
    Board board;
  public:
    virtual bool get(nString port)
    {
      char temp[100]; nString response(temp);
      if (port == "properties")
      {
        response = "properties "; response += iot.message->Params[1]; response += " "; response += board.properties(iot.message->Params[1]);
      }
      else if (port == "ports" && board.noports !=0)
      {
        response = "ports "; response += board.getports();
      }
      else if(port=="memory")
      {
        response = "portvalue memory ";
        response += board.freeRam();
      }
      else if(port == "id")
      {
        int addr = 10;
        int i = 0;
        char buff[20]; nString dbuff(buff);
        memset(buff, '\0', sizeof(buff));
        while(i<20 && buff[i]!= ' ')
        {
           buff[i] = EEPROM.read(addr+i);
           i++;
        }
        if(buff[0]==0) dbuff = "none";
        response = "id "; response += buff;
      }
      else if(port == "name")
      {
          int addr = 30;
          int i = 0;
          char buff[20];
          memset(buff, '\0', sizeof(buff));
          while(i<20  && buff[i]!= ' ')
          {
            char ch = EEPROM.read(addr+i);
            if(ch <32 || ch > 126) break;
             buff[i] = ch;
             i++;
          }
          if(strlen(buff) != 0)
            iot.myAddress = buff;
          response = "ThisIs";
      }
      else
      {
        response = "portvalue "; response += port; response += " "; response += board.in(port);
      }
      iot.transmit(iot.message->Sender, response);
    }

    virtual void set(nString port)
    {
      char temp[50]; nString response(temp);
      if (port == "direction")
      {
        board.setdirection(iot.message->Params[1], iot.message->Params[2]);
        response = "properties "; response += iot.message->Params[1]; response += " "; response += board.properties(iot.message->Params[1]);
      }
      else if(port == "id")
      {
          int addr = 9;
          int len = strlen(iot.message->Params[1]);
          int i = 0;

          for(int j=9; j<=19;  j++)
          {
            EEPROM.write(j, 0);
          }

          while(i<len && i<19) EEPROM.write(addr+i, iot.message->Params[1][i++]);

          EEPROM.write(len, ' ');
          response = "id "; response += iot.message->Params[1];
      }
      else if(port  ==  "name")
      {
        int addr = 29;
        int len = strlen(iot.message->Params[1]);
        int i = 0;

        for(int j=29; j<=50;  j++)
        {
          EEPROM.write(j, 0);
        }
        while(i<len && i<20) EEPROM.write(addr+i, iot.message->Params[1][i++]);

        EEPROM.write(len, ' ');

        response = "ThisIs "; response += iot.message->Params[1];
        delay(10000);//to force a reset
      }
      else if(port == "reset")
      {
        for(int i=8; i<=50;  i++)
        {
          EEPROM.write(i, 0);
        }
        delay(10000);//to force a reset
      }
      else
      {
        board.out(iot.message->Params[0], iot.message->Params[1]);
        response = "portvalue "; response += port; response += " "; response += board.in(iot.message->Params[0]);
      }
      iot.transmit(iot.message->Sender, response);
    }

    virtual void loop()
    {
        board.checkinputs(&iot);
    }
};

#endif

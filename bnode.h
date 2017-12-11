#ifndef BNODE_H
#define BNODE_H

#ifdef ESPCLIENT_H
  #include <BOARD8266.h>
#else
  #include <board.h>
  #include <EEPROM.h>
#endif
#include <node.h>

class bNode: public Node
{
  protected:
      Board board;

  public:
    virtual void init()
    {
       iot.begin();
       get("name");
    }

    virtual void init(char* name)
    {
        iot.begin(name);
        get("name");
    }

    virtual bool get(nString port)
    {
      char temp[100]; nString response(temp, sizeof(temp));
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
        char buff[21]; nString dbuff(buff, sizeof(buff));
        memset(buff, '\0', sizeof(buff));
        while(i<20 && buff[i]!= ' ')
        {
           buff[i] = EEPROM.read(addr+i);
           i++;
        }
        if(buff[0]==0 || buff[0]==255) dbuff = "none";
        response = "id "; response += buff;
      }
      else if(port == "name")
      {
          int addr = 30;
          int i = 0;
          char buff[21];
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
      char temp[50]; nString response(temp, sizeof(temp));
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
          #ifdef BOARD8266_H
             EEPROM.commit();
          #endif

         #ifdef BOARD8266_H
          while(i<len && i<19) EEPROM.write(addr+i+1, iot.message->Params[1][i++]);
          #else
          while(i<len && i<19) EEPROM.write(addr+i, iot.message->Params[1][i++]);
         #endif

          EEPROM.write(len, ' ');
          response = "id "; response += iot.message->Params[1];
          #ifdef BOARD8266_H
             EEPROM.commit();
          #endif
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
        #ifdef BOARD8266_H
           EEPROM.commit();
        #endif
        while(i<len && i<20) EEPROM.write(addr+i, iot.message->Params[1][i++]);

        EEPROM.write(len, ' ');

        response = "ThisIs "; response += iot.message->Params[1];
        #ifdef BOARD8266_H
           EEPROM.commit();
           ESP.restart();
        #endif
        while(1);//to force a reset
      }
      else if(port == "reset")
      {
        for(int i=8; i<=511;  i++)
        {
          EEPROM.write(i, 0);
        }
        #ifdef BOARD8266_H
           EEPROM.commit();
           ESP.restart();
        #endif
        while(1);//to force a reset
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

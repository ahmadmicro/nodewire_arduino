#ifndef BOARD_H
#define BOARD_H
#include <node.h>
#include <avr/wdt.h>

extern Node* __thenode;

class Board
{
    int loopport = 0;
    unsigned long twohundred = 0;

    bool sleeping = false;
    int _duration = 0;
    long timeStarted = 0;

    bool in_changed = false;

    //response Queue
    int responseQueue[5]={-1,-1,-1,-1,-1};
    int popResponse()
    {
      int v = responseQueue[0];
      if(v !=-1)
      {
        for(int i=1;i!=5;i++)
          responseQueue[i-1] = responseQueue[i];
        responseQueue[4] = -1;
      }
      return v;
    }

    void pushResponse(int v)
    {
      for(int j=0;j<=4;j++)
          if(responseQueue[j]==v) return;

      //Serial.print("pushing pos:");
      int i = 0;
      while(responseQueue[i]!=-1 && i<4)
        i++;
      if(responseQueue[i]==-1)
          responseQueue[i] = v;
      //Serial.println(i);
    }
    // end response Queue
  public:

    double* value;
    char* direction ;
    nString* ports ;
    int* address;
    int noports = 0;


    int checkInterval = 200;
    float checkResolution = 0.25;

    void sleep(int duration)
    {
      timeStarted = millis();
      sleeping = true;
      _duration = duration;
    }
    void checkinputs(NodeWire* iot)
    {
      wdt_reset();
      checkinputs(iot, true);
    }
    int freeRam ()
    {
      extern int __heap_start, *__brkval;
      int v;
      return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
    }
    void checkinputs(NodeWire* iot, bool report)
    {
      if(sleeping && ((millis()-timeStarted)>_duration))
      {
        sleeping = false;
      }
      if(millis()-twohundred > checkInterval)
      {
        twohundred = millis();
        if(direction[loopport]==1)
        {
          double inval;

          if(address[loopport]==-1)
               inval = atof(__thenode->read(loopport).theBuf); //read(loopport);
          else if(address[loopport]==-2)
               inval = value[loopport]*checkResolution+0.01;
          else if(address[loopport]>=14)
               inval = analogRead(address[loopport])/1023.0;
           else
               inval= digitalRead(address[loopport]);

          if(abs(inval-value[loopport])>=checkResolution)
          {
            value[loopport] = inval;
            if(!in_changed)
            {
              in_changed = true;

              if(!sleeping && __thenode!=NULL)
                __thenode->changed(loopport);
              if(report && iot->ack)
              {
                char temp[40];
                nString resp(temp);
                resp = "portvalue "; resp += ports[loopport]; resp += " "; resp += (address[loopport]<=-1)?inval:__thenode->read(loopport);//tocheck
                bool result = iot->transmit(iot->remote, resp);
                if(result!=true)
                {
                  pushResponse(loopport);
                  //Serial.print("saving"); Serial.println(loopport);
                }
              }
              in_changed = false;
            }
          }
          else
          {
            //use window to send queued responses
            int lp = popResponse();
            if(lp!=-1)
            {
              //Serial.println("popping");
              char temp[50];
              nString resp(temp);
              resp = "portvalue "; resp += ports[lp]; resp += " "; resp += value[lp];
              if(!iot->transmit(iot->remote, resp)) pushResponse(lp);
            }
          }
        }
        if(loopport<(noports-1))
          loopport++;
         else
          loopport = 0;
      }
    }

    int getportindex(nString p)
    {
      for(int i=0;i<noports;i++)
        if(ports[i]==p)
          return i;
      return -1;
    }

    double getValue(nString p)
    {
      int ind = getportindex(p);
      if(ind!=-1)
      {
        double inval;

        if(address[loopport]==-1)
             inval = atof(__thenode->read(loopport).theBuf);
        else if(address[ind]>=14)
             inval = analogRead(address[ind])/1023.0;
         else
             inval= digitalRead(address[ind]);
         return inval;
      }
      else
        return -1;
    }
    void init(nString type)
    {
      init(18, type);
    }
    void init(int num_ports=18, nString type="other")
    {
      wdt_enable(WDTO_8S);
      noports=num_ports;
      if(type=="uno" && num_ports==18)
      {
	          noports=18;
            value = new (double[18]){1,1,1,1,1,  1,1,1,1,1,  1,1,1,1,1,  1,1,1};//stores the value of the port
            direction = new (char[18]){0,0,0,0,0,  0,0,0,0,0,  0,0,0,0,0,  0,0,0};//stores the direction of the port, 1=in, 0=out
            ports = new (nString[18]){"2","3","4","5","6","7","8","9","10","11","12","13","A0","A1","A2","A3","A4","A5"};
            address = new (int[18]){2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
      }
      for(int port=0;port<noports;port++)
      {
       if(direction[port]==1)
       {
           pinMode(address[port], INPUT_PULLUP);
           //digitalWrite(address[port], 1);
       }
       else if(direction[port]==0)
       {
           pinMode(address[port], OUTPUT);

           if(outputtype(port)=="PWM")
               analogWrite(address[port], value[port]);
           else if(outputtype(port)=="Digital")
               digitalWrite(address[port], value[port]);
           //else
            //   __thenode->write(port, value[port]); //write(port, value[port]);
       }
      }
    }

    int out(char* port, char* v)
    {
        double val = atof(v);
        long lval = val * 255;
        int portindex  = getportindex(port);
        //String response;
        if(direction[portindex]==0)
        {
          if(value[portindex] != val && value[portindex] != lval || address[portindex]==-2)
          {
            value[portindex] = val;
            if(outputtype(portindex)=="PWM")
                analogWrite(address[portindex], lval);
            else if(outputtype(portindex)=="Digital")
                digitalWrite(address[portindex], val);
            else if(address[portindex]==-1)
                __thenode->write(portindex, val);//write(portindex, lval);
            else
                __thenode->write(portindex, v);//write(portindex, lval);
            if(!__thenode->get(port)) pushResponse(portindex);
          }
	        return 0;

        }
	      return -1;
    }

    nString in(nString port)
    {
        int portindex = getportindex(port);
        if(portindex!=-1)
      	    return nString((address[portindex]!=-2)?value[portindex]:  __thenode->read(portindex));
      	else
      	    return nString("error");
    }

    void setdirection(nString port, nString direction1)
    {
       int portindex  = getportindex(port);
       if(direction1=="in")
       {
            pinMode(address[portindex], INPUT_PULLUP);
            direction[portindex] = 1;
            //digitalWrite(address[portindex], 1);
       }
       else if(direction1=="out")
       {
            pinMode(address[portindex], OUTPUT);
            direction[portindex] = 0;
       }
    }

    nString properties(char* port)
    {
        char temp[15]; nString prop(temp); prop.clearBuffer(15);
        int portindex  = getportindex(port);
        prop = (direction[portindex]==0?outputtype(portindex): inputtype(portindex));
        prop += (direction[portindex]==1?" IN":" OUT");
    }

    nString getports()
    {
        char temp[100]="";//tocheck can't return stack based buffer
        nString ps(temp);
	      for(int i=0;i<noports;i++)
        {
	          ps+=ports[i]; ps += " ";
         }
	      return ps;
    }

    nString inputtype(int pin)
    {
        if(address[pin]>=14)
          return nString("Analog");
        else if (address[pin] =<  -1)
            return "Custom";
        else
          return nString("Digital");
    }

    nString outputtype(int pin)
    {
        if(address[pin]==3 || address[pin]==5 || address[pin]==6 || address[pin]==9 || address[pin]==10 || address[pin]==11)
          return nString("PWM");
        else if (address[pin] =<  -1)
          return "Custom";
        else
          return nString("Digital");
    }
};

#endif

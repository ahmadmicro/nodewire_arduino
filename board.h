#ifndef BOARD_H
#define BOARD_H
#include <node.h>

extern Node* thenode;

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
      for(int i=1;i!=4;i++)
        responseQueue[i-1] = responseQueue[i];
      return v;
    }

    void pushResponse(int v)
    {
      Serial.println("pushing");
      int i = 0;
      while(responseQueue[i]!=-1 && i<=4)
        i++;
      if(responseQueue[i]==-1)
          responseQueue[i] = v;
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
      checkinputs(iot, true);
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

          if(address[loopport]>=14)
               inval = analogRead(address[loopport])/1023.0;
           else
               inval= digitalRead(address[loopport]);

          if(abs(inval-value[loopport])>=checkResolution)
          {
            if(!in_changed)
            {
              in_changed = true;

              if(!sleeping && thenode!=NULL)
                thenode->changed(loopport);
              if(report && iot->ack)
              {
                char temp[40];
                nString resp(temp);
                resp = "portvalue "; resp += ports[loopport]; resp += " "; resp += inval;//tocheck
                bool result = iot->transmit(iot->remote, resp);
                if(result==true)
                {
                  value[loopport] = inval;
                }
                else
                {
                  pushResponse(loopport);
                  Serial.print("saving"); Serial.println(loopport);
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
              Serial.println("popping");
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

        if(address[ind]>=14)
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
      noports=num_ports;
      if(type=="uno" && num_ports==18)
      {
	          noports=18;
            value = new (double[18]){1,1,1,1,1,  1,1,1,1,1,  1,1,1,1,1,  1,1,1};//stores the value of the port
            direction = new (char[18]){0,0,0,0,0,  0,0,0,0,0,  0,0,0,0,0,  0,0,0};//stores the direction of the port, 1=in, 0=out
            ports = new (nString[18]){"2","3","4","5","6","7","8","9","10","11","12","13","A0","A1","A2","A3","A4","A5"};
            address = new (int[18]){2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
      }
      /*else if(num_ports<20)
      {
	    noports=num_ports;
	    value = new double[num_ports];
            direction = new char[num_ports];
            ports = new String[num_ports];
            address = new int[num_ports];
      }*/
      for(int port=0;port<noports;port++)
      {
       if(direction[port]==1)
       {
           pinMode(address[port], INPUT);
           //digitalWrite(address[port], 1);
       }
       else if(direction[port]==0)
       {
           pinMode(address[port], OUTPUT);
           digitalWrite(address[port], value[port]);
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
          value[portindex] = val;
          if(outputtype(portindex)=="PWM")
              analogWrite(address[portindex], lval);
          else
              digitalWrite(address[portindex], val);
          thenode->get(port);
	        return 0;
        }
	      return -1;
    }

    nString in(nString port)
    {
        int portindex = getportindex(port);
        if(portindex!=-1)
      	    return nString(value[portindex]);
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
       }
       else if(direction1=="out")
       {
            pinMode(address[portindex], OUTPUT);
            direction[portindex] = 0;
       }
    }

    nString properties(char* port)
    {
        char temp[15]; nString prop(temp);
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
        else
          return nString("Digital");
    }

    nString outputtype(int pin)
    {
        if(address[pin]==3 || address[pin]==5 || address[pin]==6 || address[pin]==9 || address[pin]==10 || address[pin]==11)
          return nString("PWM");
        else
          return nString("Digital");
    }
};

#endif

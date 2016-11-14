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
  public:
    double* value;
    char* direction ;
    String* ports ;
    int* address;
    int noports = 0;
    String ps="";

    int checkInterval = 200;
    float checkResolution = 0.2;

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
    void checkinputs(NodeWire iot)
    {
      checkinputs(&iot, true);
    }
    void checkinputs(NodeWire* iot, bool report)
    {

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
              value[loopport] = inval;
              if(!sleeping && thenode!=NULL)
                thenode->changed(loopport);
              if(sleeping && (millis()-timeStarted)>_duration)
              {
                sleeping = false;
              }
              if(report && iot->ack)
              {
                String resp = "portvalue " + String(ports[loopport]) + " " + String(inval);
                iot->transmit(resp);
              }
              in_changed = false;
            }
          }
        }
        if(loopport<(noports-1))
          loopport++;
         else
          loopport = 0;
      }
    }

    int getportindex(String p)
    {
      for(int i=0;i<noports;i++)
        if(ports[i]==p)
          return i;
      return -1;
    }

    double getValue(String p)
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

    void init(int num_ports=18, String type="uno")
    {
      noports=num_ports;
      if(type=="uno" && num_ports==18)
      {
	          noports=18;
            value = new (double[18]){1,1,1,1,1,  1,1,1,1,1,  1,1,1,1,1,  1,0.5,0.5};//stores the value of the port
            direction = new (char[18]){0,0,0,0,0,  0,0,0,0,0,  0,0,0,0,0,  0,0,1};//stores the direction of the port, 1=in, 0=out
            ports = new (String[18]){"2","3","4","5","6","7","8","9","10","11","12","13","A0","A1","A2","A3","A4","A5"};
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
           pinMode(address[port], INPUT_PULLUP);
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
        int portindex  = getportindex(String(port));
        //String response;
        if(direction[portindex]==0)
        {
          value[portindex] = val;
          if(outputtype(portindex)=="PWM")
              analogWrite(address[portindex], lval);
          else
              digitalWrite(address[portindex], val);
	        return 0;
        }
	      return -1;
    }

    String in(char* port)
    {
        int portindex = getportindex(String(port));
        if(portindex!=-1)
      	    return String(value[portindex]);
      	else
      	    return String("error");
    }

    void setdirection(String port, String direction1)
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

    String properties(char* port)
    {
        int portindex  = getportindex(String(port));
        return (direction[portindex]==0?outputtype(portindex): inputtype(portindex)) + (direction[portindex]==1?" IN":" OUT");
    }

    String getports()
    {
        if(ps == "")
        {
    	     for(int i=0;i<noports;i++)
    	          ps = ps+ports[i] + " ";
        }
	      return ps;
    }

    String inputtype(int pin)
    {
        if(address[pin]>=14)
          return String("Analog");
        else
          return String("Digital");
    }


    String outputtype(int pin)
    {
        if(address[pin]==3 || address[pin]==5 || address[pin]==6 || address[pin]==9 || address[pin]==10 || address[pin]==11)
          return String("PWM");
        else
          return String("Digital");
    }
};

#endif

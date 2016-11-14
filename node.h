#ifndef NODE_H
#define NODE_H

class Node
{
      long timerValue;
      int timerAfter = 0;
      bool timerEnabled = false;

    public:
     NodeWire iot;


     void startTimer(int duration)
     {
       timerAfter = duration;
       timerEnabled = true;
       timerValue = millis();
     }

     void stopTimer()
     {
       timerEnabled = false;
     }

     bool checkTimer()
     {
       if(timerEnabled && (millis()-timerValue>=timerAfter))
       {
         timerValue = millis();
         timer();
       }
     }

     virtual void set(String port)
     {
     }
     virtual void get(String port)
     {
     }
     virtual void init()
     {
	      iot.begin();
     }
     virtual void loop()
     {
       //make this function as short as possible in order not to impact the realtime performance of the software
     }
     virtual void changed(int portindex)
     {
        //make this function as short as possible.
        //if it takes too long, you might miss subsequent calls.
     }
     virtual void timer()
     {

     }
};


extern Node* thenode;

void setup() {
   thenode->init();
}

void wait(long ms)
{
   long now = millis();
   while((millis()-now)<ms)
       loop();
}

void loop() {
  thenode->loop();
  thenode->checkTimer();
  if (thenode->iot.messageArrived())
  {
        if(thenode->iot.cmd == "set" || thenode->iot.cmd == "setportvalue")
        {
            String port = thenode->iot.message->Params[0];
            thenode->set(port);
        }
        else if(thenode->iot.cmd == "get" || thenode->iot.cmd == "getportvalue")
        {
             String port = thenode->iot.message->Params[0];
             thenode->get(port);
        }
        /*else if(thenode->iot.cmd == "getnoports")
        {
           String response = String("noports 14");
           thenode->iot.transmit(response);
        }*/
        else if(thenode->iot.cmd == "version")
        {
           String response = String("PlainTalk v2.0");
           thenode->iot.transmit(response);
        }
        else
        {
             thenode->iot.transmit(String("unknown command ") + thenode->iot.cmd);
        }
        thenode->iot.resetmessage();
  }
}

#endif

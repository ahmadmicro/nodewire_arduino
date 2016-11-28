#ifndef NODE_H
#define NODE_H

int freeRam ()
{
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

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
       if(timerEnabled && (millis()-timerValue)>=timerAfter)
       {
         timerValue = millis();
         timer();
       }
     }

     virtual void handleMessage()
     {
       if(iot.cmd == "set" || iot.cmd == "setportvalue")
       {
           nString port = iot.message->Params[0];
           set(port);
       }
       else if(iot.cmd == "get" || iot.cmd == "getportvalue")
       {
            nString port = iot.message->Params[0];
            if(port=="memory")
            {
              char temp[50]; nString result(temp);
              //result = thenode->iot.message->Sender;
              result = "portvalue memory ";
              result += freeRam();

              iot.transmit(iot.message->Sender, result);
            }
            else
              get(port);
       }
       /*else if(thenode->iot.cmd == "getnoports")
       {
          String response = String("noports 14");
          thenode->iot.transmit(response);
       }*/
       else if(iot.cmd == "version")
       {
          nString response = "PlainTalk v2.0";
          iot.transmit(iot.message->Sender, response);
       }
       else if(iot.cmd == "unknown")
       {/*ignore unknown */}
       else
       {
           char temp[50]; nString response(temp);//todo what if the command is very long?
           response = "unknown command ";
           response += iot.cmd;

           iot.transmit(iot.message->Sender, response);
       }
     }

     virtual void set(nString port)
     {
     }
     virtual bool get(nString port)
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
        thenode->handleMessage();
        thenode->iot.resetmessage();
  }
}

#endif

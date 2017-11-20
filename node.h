#ifndef NODE_H
#define NODE_H


class Node
{
      long timerValue;
      int timerAfter = 0;
      bool timerEnabled = false;

    public:
     #ifdef TCPCLIENT_H
         TCPClient iot;
     #else
       #ifdef MODEM_CLIENT_H
          ModemClient iot;
       #else
          #ifdef ESPCLIENT_H
             ESPClient iot;
          #else
             NodeWire iot;
          #endif
       #endif
     #endif

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

     virtual void go_to_sleep(){}
     virtual void wake_up(){}

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
       else if(iot.cmd == "unknown" || iot.cmd == "portvalue")
       {/*ignore unknown */}
       else if(iot.message->Address != "any")
       {
           char temp[40]; nString response(temp);//todo what if the command is very long?
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

     virtual nString read(int portIndex)
     {
       //override this to implement a custom port reader
       return "0";
     }

     virtual void write(int portIndex, nString val)
     {
       //override to implement custom port writer
     }

     virtual void init(char* name)
     {
         iot.begin(name);
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

Node* __thenode;

void wait(long ms)
{
   long now = millis();
   while((millis()-now)<ms)
       loop();
}

void loop() {
  __thenode->loop();
  __thenode->checkTimer();
  if (__thenode->iot.messageArrived())
  {
        __thenode->handleMessage();
        __thenode->iot.resetmessage();
  }
}

void setNode(Node* node)
{
  __thenode = node;
  __thenode->init();
}




#endif

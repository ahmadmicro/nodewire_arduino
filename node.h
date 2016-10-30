

class Node
{
    public:
     NodeWire iot;
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
     }
};


extern Node* thenode;

void setup() {
   thenode->init();
}


void loop() {
  thenode->loop();
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

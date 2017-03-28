/*
Copyright (c) 2016, nodewire.org
All rights reserved.
*/

#include <nodewire.h>
#include <node.h>

class myNode: public Node
{
  nString response = nString(new char[100]);
  int led = 13;
  nString val = "blink"; //on, off or blink, reserve enough space for the longest string that will be used

  public:
  void init()
  {
    Node::init("blinky");
    pinMode(led, OUTPUT);
  }

  bool get(nString port)
  {
       if (port == "led")
       {
          response = "portvalue led "; response +=  val;
       }
       else if(port == "ports")
       {
          response = "ports led ";
       }
       else if(port == "properties")
       {
          response = "properties led LIST on off blink";
       }
       iot.transmit(iot.message->Sender, response);
  }

  void set(nString port)
  {
      if(port == "led")
      {
          val = iot.message->Params[1];
          stopTimer();
          if(val == "on")
              digitalWrite(led, 1);
           else if(val == "off")
              digitalWrite(led, 0);
           else if(val == "blink")
               startTimer(500);
          response = "portvalue led "; response += val;
      }
      iot.transmit(iot.message->Sender, response);
  }

  bool ledon = false;
  void timer()
  {
    if(ledon) digitalWrite(led, 1); else digitalWrite(led, 0);
    ledon = !ledon;
  }
};

void setup() {
   setNode(new myNode());
}

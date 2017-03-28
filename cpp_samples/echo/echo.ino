/*
Copyright (c) 2016, nodewire.org
All rights reserved.
*/

#include <nodewire.h>
#include <node.h>

class myNode: public Node
{
  public:
  void init()
  {
    iot.begin("echo");
  }

  void handleMessage() {
      nString response(new char[100]);
      response = iot.message->Command;
      int i = 0;
      while(iot.message->Params[i+1]!=NULL)
      {
        response+=" ";
        response+=iot.message->Params[i++];
      }
      iot.transmit(iot.message->Sender, response);
  }
};

void setup() {
   setNode(new myNode());
}

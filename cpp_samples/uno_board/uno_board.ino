/*
Copyright (c) 2016, nodewire.org
All rights reserved.
*/
#include <nodewire.h>
#include <board.h>
#include <bnode.h>

class myNode: public bNode
{
public:
  void init()
  {
    iot.begin("node02");
    board.init("uno");
  }
};

void setup() {
   setNode(new myNode());
}

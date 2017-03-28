/*
Copyright (c) 2016, nodewire.org
All rights reserved.
*/

#include <nodewire.h>
#include <bnode.h>

class myNode: public bNode
{
  public:
  void init()
  {
    iot.begin("node01");

    board.value = new (double[1]) { 0 };
    board.direction = new (char[1]) { 0 };
    board.ports = new (nString [1]) {"led"};
    board.address = new (int[1]) {13};
    board.init(1);
  }
};

void setup() {
   setNode(new myNode());
}

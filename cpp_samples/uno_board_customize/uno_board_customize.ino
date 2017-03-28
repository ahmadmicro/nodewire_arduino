/*
Copyright (c) 2016, nodewire.org
All rights reserved.
*/

#include <nodewire.h>
#include <board.h>
#include <bnode.h>

#define nopins 18

class myNode: public bNode
{
public:

  void init()
  {
    Node::init("node02");

    board.value = new (double[nopins]){0,0,0,0,0,  0,0,0,0,0,  0,0,0,0,0,  0,0,0};//stores the value of the port
    board.direction = new (char[nopins]){1,1,0,0,0,  0,0,0,0,0,  0,0,0,0,0,  0,0,1};//stores the direction of the port, 1=in, 0=out
    board.ports = new (nString[nopins]){"2","3","4","5","6","7","8","9","10","11","12","13","A0","A1","A2","A3","A4","A5"};//the port name
    board.address = new (int[nopins]){2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};//the pin that will be mapped to the port
    board.init(nopins);
  }
};


void setup() {
   setNode(new myNode());
}

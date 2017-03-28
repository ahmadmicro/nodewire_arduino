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
      Node::init("basic");
    }
};

void setup() {
   setNode(new myNode());
}

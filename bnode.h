#ifndef BNODE_H
#define BNODE_H

#include <board.h>
#include <node.h>
class bNode: public Node
{
  protected:
    Board board;
  public:
    virtual void get(String port)
    {
      String response;
      if (port == "properties")
      {
        response = String("properties ") + iot.message->Params[1] + " " + board.properties(iot.message->Params[1]);
      }
      else if (port == "ports")
      {
        response = "ports " + board.getports();
      }
      else
      {
        response = "portvalue " + port + " " + board.in(iot.message->Params[0]);
      }
      iot.transmit(response);
    }

    virtual void set(String port)
    {
      String response;
      if (port == "direction")
      {
        board.setdirection(iot.message->Params[1], iot.message->Params[2]);
        response = String("properties ") + iot.message->Params[1] + " " + board.properties(iot.message->Params[1]);
      }
      else
      {
        board.out(iot.message->Params[0], iot.message->Params[1]);
        response = String("portvalue ") + port + " " + board.in(iot.message->Params[0]);
      }
      iot.transmit(response);
    }

    virtual void loop()
    {
        board.checkinputs(iot);
    }
};

#endif

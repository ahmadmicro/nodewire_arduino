#ifndef BNODE_H
#define BNODE_H

#include <board.h>
#include <node.h>
class bNode: public Node
{
  protected:
    Board board;
  public:
    virtual bool get(nString port)
    {
      char temp[100]; nString response(temp);
      if (port == "properties")
      {
        response = "properties "; response += iot.message->Params[1]; response += " "; response += board.properties(iot.message->Params[1]);
      }
      else if (port == "ports")
      {
        response = "ports "; response += board.getports();
      }
      else
      {
        response = "portvalue "; response += port; response += " "; response += board.in(port);
      }
      iot.transmit(iot.message->Sender, response);
    }

    virtual void set(nString port)
    {
      char temp[50]; nString response(temp);
      if (port == "direction")
      {
        board.setdirection(iot.message->Params[1], iot.message->Params[2]);
        response = "properties "; response += iot.message->Params[1]; response += " "; response += board.properties(iot.message->Params[1]);
      }
      else
      {
        board.out(iot.message->Params[0], iot.message->Params[1]);
        response = "portvalue "; response += port; response += " "; response += board.in(iot.message->Params[0]);
      }
      iot.transmit(iot.message->Sender, response);
    }

    virtual void loop()
    {
        board.checkinputs(&iot);
    }
};

#endif

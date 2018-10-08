#include <nlink.h>
#ifndef VIRTUALLINK_H
#define  VIRTUALLINK_H

class VirtualLink: public Link
{
private:
    Link* parent=NULL;
public:
  VirtualLink()
  {
    index = 0;
    messageComplete = false;
    message.setBuffer(in_buff,BUFF_SIZE);
  }

  void begin(Link* parentLink)
  {
      parent = parentLink;
  }

  void receive()
  {
    if (parent->message_received()==true) {
        messageComplete = true;
        message = parent->message;
    }
  }

  void new_message(nString& msg)
  {
      message = msg;
      message.trim();
      messageComplete = true;
  }

  void checkSend() {
    if(strlen(out_buff)!=0)
    {
      parent->response =  response;
      memset(out_buff, '\0', sizeof(out_buff));
    }
  }

};

#endif

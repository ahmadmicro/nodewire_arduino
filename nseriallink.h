#include <nlink.h>
#ifndef SERIALLINK_H
#define  SERIALLINK_H

class SerialLink: public Link
{
private:
  Stream* _serial;
  long whenlastreceived;
  int sendDelay;

public:
  SerialLink()
  {
    index = 0;
    messageComplete = false;
    message.setBuffer(in_buff,BUFF_SIZE);
    sendDelay = random(30, 100);
  }

  void setSerial(Stream* serial)
  {
      _serial = serial;
  }

  void receive()
  {
    while (_serial->available() && !messageComplete) {
      char inChar = (char)_serial->read();
      whenlastreceived = millis();

      if (inChar == '\n' || index >= (sizeof(in_buff)-2)) {
        messageComplete = true;
        in_buff[index]=0;
        index = 0;
        return;
      }
      else
      {
        in_buff[index] = inChar;
        index++;
      }
    }
  }

  void checkSend() {
    if(strlen(out_buff)!=0 && _serial->available()==0 && millis()-whenlastreceived>sendDelay)
    {
      _serial->println(out_buff);
      /*delay(20);//wait to receive copy of sent message
      receive();//not tested. receive a copy of the message sent
      if(strncmp(out_buff, in_buff, strlen(in_buff))!=0)
      {
        whenlastreceived = millis();
        return;//not tested. detect collision
      }*/
      memset(out_buff, '\0', sizeof(out_buff));
    }
  }

};

#endif

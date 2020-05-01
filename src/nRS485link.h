#include <nlink.h>
#ifndef RS485LINK_H
#define  RS485LINK_H
#define TRANSMIT HIGH
#define RECEIVE LOW

class RS485Link: public Link
{
private:
  Stream* _serial;
  long whenlastreceived;
  int sendDelay;
  int en1 = 2, en2 = 3;

public:
  RS485Link(int en1=2, int en2=3)
  {
    index = 0;
    messageComplete = false;
    message.setBuffer(in_buff,BUFF_SIZE);

    pinMode(en1, OUTPUT);
    pinMode(en2, OUTPUT);
    setmode(RECEIVE);
  }

  void setmode(int val)
  {
    digitalWrite(en1, val);
    digitalWrite(en2, val);
  }

  void begin(Stream* serial)
  {
      randomSeed(analogRead(0));
      _serial = serial;
      sendDelay = random(30, 100);
      Serial.println(sendDelay);
  }

  void receive()
  {
    while (_serial->available() && !messageComplete) {
      char inChar = (char)_serial->read();
      whenlastreceived = millis();

      if (inChar == '\n' || index >= (sizeof(in_buff)-2)) {
        debug.log2(in_buff);
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
      setmode(TRANSMIT);
      _serial->println(out_buff);
      _serial->flush();
      setmode(RECEIVE);
      //delay(10);//wait to receive copy of sent message
      /*receive();//not tested. receive a copy of the message sent
      Serial.println(strlen(in_buff));
      if(strncmp(out_buff, in_buff, strlen(in_buff))!=0)
      {
        whenlastreceived = millis();
        return;//not tested. detect collision
      }*/
      //Serial.println(in_buff);
      memset(out_buff, '\0', sizeof(out_buff));
    }
  }

};

#endif

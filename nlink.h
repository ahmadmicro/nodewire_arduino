#ifndef LINK_H
#define LINK_H

#ifdef ESP8266
  #define BUFF_SIZE 150
#else
  #if defined (__STM32F1__)
    #define BUFF_SIZE 100
  #else
    #if  defined (STM32_HIGH_DENSITY)
       #define BUFF_SIZE 100
    #else
       #define BUFF_SIZE 80
    #endif
  #endif
#endif

class Link
{
  protected:
    char in_buff[BUFF_SIZE];
    char out_buff[BUFF_SIZE];
    int index;
    bool messageComplete;

  public:
    nString message;
    nString response;
    nString* nodename = NULL;

    Link()
    {
      index = 0;
      messageComplete = false;
      message.setBuffer(in_buff,BUFF_SIZE);
      response.setBuffer(out_buff, BUFF_SIZE);
    }

    void resetmessage()
    {
      messageComplete = false;
      message.collapse();
      memset(in_buff, '\0', BUFF_SIZE);
    }

    bool messageArrived()
    {
      if(!messageComplete) // if no message waiting for processing, then receive new message
        receive();
      else
      {
        int len = message.split(' ');
        if(len==5)
          message.convert_object("address command port value sender");
        else if(len==4)
          message.convert_object("address command port sender");
        else if(len==3)
          message.convert_object("address command sender");
        else
        {
          resetmessage();
          return false;
        }
        if(message["address"]==*nodename || message["address"]=="any")
          return true;
        else
        {
            resetmessage();
            return false;
        }
      }

      checkSend();

      return false;
    }

    virtual void receive(){
    }
    virtual void checkSend() {}

};

#endif

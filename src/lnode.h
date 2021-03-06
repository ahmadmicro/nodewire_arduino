#ifndef LNODE_H
#define LNODE_H

#include <nstring2.h>
#include <nlink.h>
#include <nEEPROMFile.h>
#include <nport.h>

#define NO_EEPROM_FILES 5

#ifdef ESP8266
  #include <tr1/functional>
  extern "C" {
    #include "user_interface.h"
  }
  int _freeRam()
  {
    return (int) system_get_free_heap_size();
  }
#elif defined (__STM32F1__)
  int _freeRam (){ return 0;}
#elif defined (STM32_HIGH_DENSITY)
  int _freeRam (){ return 0;}
#elif defined ESP32
  #include <tr1/functional>
  int _freeRam()
  {
    return (int) system_get_free_heap_size();
  }
#else
  int _freeRam ()
  {
    extern int __heap_start, *__brkval;
    int v;
    return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
 }
#endif

#define MAX_TIMERS 3

#define digitalPins(ports, pins) [](nString port, nString val) { \
  int pp = ports.find(port);                                     \
  if(pp!=-1)                                                     \
      digitalWrite(pins[pp],(int)val);                           \
}

#define digitalPin(pin) [](nString val, nString sender) {digitalWrite(pin,(int)val);}  
#define outputValue(var) []() -> nString {  return var;}        

typedef  void (*timerHandler)();

template <class PVT>
class Node
{
private:
  Link* _link = NULL;
  setHandler* set_handlers = NULL;
  readHandler* read_handlers = NULL;
  timerHandler* timer_handlers = NULL;

  PVT* portvalues=NULL;
  Port<PVT>* port=NULL;
  void* remote = NULL;
  nString remote_address;
  #if defined(ESP8266) || defined(ESP32)
  std::tr1::function<void(nString port, nString value)> remote_handle;
  #endif

  int no_timers = 0;
  long* timer_intervals = NULL;
  long* timer_values = NULL;
  bool* timer_enabled = NULL;

  char _name[12];
  //char _id[12];

  bool announcing = true;
  long last_announced;

  bool with_props = false;

public:
  nString inputs;
  nString outputs;
  nString address;
  nString id;

  setHandler set_portvalue = NULL;
  getHandler get_portvalue = NULL;

  Node()
  {
    timer_handlers = new timerHandler[MAX_TIMERS];
    timer_intervals = new long[MAX_TIMERS];
    timer_values = new long[MAX_TIMERS];
    timer_enabled = new bool[MAX_TIMERS];

    //id.setBuffer(_id, sizeof(_id));
  }

  ~Node()
  {
    if(set_handlers!=NULL) free(set_handlers);
    if(read_handlers!=NULL) free(read_handlers);
    if(timer_handlers!=NULL)
    {
      free(timer_handlers);
      free(timer_intervals);
      free(timer_values);
      free(timer_enabled);
    }
    if(portvalues!=NULL) free(portvalues);
    if(remote!=NULL) free(remote);
  }

  void readConfig()
  {
    EEPROM_File file;
    char content[30];
    nString cont(content, sizeof(content));
    if(file.no_files()==-1) {
        file.format();
        file.create_FS(NO_EEPROM_FILES);
    }
    if(file.open("nw.cfg", cont))
    {
        cont.split(' ');
        address = cont[0];
        id = cont[1];
    }
    else
    {
       id = "none";
       file.create_file("nw.cfg", 30);
       for(int i=0; i<strlen(_name);i++)
       {
          if(_name[i]=='#') _name[i] = (char) random('0', ':');
       }
       cont.create_array(2);
       cont.append(address);
       cont.append(id);
       cont.join(' ');
       file.save("nw.cfg", cont);
    }
  }

  void saveConfig()
  {
    EEPROM_File file;
    char content[30];
    nString cont(content, sizeof(content));
    cont.create_array(2);
    cont.append(address);
    cont.append(id);
    cont.join(' ');
    file.save("nw.cfg", cont);
  }

  void init(nString nodename)
  {
    inputs.split(' ');
    outputs.split(' ');

    set_handlers = new setHandler[inputs.len];
    read_handlers = new readHandler[outputs.len];
    portvalues = new PVT[inputs.len];

    for(int i=0;i<inputs.len;i++) { portvalues[i] = *(new PVT()); portvalues[i]=0;}

    for(int i=0;i<outputs.len;i++)
    {
      read_handlers[i] = NULL;
    }

    for(int i=0;i<inputs.len;i++)
    {
      set_handlers[i]  = NULL;
    }

    address.setBuffer(_name, sizeof(_name));
    address = nodename;
    readConfig();

    with_props = false;

    if(_freeRam()<700)
    {
        Serial.println(_freeRam());
        debug.log("Memory LoW!\nHalting");
        while(1);
    }
  }

  void init_with_props(nString nodename)
  {
    address.setBuffer(_name, sizeof(_name));
    address = nodename;
    readConfig();

    inputs.split(' ');
    outputs.split(' ');

    set_handlers = new setHandler[inputs.len];
    read_handlers = new readHandler[outputs.len];
    portvalues = new PVT[inputs.len];

    for(int i=0;i<inputs.len;i++){ portvalues[i] = *(new PVT()); portvalues[i]=0;}

    for(int i=0;i<outputs.len;i++)
    {
      outputs[i].split(':');
      outputs[i].convert_object("name prop");
      read_handlers[i] = NULL;
    }

    for(int i=0;i<inputs.len;i++)
    {
      inputs[i].split(':');
      inputs[i].convert_object("name prop");
      set_handlers[i]  = NULL;
    }

    with_props = true;
  }

  void init(nString nodename, Link* link)
  {
    init(nodename);
    setLink(link);
  }

  void setLink(Link* link)
  {
    _link = link;
    _link->nodename = &address;
  }

  void on(nString port, setHandler handler)
  {
    int index = inputs.find(with_props?"name="+port:port);
    if(index!=-1) set_handlers[index] = handler;
  }

  void timer(long duration, timerHandler handler)
  {
    if(no_timers<=2)
    {
        timer_handlers[no_timers] = handler;
        timer_intervals[no_timers] = duration;
        timer_enabled[no_timers] = false;
        no_timers++;
    }
  }

  void update_time(int t, long duration)
  {
    if(t<no_timers)
      timer_intervals[t] = duration;
  }

  void on(nString port, readHandler handler)
  {
    int index = outputs.find(with_props?"name="+port:port);
    if(index!=-1) read_handlers[index] = handler;
  }

  void operator>>(setHandler handler){
    set_portvalue = handler;
  }

  void operator<<(getHandler handler){
    get_portvalue = handler;
  }

  Port<PVT>& operator[](const char* p)
  {
    int pp = outputs.find(p);
    if(port!=NULL)
    {
      delete port;
      port = NULL;
    }
    if(pp!=-1)
    {
      if(strlen(_link->response.theBuf)!=0) _link->checkSend();
      port = new Port<PVT>(&address, p, &_link->response, NULL);
      port->handler.get_handler = &read_handlers[pp];
      return *port;
    }
    else if(inputs.find(p)!=-1)
    {
      pp = inputs.find(p);
      port = new Port<PVT>(&address, p, &_link->response, &portvalues[inputs.find(p)]);
      port->handler.set_handler = &set_handlers[pp];
      return *port;
    }
    else
    {
        debug.log2("port does not exist! Halting...");
        debug.log2(p);
        while(true);
    }
  }

#if defined(ESP8266) || defined(ESP32)
  template <class NVT>
  Remote<NVT>& get_node(char* nodename)
  {
    if(remote!=NULL) {
      if(((Remote<NVT>*)remote)->address!=nodename)
      {
        delete remote; remote = NULL;
      }
    }
    if(remote==NULL)
    {
      remote = new Remote<NVT>(nodename, &_link->response);
      *((Remote<NVT>*)remote)->sender = address;
      remote_address = nodename;
      remote_handle = std::tr1::bind(&Remote<NVT>::handle, (Remote<NVT>*)remote, std::tr1::placeholders::_1, std::tr1::placeholders::_2);

      _link->response = "cp subscribe "; _link->response += nodename; _link->response += " portvalue ";  _link->response = _link->response + address;
      _link->checkSend();
    }
    return *((Remote<NVT>*)remote);
  }
 #endif

  void startTimer(int t)
  {
    if(t<no_timers)
    {
      timer_enabled[t] = true;
      timer_values[t] = millis();
    }
  }

  void stopTimer(int t)
  {
    if(t<no_timers)
      timer_enabled[t] = false;
  }

  void message_process()
  {
    if(_link->messageArrived())
    {
      if(_link->message[1]=="ack" || _link->message[1]=="not_registered") {
        announcing = false;
      }
      else if(_link->message[1]=="not_registered")
        announcing = false;
      else if(_link->message[1]=="get")
      {
        if(_link->message[2]=="name")
        {
          _link->response = _link->message[_link->message.len-1] + " ThisIs " + address;
        }
        else if(_link->message[2]=="id")
        {
          _link->response = _link->message[_link->message.len-1] + " id " + id + " " + address;
        }
        else if(_link->message[2]=="memory")
        {
          _link->response = _link->message[_link->message.len-1] + " memory " + _freeRam() + " " + address;
        }
        else if(_link->message[2]=="properties" && with_props)
        {
            int port = inputs.find("name="+_link->message[3]);
            if(port!=-1)
            {
                 _link->response = _link->message[_link->message.len-1] + " properties " +  _link->message[3] + " " + inputs[port]["prop"] + " " + address;
            }
            else
            {
                int port = outputs.find("name="+_link->message[3]);
                if(port!=-1)
                {
                     _link->response = _link->message[_link->message.len-1] + " properties " +  _link->message[3] + " " + outputs[port]["prop"] + " " + address;
                }
            }
        }
        else if(_link->message[2]=="ports")
        {
          _link->response = _link->message[_link->message.len-1];
          _link->response+=" ports ";
          for(int i=0;i<outputs.len;i++)
          {
            _link->response  += outputs[i];
            _link->response  += " ";
          }
          for(int i=0;i<inputs.len;i++)
          {
             _link->response  += inputs[i];
             _link->response  += " ";
          }

          _link->response  += address;
        }
        else
        {
          int port = outputs.find(with_props?"name="+_link->message[2]:_link->message[2]);
          if(port!=-1)
          {
             if(read_handlers[port]!=NULL)
             {
                 nString val = read_handlers[port]();
                 if(strlen(_link->response.theBuf)!=0) _link->checkSend();
                 _link->response = _link->message[_link->message.len-1] + " portvalue " + _link->message[2] + " " + val + " " + address;
             }
             else if(get_portvalue!=NULL)
             {
                 nString val = get_portvalue(_link->message[2]);
                 if(strlen(_link->response.theBuf)!=0) _link->checkSend();
                 _link->response = _link->message[_link->message.len-1] + " portvalue " + _link->message[2] + " " + val + " " + address;
             }
             /*else
             {
                 if(strlen(_link->response.theBuf)!=0) _link->checkSend();
                _link->response = _link->message[_link->message.len-1] + " portvalue " + _link->message[2] + " " + portvalues[port] + " " + address;
            }*/
          }
          else
          {
            int port = inputs.find(with_props?"name="+_link->message[2]:_link->message[2]);
            if(port!=-1)
            {
                if(strlen(_link->response.theBuf)!=0) _link->checkSend();
                _link->response = _link->message[_link->message.len-1] + " portvalue " + _link->message[2] + " " + portvalues[port] + " " + address;
            }
          }
        }
      }
      else if(_link->message[1]=="set")
      {
        if(_link->message[2]=="name")
        {
          address = _link->message[3];
          _link->response = _link->message[_link->message.len-1] + " ThisIs " + address;
          _link->resetmessage();
          saveConfig();
          readConfig();
        }
        else if(_link->message[2]=="id")
        {
          id = _link->message[3];
          _link->response = _link->message[_link->message.len-1] + " id " + id + " " + address;
          _link->resetmessage();
          saveConfig();
          readConfig();
        }
        else if(_link->message[2]=="script")
        {
            EEPROM_File file;
            if(file.filelenght("script")==-1) file.create_file("script", 2000);
            _link->message[3].removeends();
            for(int i=0;i<_link->message[3].size;i++)
               if(_link->message[3].theBuf[i]=='\'') _link->message[3].theBuf[i]='\"';
            file.save("script", _link->message[3]);
            #if defined(ESP8266) || defined(ESP32)
             ESP.restart();
            #endif
        }
        else if(_link->message[2]=="scriptlet")
        {
            if(set_portvalue!=NULL) set_portvalue(_link->message[2], _link->message[3]);
        }
        else if(_link->message[2]=="reset")
        {
            EEPROM_File file;
            file.format();
            file.create_FS(NO_EEPROM_FILES);
            #if defined(ESP8266) || defined(ESP32)
             ESP.restart();
            #endif
        }
        else if(_link->message[2]=="restart")
        {
            #if defined(ESP8266) || defined(ESP32)
             ESP.restart();
            #endif
        }
        else
        {
          int port = inputs.find(with_props?"name="+_link->message[2]:_link->message[2]);
          if(port!=-1)
          {
            portvalues[port] = (PVT) _link->message[3];
            if(set_handlers[port]!=NULL)
              set_handlers[port](_link->message[3], _link->message[_link->message.len-1]);
            if(strlen(_link->response.theBuf)!=0) _link->checkSend();
            _link->response = _link->message[_link->message.len-1] + " portvalue " + _link->message[2] + " " + portvalues[port] + " " + address;
            if(set_portvalue!=NULL) set_portvalue(_link->message[2], _link->message[3]);
          }
          else if(set_portvalue!=NULL) set_portvalue(_link->message[2], _link->message[3]);
        }
      }
      else if(_link->message[1]=="portvalue" && remote != NULL && remote_address==_link->message[_link->message.len-1])
      {
          debug.log("handing remote event");
          #if  defined(ESP8266) || defined(ESP32)
          remote_handle(_link->message[2], _link->message[3]);
          #endif
      }
      else if(_link->message[1]=="ping")
      {
        announcing = true;
        last_announced = millis() - 5000;
      }

      _link->resetmessage();
    }
  }

  void kernel_process()
  {
    if(announcing && millis()-last_announced>=5000)
    {
      if(_link->response.theBuf[0]==0)
      {
        _link->response = "cp ThisIs ";
        _link->response += id;
        _link->response += " ";
        _link->response += address;
        last_announced = millis();
      }

    }
    for(int t=0;t<no_timers;t++)
    {
      if(timer_enabled[t] && millis()-timer_values[t]>=timer_intervals[t])
      {
        timer_values[t]+=timer_intervals[t];
        timer_handlers[t]();
      }
    }

  }

  void run()
  {
      message_process();
      kernel_process();
  }
};

#endif

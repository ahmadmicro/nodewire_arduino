#ifndef NODE_H
#define NODE_H

#include <nstring2.h>
#include <nlink.h>
#include <nEEPROMFile.h>
#include <nport.h>

#ifdef ESP8266
  extern "C" {
    #include "user_interface.h"
  }
  int _freeRam()
  {
    return (int) system_get_free_heap_size();
  }
#else
  #if defined (__STM32F1__)
    int _freeRam (){ return 0;}
  #else
    #if  defined (STM32_HIGH_DENSITY)
        int _freeRam (){ return 0;}
    #else
      int _freeRam ()
      {
        extern int __heap_start, *__brkval;
        int v;
        return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
      }
    #endif
  #endif
#endif

#define MAX_TIMERS 3

typedef  void (*setHandler)( nString, nString );
typedef  void (*timerHandler)();
typedef  nString (*readHandler)();

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
  }

  void readConfig()
  {
    EEPROM_File file;
    char content[30];
    nString cont(content, sizeof(content));
    if(file.no_files()==-1) file.create_FS(4);
    if(file.open("nw.cfg", cont))
    {
        cont.split(' ');
        address = cont[0];
        id = cont[1];
    }
    else
    {
       file.create_file("nw.cfg", 30);
       cont.create_array(2);
       cont.append(address);
       cont.append("none");
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

  void on_timer(long duration, timerHandler handler)
  {
    if(no_timers<=2)
    {
        timer_handlers[no_timers] = handler;
        timer_intervals[no_timers] = duration;
        timer_enabled[no_timers] = false;
        no_timers++;
    }
  }

  void on_read(nString port, readHandler handler)
  {
    int index = outputs.find(with_props?"name="+port:port);
    if(index!=-1) read_handlers[index] = handler;
  }

  Port<PVT>& operator[](const char* p)
  {
    if(outputs.find(p)!=-1)
    {
      if(port==NULL)
      {
        port = new Port<PVT>(&address, p, &_link->response);
      }
      else
        port->portname = p;
      return *port;
    }
    //todo handle when port not found
  }

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
      remote = new Remote<NVT>(address.theBuf, &_link->response);
      *((Remote<NVT>*)remote)->sender = nodename;
    }
    return *((Remote<NVT>*)remote);
  }

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
      if(_link->message["command"]=="ack") {
        announcing = false;
      }
      else if(_link->message["command"]=="get")
      {
        if(_link->message["port"]=="name")
        {
          _link->response = _link->message["sender"] + " ThisIs " + address;
        }
        else if(_link->message["port"]=="id")
        {
          _link->response = _link->message["sender"] + " id " + id + " " + address;
        }
        else if(_link->message["port"]=="memory")
        {
          _link->response = _link->message["sender"] + " memory " + _freeRam() + " " + address;
        }
        else if(_link->message["port"]=="properties" && with_props)
        {
            int port = inputs.find("name="+_link->message["value"]);
            if(port!=-1)
            {
                 _link->response = _link->message["sender"] + " properties " +  _link->message["value"] + " " + inputs[port]["prop"] + " " + address;
            }
            else
            {
                int port = outputs.find("name="+_link->message["value"]);
                if(port!=-1)
                {
                     _link->response = _link->message["sender"] + " properties " +  _link->message["value"] + " " + outputs[port]["prop"] + " " + address;
                }
            }
        }
        else if(_link->message["port"]=="ports")
        {
          _link->response = _link->message["sender"];
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
          int port = outputs.find(with_props?"name="+_link->message["port"]:_link->message["port"]);
          if(port!=-1 && read_handlers[port]!=NULL)
          {
            nString val = read_handlers[port]();
            _link->response = _link->message["sender"] + " portvalue " + _link->message["port"] + " " + val + " " + address;
          }
          else
          {
            int port = inputs.find(with_props?"name="+_link->message["port"]:_link->message["port"]);
            if(port!=-1)
            {
              _link->response = _link->message["sender"] + " portvalue " + _link->message["port"] + " " + portvalues[port] + " " + address;
            }
          }
        }
      }
      else if(_link->message["command"]=="set")
      {
        if(_link->message["port"]=="name")
        {
          address = _link->message["value"];
          _link->response = _link->message["sender"] + " ThisIs " + address;
          _link->resetmessage();
          saveConfig();
          readConfig();
        }
        else if(_link->message["port"]=="id")
        {
          id = _link->message["value"];
          _link->response = _link->message["sender"] + " id " + id + " " + address;
          _link->resetmessage();
          saveConfig();
          readConfig();
        }
        else
        {
          int port = inputs.find(with_props?"name="+_link->message["port"]:_link->message["port"]);
          if(port!=-1)
          {
            if(set_handlers[port]!=NULL)
              set_handlers[port](_link->message["value"], _link->message["sender"]);
            portvalues[port] = (PVT) _link->message["value"];
            _link->response = _link->message["sender"] + " portvalue " + _link->message["port"] + " " + _link->message["value"] + " " + address;
          }
        }
      }
      else if(_link->message["command"]=="ping")
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
        _link->response += address;
        last_announced = millis();
      }

    }
    for(int t=0;t<no_timers;t++)
    {
      if(timer_enabled[t] && millis()-timer_values[t]>=timer_intervals[t])
      {
        timer_values[t] = millis();
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

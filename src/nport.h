#ifndef PORT_H
#define PORT_H

typedef  nString (*readHandler)();
typedef  nString (*getHandler)(nString);
typedef  void (*setHandler)( nString, nString );

union PortHandler{
  setHandler* set_handler;
  readHandler* get_handler;
};

template <class PVT>
class Port
{
   nString* nodename;
   nString* response;
   bool remote = false;
   nString cmd;
   PVT* val;
   
 public:
   const char* portname;
   union PortHandler handler;

   Port(nString* node, const char* port, nString* resp, PVT* pval)
   {
     nodename = node;
     portname = port;
     response = resp;
     if(pval!=NULL)
      val = pval;
     else
      val = new PVT();
   }

   void set_target(nString& addr)
   {
     cmd = addr; cmd+=" set ";
     remote = true;
   }

   Port<PVT>& operator=(PVT value)
   {
     *response = (remote)?cmd:"re portvalue ";
     *response +=portname;
     *response += " ";
     *response += value;
     *response += " ";
     *response += *nodename;
     *val = value;

     return *this;
   }

  void operator>>(setHandler hh){
    *handler.set_handler = hh;
  }

  void operator<<(readHandler hh){
    *handler.get_handler = hh;
  }

  operator PVT()
  {
    return (PVT)(*val);
  }
};

template <class NVT>
class Remote
{
  typedef void (*whenHandler)(NVT value);
  nString* response;
  Port<NVT>* port=NULL;
  whenHandler when_handler = NULL;


public:
  nString* sender = NULL;
  nString address;
  NVT val; //not used
  nString when_condition;

  Remote(char* nodename, nString* resp)
  {
    address = nodename;
    response = resp;
    sender = new nString();
  }

  ~Remote()
  {
      if(port!=NULL) free(port);
      free(sender);
  }

  Port<NVT>& operator[](const char* p)
  {
    if(port==NULL)
    {
      port = new Port<NVT>(sender, p, response, &val);
      port->set_target(address);
    }
    else
      port->portname = p;
    return *port;
  }

  void when(nString condition, whenHandler handler)
  {
      when_condition = condition;
      when_handler = handler;
  }

  void handle(nString port, nString value)
  {
      value.parse_as_json();
      val = (NVT)value;
      if(when_handler!=NULL && port == when_condition)
      {
          when_handler(val);
      }
  }
};


#endif

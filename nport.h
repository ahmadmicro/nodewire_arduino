#ifndef PORT_H
#define PORT_H

template <class PVT>
class Port
{
   nString* nodename;
   nString* response;
   bool remote = false;
   nString cmd;
   //PVT val;

 public:
   const char* portname;

   Port(nString* node, const char* port, nString* resp)
   {
     nodename = node;
     portname = port;
     response = resp;
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
   }

   /*
   operator PVT()
   {
     return (PVT)val;
   }*/
};

template <class NVT>
class Remote
{
  nString* response;

  Port<NVT>* port=NULL;
public:
  nString* sender = NULL;
  nString address;

  Remote(char* nodename, nString* resp)
  {
    address = nodename;
    response = resp;
    sender = new nString();
  }

  Port<NVT>& operator[](const char* p)
  {
    if(port==NULL)
    {
      port = new Port<NVT>(&address, p, response);
      port->set_target(*sender);
    }
    else
      port->portname = p;
    return *port;
  }
};


#endif

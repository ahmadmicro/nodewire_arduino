#include <nstring2.h>
#include <ESP8266WiFi.h>
#include <nEEPROMFile.h>
#include <nesp8266link.h>

#define MAX_SRV_CLIENTS 5
#define BUFF_SIZE 200

/*
  Todo
  1. websockets: https://tttapa.github.io/ESP8266/Chap14%20-%20WebSocket.html
  2. get node
*/

WiFiServer server(10001);
WiFiClient serverClients[MAX_SRV_CLIENTS];
nString addresses[MAX_SRV_CLIENTS];
//nString nodes[MAX_SRV_CLIENTS];

Esp8266Link e_link;

char in_buff[BUFF_SIZE];
char proc_buff[BUFF_SIZE];

nString buffer;
nString message;

void setup() {
  Serial.begin(38400);
  debug.setOutput(&Serial);
  debug.level = LOW_LEVEL;
  e_link.begin();

  debug.log("starting server...");

  server.begin();
  debug.log("started");

  server.setNoDelay(true);

  Serial.print("Ready! Use 'telnet ");
  Serial.print(WiFi.localIP());
  Serial.println(" 10001' to connect");

  buffer.setBuffer(in_buff, BUFF_SIZE);
  message.setBuffer(proc_buff, BUFF_SIZE);

  for(int i=0; i<MAX_SRV_CLIENTS;i++)
  {
    addresses[i]="address";
  }

  debug.log("done");
}

void process(int client_index) {
    message = buffer;
    int len = message.splitPT(' ');
    if(message[1]=="ports")
    {
        if(e_link.connect_state()==2)
        {
            e_link.response = buffer; // send to cloud
            Serial.println(buffer.theBuf);
        }
        e_link.checkSend();
        return;
    }
    if(len==5)
      message.convert_object("address command port value sender");
    else if(len==4)
      message.convert_object("address command port sender");
    else if(len==3)
      message.convert_object("address command sender");
    else return;

    message["sender"].trim();

    if(message["address"]=="cp" || message["address"]=="re")
    {
        if(client_index==-1){
          for(int i=0;i< MAX_SRV_CLIENTS; i++)
          {
              if(message["sender"]==addresses[i])
              {
                 client_index = i;
                 break;
              }
          }
          if(client_index==-1)
          {
              message.println(&Serial);
              return;
          }
        }

        if(e_link.connect_state()==2)
        {
            e_link.response = buffer; // send to cloud
            Serial.println(buffer.theBuf);
        }

        if(message["command"]=="ThisIs")
        {
            addresses[client_index] = message["sender"];
            nString addr = message["sender"];
            buffer = message["sender"] + " ack cp";
            serverClients[client_index].println(in_buff);
            //buffer = addr + " get ports cp\n";
            //serverClients[client_index].println(in_buff);
        }
        else if(message["command"]=="keepalive")
        {
            buffer = message["sender"] + " ack cp\n";
            serverClients[client_index].println(in_buff);
        }
        else if(message["command"]=="getnode")
        {

        }
        else if(message["command"]=="get" && message["port"]=="nodes")
        {

        }
    }
    else
    {
        bool found = false;
        for(int i=0;i< MAX_SRV_CLIENTS; i++)
        {
            if(message["address"]==addresses[i])
            {
                serverClients[i].println(in_buff);
                found = true; break;
            }
        }
        if(!found && e_link.connect_state()==2)
        {
            buffer += "\n";
            e_link.response = buffer; // send to cloud
        }
    }

    e_link.checkSend();
}

void resetmessage()
{
    message.collapse();
    memset(proc_buff, '\0', BUFF_SIZE);
    memset(in_buff, '\0', BUFF_SIZE);
}

void loop() {
  if(e_link.messageArrived())
  {
     //forward to addressed client
     buffer = e_link.message;
     process(-1);
     resetmessage();
     e_link.resetmessage();
  }

  uint8_t i;
  //check if there are any new clients
  if (server.hasClient()){
    for(i = 0; i < MAX_SRV_CLIENTS; i++){
      //find free/disconnected spot
      if (!serverClients[i] || !serverClients[i].connected()){
        if(serverClients[i]) serverClients[i].stop();
        serverClients[i] = server.available();
        Serial.print("New client: "); Serial.println(i);
        break;
      }
    }
    //no free/disconnected spot so reject
    WiFiClient serverClient = server.available();
    serverClient.stop();
  }

  //check clients for data
  for(i = 0; i < MAX_SRV_CLIENTS; i++){
    if (serverClients[i] && serverClients[i].connected()){
      if(serverClients[i].available()){
        //get data from the telnet client and push it to the UART
        int index = 0;
        while(serverClients[i].available()) in_buff[index++]=serverClients[i].read();
        process(i);
        resetmessage();
      }
    }
  }
}

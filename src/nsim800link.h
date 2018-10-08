#ifndef SIM800LINK_H
#define  SIM800LINK_H

#include <nlink.h>
#include <sim800.h>

#ifdef MTN
#define network "web.gprs.mtnnigeria.net";
#endif

#ifdef AIRTEL
#define network "internet.ng.airtel.com";
#endif

#ifdef GLO
#define network "gloflat";
#endif

#ifdef ETISALAT
#define network "etisalat";
#endif

#ifndef network
#define network "internet.ng.airtel.com"
#endif

class Sim800Link: public Link
{
private:
  Modem client= Modem(16);//reset pin
  long whenlastreceived;
  long last_ping;
  long last_ack;
  long connection_timeout = 20000;
  long last_attempt;
  int sendDelay;
  bool connected;
  bool auth_done = false;
  int wait;

  char _config[BUFF_SIZE]; // server instance user pwd

public:
    nString configuration;

private:
  void login()
  {
     response = "cp Gateway user=";
     response += configuration["user"]; response += " pwd=";
     response += configuration["pwd"]; response += " ";
     response += configuration["instance"];
     client.TCPsend(response.theBuf, strlen(response.theBuf));
     debug.log2(response.theBuf);
     memset(out_buff, '\0', BUFF_SIZE);
  }

  void readEM()
   {
       EEPROM_File file;
       if(file.no_files()==-1) file.create_FS(4);
       if(file.open("modem.cfg", response))
       {
           debug.log2("openning modem.cfg");
           configuration.collapse();
           configuration.create_object("server instance user pwd");
           response.parse_as_json();
           configuration["server"] = response["server"];
           configuration["instance"]=response["instance"];
           configuration["user"]=response["user"];
           configuration["pwd"]=response["pwd"];
           debug.log2("opened modem.cfg=>");
            if(debug.level==LOW_LEVEL)
              configuration.println(&Serial);
           response.collapse();
           memset(out_buff, '\0', BUFF_SIZE);
       }
       else
       {
          debug.log2("creating modem.cfg");
          file.create_file("modem.cfg", BUFF_SIZE);
          configuration.create_object("server instance user pwd");
          configuration["server"] = "dashboard.nodewire.org";
          configuration["instance"]="instance";
          configuration["user"]="user";
          configuration["pwd"]="12345678";

          configuration.dump_json(out_buff);
          debug.log2(out_buff);
          file.save("modem.cfg", response);
          memset(out_buff, '\0', BUFF_SIZE);
       }
   }

   void writeEM()
   {
     EEPROM_File file;
     configuration.dump_json(out_buff);
     debug.log2(out_buff);
     if(!file.save("modem.cfg", response))
     {
         debug.log("Re-creating File EEMPROM System...");
         file.create_FS(4);
         if(file.save("modem.cfg", response))
            debug.log("success");
     }
     memset(out_buff, '\0', BUFF_SIZE);
   }

public:
  Sim800Link()
  {
    index = 0;
    messageComplete = false;
    message.setBuffer(in_buff,BUFF_SIZE);
    response.setBuffer(out_buff, BUFF_SIZE);
    sendDelay = random(30, 100);

    configuration.setBuffer(_config, sizeof(_config));
  }

  void saveconfig()
  {
      writeEM();
  }

  int connect_state()
  {
      if(client.TCPconnected())
        return 2;
      else
        return 0;
  }

  void modemSetup(Stream* serial){
          //modemSerial.begin(9600);
          if (! client.begin(*serial)) {
            debug.log2("Couldn't find modem");
            while(1);
          }
          debug.log2("Checking for Cell network...");
          while (client.getNetworkStatus() != 1);
          debug.log2("Registered.");

          client.setGPRSNetworkSettings(F(network));

          debug.log2("APN set");
          client.enableGPRS(true);
          debug.log2("GPRS enabled");
        }

  void begin(Stream* serial)
  {
    readEM();
    modemSetup(serial);
    debug.log2("connecting...");
    while(1)
    {
        if (client.TCPconnect(configuration["server"].theBuf, 10001)) {
            debug.log2("Connected");
            break;
        }
        else {
            debug.log2("Connect fail");
        }
        debug.log2(".");
        delay(1000);
    }
  }

  void receive()
  {
    while (messageComplete==false && client.TCPavailable()) {
        char * ptr = &in_buff[index];
        uint8_t left = BUFF_SIZE - index;
        while (client.TCPavailable()) {
            uint16_t read = client.TCPread((uint8_t *)ptr, left);
            index += read;
            left -= read;
            char c = in_buff[index-1];
            debug.log2(in_buff);
            if (c == '\n' || c == '\r' || index >= BUFF_SIZE) {
                if(index !=0)
                {
                    debug.log2(in_buff);
                    int pos = message.index(configuration["instance"]);
                    if(pos==0)
                    {
                      pos+=strlen(configuration["instance"].theBuf);
                      message = message.tail(pos);
                    }
                    messageComplete = true;
                    if(connection_timeout<500000) connection_timeout+=5000;
                    index = 0;
                    last_ping = millis();
                    last_ack = last_ping;
                    return;
                }
            }
        }
     }
  }

  void checkSend() {
    if(client.TCPconnected() && strlen(out_buff)!=0)
    {
      out_buff[strlen(out_buff)] = '\n';
      client.TCPsend(out_buff, strlen(out_buff));
      debug.log2(out_buff);
      memset(out_buff, '\0', sizeof(out_buff));
    }

    if(client.TCPconnected() && (millis()  - last_ping >= connection_timeout))
    {
       last_ping = millis();
       response = "cp keepalive ";
       response+=configuration["instance"];
       response+="\n";
       client.TCPsend(out_buff, sizeof(out_buff));
       debug.log2(out_buff);
       memset(out_buff, '\0', sizeof(out_buff));
    }

    if (!client.TCPconnected() || (millis()-last_ack)> (connection_timeout+5000))
    {
      client.TCPclose();
      //Serial.println("### Client has disconnected...");

      connection_timeout = 20000;
      last_ack = millis();

      if (client.TCPconnect(configuration["server"].theBuf, 10001)) {
        Serial.println("connected");
        login();
      }
      /*else
      {
        Serial.println('restarting ...');
        ESP.reset();
      }*/
    }
  }

};

#endif

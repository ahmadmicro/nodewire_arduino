#ifndef ESP8266_H
#define  ESP8266_H

#include <nlink.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>

ESP8266WebServer http_server(80);

class Esp8266Link: public Link
{
private:
  WiFiClient client;
  long whenlastreceived;
  long last_ping;
  long last_ack;
  long connection_timeout = 20000;
  long last_attempt;
  int sendDelay;
  bool connected;
  bool auth_done = false;
  int wait;

  char _config[BUFF_SIZE]; // server instance ssid pass user pwd dev

public:
    nString configuration;

    bool wificonnected()
    {
      if(WiFi.status() != WL_CONNECTED)// && connected==false)
        return false;
      else
        return true;
    }


private:
  void login()
  {
     response = "cp Gateway user=";
     response += configuration["user"]; response += " pwd=";
     response += configuration["pwd"]; response += " ";
     response += configuration["instance"];
     client.println(response.theBuf);
     debug.log2(response.theBuf);
     memset(out_buff, '\0', BUFF_SIZE);
  }

  /* OTA */
  void startOTA()
  {
    ArduinoOTA.setHostname(configuration["dev"].theBuf);
    ArduinoOTA.setPassword((const char *)configuration["pwd"].theBuf);
    ArduinoOTA.onStart([]() {
      Serial.println("Start");
    });
    ArduinoOTA.onEnd([]() {
      Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) debug.log2("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) debug.log2("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) debug.log2("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) debug.log2("Receive Failed");
      else if (error == OTA_END_ERROR) debug.log2("End Failed");
    });
    ArduinoOTA.begin();
    debug.log2("OTA Started");
  }

  /* WIFI configuration */
 void config()
 {
   for (int i = 0; i < http_server.args(); i++) {
     if(http_server.argName(i) == "instance") http_server.arg(i).toCharArray(configuration["instance"].theBuf, configuration["instance"].size);
     else if(http_server.argName(i) == "ssid") http_server.arg(i).toCharArray(configuration["ssid"].theBuf, configuration["ssid"].size);
     else if(http_server.argName(i) == "password") http_server.arg(i).toCharArray(configuration["pass"].theBuf, configuration["pass"].size);
     else if(http_server.argName(i) == "user") http_server.arg(i).toCharArray(configuration["user"].theBuf, configuration["user"].size);
     else if(http_server.argName(i) == "pwd") http_server.arg(i).toCharArray(configuration["pwd"].theBuf, configuration["pwd"].size);
     else if(http_server.argName(i) == "devname") http_server.arg(i).toCharArray(configuration["dev"].theBuf, configuration["dev"].size);
     else if(http_server.argName(i) == "server") http_server.arg(i).toCharArray(configuration["server"].theBuf, configuration["server"].size);
   }

   writeEM();
   auth_done = false;
   wait = 0;
   http_server.send(200, "text/plain", "success");
   WiFi.softAP(configuration["dev"].theBuf, "12345678");//configuration["pwd"].theBuf);
   client.stop();
   delay(2000);
   Serial.println('restarting ...');
   ESP.reset();
 }


 void startWeb()
  {
    if (!MDNS.begin(configuration["dev"].theBuf)) {
      debug.log2("Error setting up MDNS responder!");
      debug.log2(configuration["dev"].theBuf);
    }
    else
    {
      debug.log2("mDNS responder started");
      MDNS.addService("http", "nodewire", 80);
      debug.log2(configuration["dev"].theBuf);
      connected = true;
    }

    http_server.on("/config",
        [&]() {
              config();
         }
    );

    http_server.on("/",
        []() {
           IPAddress ip = WiFi.localIP();
           String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
           String s = "<html>Hello from NodeWire gateway at <b>";
           s += ipStr;
           s += "</b></html>";

           http_server.send(200, "text/html", s);
         }
    );

    http_server.begin();
  }

  void readEM()
   {
       EEPROM_File file;
       if(file.no_files()==-1) file.create_FS(4);
       if(file.open("gw.cfg", response))
       {
           debug.log2("openning gw.cfg");
           configuration.collapse();
           configuration.create_object("server instance ssid pass user pwd dev");
           response.parse_as_json();
           configuration["server"] = response["server"];
           configuration["instance"]=response["instance"];
           configuration["ssid"] = response["ssid"];
           configuration["pass"] = response["pass"];
           configuration["user"]=response["user"];
           configuration["pwd"]=response["pwd"];
           configuration["dev"]=response["dev"];
           debug.log2("opened gw.cfg=>");
            if(debug.level==LOW_LEVEL)
              configuration.println(&Serial);
           response.collapse();
           memset(out_buff, '\0', BUFF_SIZE);
       }
       else
       {
          debug.log2("creating gw.cfg");
          file.create_file("gw.cfg", BUFF_SIZE);
          configuration.create_object("server instance ssid pass user pwd dev");
          configuration["server"] = "dashboard.nodewire.org";
          configuration["instance"]="instance";
          configuration["ssid"] = "ssid";
          configuration["pass"] = "pass";
          configuration["user"]="user";
          configuration["pwd"]="12345678";
          configuration["dev"]="mydevice";

          configuration.dump_json(out_buff);
          debug.log2(out_buff);
          file.save("gw.cfg", response);
          memset(out_buff, '\0', BUFF_SIZE);
       }
   }

   void writeEM()
   {
     EEPROM_File file;
     configuration.dump_json(out_buff);
     debug.log2(out_buff);
     if(!file.save("gw.cfg", response))
     {
         debug.log("Re-creating File EEMPROM System...");
         file.create_FS(4);
         if(file.save("gw.cfg", response))
            debug.log("success");
     }
     memset(out_buff, '\0', BUFF_SIZE);
   }

public:
  Esp8266Link()
  {
    index = 0;
    messageComplete = false;
    message.setBuffer(in_buff,BUFF_SIZE);
    sendDelay = random(30, 100);

    configuration.setBuffer(_config, sizeof(_config));
  }

  int connect_state()
  {
      if(client.connected())
        return 2;
      else if(wificonnected())
        return 1;
      else
        return 0;
  }

  void begin()
  {
    readEM();
    //WiFi.mode(WIFI_STA);
    WiFi.hostname(configuration["dev"].theBuf);
    WiFi.begin(configuration["ssid"].theBuf, configuration["pass"].theBuf);

    long wt = millis();
    long apw = wt;
    bool apmode = false;
    while (WiFi.status() != WL_CONNECTED && connected==false) {
      yield();
      if(millis()-wt>5000)
      {
        debug.log2(".");
        ESP.wdtFeed();
        wt = millis();
      }
      if(millis()-apw>30000)
      {
          if(apmode!=true)
          {
            apmode = true;
            WiFi.softAP(configuration["dev"].theBuf, "12345678");// configuration["pwd"].theBuf);
            debug.log2("access point");
            debug.log2(configuration["dev"].theBuf);
            Serial.println(WiFi.softAPIP());
            Serial.println(WiFi.status());

            startOTA();
            startWeb();
            return;
          }
          http_server.handleClient();
          ArduinoOTA.handle();
      }
    }
    debug.log2("");
    debug.log2("WiFi connected");
    debug.log2("IP address: ");
    //debug.log2(WiFi.localIP());

    startOTA();
    startWeb();

    int tries = 0;

    while(tries++<5)
    {
      debug.log2("\nconnecting to cp .");
      debug.log2(".");
      long del = millis();
      while(millis()-del<1000)
      {
        ESP.wdtFeed();
        http_server.handleClient();
        ArduinoOTA.handle();
      }
      if (client.connect(configuration["server"].theBuf, 10001)) {
        debug.log2("connected");
        login();
        break;
      }
    }
  }

  void receive()
  {
    while (messageComplete==false && client.available()) {
        char c = client.read();

        if (c == '\n' || index >= BUFF_SIZE-1) {
          if(index !=0)
          {
              int pos = message.index(configuration["instance"]);
              if(pos==0)
              {
                pos+=strlen(configuration["instance"].theBuf);
                message = message.tail(pos);
              }
              debug.log2(in_buff);
              messageComplete = true;
              if(connection_timeout<500000) connection_timeout+=5000;
              index = 0;
              last_ping = millis();
              last_ack = last_ping;
              return;
          }
        }
        else
        {
          in_buff[index] = c; index++;
        }
     }
  }

  void checkSend() {
     http_server.handleClient();
     ArduinoOTA.handle();
     if (!wificonnected() &&  millis()-last_attempt>120000) {
        last_attempt = millis();
        if(Serial.println(wifi_softap_get_station_num())==0)
        {
            Serial.print("Connecting to ");
            Serial.print(configuration["ssid"].theBuf);
            Serial.println("...");
            WiFi.begin(configuration["ssid"].theBuf, configuration["pass"].theBuf);
        }

        //if (WiFi.waitForConnectResult() != WL_CONNECTED)
        //  return;
        //Serial.println("WiFi connected");
      }

      if (wificonnected()) {
        if(client.connected() && strlen(out_buff)!=0)
        {
          client.println(out_buff);
          debug.log2(out_buff);
          memset(out_buff, '\0', sizeof(out_buff));
        }

        if(client.connected() && (millis()  - last_ping >= connection_timeout))
        {
           last_ping = millis();
           response = "cp keepalive ";
           response+=configuration["instance"];
           client.println(out_buff);
           Serial.println(out_buff);
           memset(out_buff, '\0', sizeof(out_buff));
        }

        if (!client.connected() || (millis()-last_ack)> (connection_timeout+5000))
        {
          client.stop();
          //Serial.println("### Client has disconnected...");

          connection_timeout = 20000;
          last_ack = millis();

          if (client.connect(configuration["server"].theBuf, 10001)) {
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
  }

};

#endif

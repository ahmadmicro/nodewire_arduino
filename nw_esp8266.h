#ifndef ESPCLIENT_H
#define ESPCLIENT_H

#include <nodewire.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <nstring.h>
#include <ArduinoOTA.h>
extern "C" {
#include "user_interface.h"
}

//https://github.com/adafruit/ESP8266-Arduino
//char* server = "192.168.8.102"; //"138.197.6.173";
char server[30] = "dashboard.nodewire.org";
extern char instance[];

ESP8266WebServer http_server(80);

NodeWire* __iot;

void handle_root();
void handle_config();
void handle_cmd();

char instance[20]="0p2kzwfnvwu2";
char ssid[20]="CPE_A6805B_2.4G";
char password [20] = "5026777022";
char user [30] = "sadiq.a.ahmad@gmail.com";
char pwd[20]= "secret           ";
char devname[20] ="nodewire";

class ESPClient: public NodeWire
{
   WiFiClient client;
   long last_ping;

   bool auth_done;
   bool connected;
   int wait;
   Stream* debug;

   bool programit = false;

public:
  ESPClient()
  {
    message = new Message();
    myAddress = nodeName;
    myAddress = "node01";
    cmd = _cmd;
    last_ping = millis();
    ackcount = millis();
    ack = 1;

    auth_done = false;
    connected = false;
    wait = 0;

    //EEPROM.begin(512);
  }

  void begin(char* address)
  {
     myAddress = address;
     begin();
  }

  void begin()
  {
    Serial.begin(38400);
    debug = &Serial;
    EEPROM.begin(2048);

    //writeEM();
    readEM();
    WiFi.begin(ssid, password);
    //delay(1000);

    __iot = this;

    long wt = millis();
    long apw = wt;
    bool apmode = false;
    while (WiFi.status() != WL_CONNECTED && connected==false) {
      yield();
      if(millis()-wt>5000)
      {
        Serial.print(".");
        ESP.wdtFeed();
        wt = millis();
      }
      if(millis()-apw>30000)
      {
          if(apmode!=true)
          {
            apmode = true;
            debug->println("access point");
            debug->println(devname);
            WiFi.softAP(devname, "12345678");
            debug->println(WiFi.softAPIP());
            debug->println(WiFi.status());
            startWeb();
            startOTA();
          }
          http_server.handleClient();
          ArduinoOTA.handle();
      }
      if(millis()-apw>50000) break;
    }

    if(WiFi.status() == WL_CONNECTED)
    {
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());

      readEM();

      startOTA();startWeb();

      WiFi.hostname(devname);

      int tries = 0;

      while(tries++<5)
      {
        Serial.print("\nconnecting to cp .");
        Serial.print(".");
        ESP.wdtFeed();
        delay(1000);
        if (client.connect(server, 10001)) {
          Serial.println("connected");
          login();
          break;
        }
      }
    }


  }

  void enable_sleep()
  {
    sleep_mode = true;
    WiFi.mode(WIFI_STA);
    wifi_set_sleep_type(LIGHT_SLEEP_T);
  }

  void login()
  {
     nString cmd(sendBuffer);
     cmd = "cp Gateway user=";
     cmd+= user; cmd += " pwd=";
     cmd+= pwd; cmd += " ";
     cmd += instance;
     client.println(cmd.theBuf);
     Serial.println(cmd.theBuf);
  }

  bool wificonnected()
  {
    if(WiFi.status() != WL_CONNECTED && connected==false)
      return false;
    else
      return true;
  }


   long last_tried_connection;
   void checkSend()
   {
     http_server.handleClient();
     ArduinoOTA.handle();

     if (!wificonnected() && millis()-last_tried_connection>60000) {
        last_tried_connection = millis();
        Serial.print("Connecting to ");
        Serial.print(ssid);
        Serial.println("...");
        WiFi.begin(ssid, password);

        if (WiFi.waitForConnectResult() != WL_CONNECTED)
          return;
        Serial.println("WiFi connected");
        startWeb();
      }

      if (wificonnected()) {

        if (!client.connected() && last_ping>660000) {
          Serial.println("### Client has disconnected...");
          //ESP.reset();

          if (client.connect(server, 10001)) {
            Serial.println("connected");
            login();
            ack = 0;
          }
          else
          {
            Serial.println('restarting ...');
            ESP.reset();
          }
        }
     }
   }

   bool transmit(nString sender, nString response)
   {
     nString r(sendBuffer);
     if(strlen(sender.theBuf)>1)
        r = sender;
     else
        r = remote;
     r+=" ";
     r+=response; r+=" ";
     r+= instance;
     r+=":";
     r+= myAddress;
     client.println(sendBuffer);
     Serial.println(sendBuffer);

     return true;
   }

   void announciate()
   {
     if(ack==0)
     {
       if(millis() - ackcount >= 5000)
       {
         ackcount = millis();
         nString response(sendBuffer);//todo using sendBuffer a bad idea?
         response = "cp ThisIs ";
         response+=instance;
         response+=":";
         response+=myAddress;
         client.println(sendBuffer);
         Serial.println(sendBuffer);
       }
     }
     else
     {
       if(millis()  - ackcount >= 600000)
       {
         ackcount = millis();
         nString response(sendBuffer);//todo using sendBuffer a bad idea?
         response = "cp keepalive ";
         response+=instance;
         response+=":";
         response+=myAddress;
         client.println(sendBuffer);
         Serial.println(sendBuffer);
       }
     }
   }

   void serialEvent()
   {
     while (client.available()) {
        char c = client.read();
        buffer[index] = c; index++;
        if (c == '\n' || c == '\r' || index >= sizeof(buffer)-1) {
          if(index !=0)
          {
               if(strncmp(buffer,instance, strlen(instance))==0)
               {
                   strncpy(buffer, buffer+strlen(instance)+1, strlen(buffer)-strlen(instance));
               }
               Serial.println(buffer);
               messageComplete = true;
               index = 0;
               last_ping = millis();
               return;
          }
        }
     }
   }

   /* OTA */
   void startOTA()
   {
     ArduinoOTA.setHostname(devname);
     ArduinoOTA.setPassword((const char *)"123");
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
       if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
       else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
       else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
       else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
       else if (error == OTA_END_ERROR) Serial.println("End Failed");
     });
     ArduinoOTA.begin();
     Serial.println("OTA Started");
   }

   /* WIFI configuration */
  void config()
  {
    for (int i = 0; i < http_server.args(); i++) {
      if(http_server.argName(i) == "instance") http_server.arg(i).toCharArray(instance, 20);
      else if(http_server.argName(i) == "ssid") http_server.arg(i).toCharArray(ssid, 20);
      else if(http_server.argName(i) == "password") http_server.arg(i).toCharArray(password, 20);
      else if(http_server.argName(i) == "user") http_server.arg(i).toCharArray(user, 30);
      else if(http_server.argName(i) == "pwd") http_server.arg(i).toCharArray(pwd, 20);
      else if(http_server.argName(i) == "devname") http_server.arg(i).toCharArray(devname, 20);
      else if(http_server.argName(i) == "server") http_server.arg(i).toCharArray(server, 30);
      //else if(http_server.argName(i) == "program") programit = true;
    }

    writeEM();
    auth_done = false;
    wait = 0;
    http_server.send(200, "text/plain", "success");
    client.stop();
  }

  void http_cmd()
  {
    if(http_server.argName(0) == "cmd")
    {
      http_server.arg(0).toCharArray(buffer, 150);
      messageComplete = true;
      Serial.println(buffer);
    }
  }

  void startWeb()
  {
    if (!MDNS.begin(devname)) {
      debug->println("Error setting up MDNS responder!");
      debug->println(devname);
    }
    else
    {
      debug->println("mDNS responder started");
      MDNS.addService("http", "nodewire", 80);
      debug->println(devname);
      connected = true;
    }

    http_server.on("/config", handle_config);
    http_server.on("/",  handle_root);
    http_server.on("/cmd", handle_cmd);

    http_server.begin();
  }

   void readEEPROM(char* buffer, long adderess, int size)
   {
    int i  = 0;
    while(i<size && buffer[i]!= 0)
    {
       buffer[i] = EEPROM.read(adderess+i);
       i++;
    }
    if(buffer[0]==255) buffer[0]=0;
    Serial.print(strlen(buffer));
    Serial.print(':');
    Serial.println(buffer);
   }

   void  writeEEPROM(char* content, long address, int size)
   {
      int i  = address;
      while(i<address+size && content[i-address]!=0)
      {
        EEPROM.write(i, content[i-address]);
        i++;
      }
      EEPROM.write(i,0);
      EEPROM.commit();
   }

  void readEM()
   {
       readEEPROM(instance, 100, 20);
       readEEPROM(ssid, 150, 20);
       readEEPROM(password, 200, 20);
       readEEPROM(user, 300, 30);
       readEEPROM(pwd, 400, 20);
       readEEPROM(devname, 450, 20);
       readEEPROM(server, 250, 30);

       if(strlen(devname)==0) strcpy(devname, "nodewire");
   }

   void writeEM()
   {
       writeEEPROM(instance, 100, 20);
       writeEEPROM(ssid, 150, 20);
       writeEEPROM(password, 200, 20);
       writeEEPROM(user, 300, 30);
       writeEEPROM(pwd, 400, 20);
       writeEEPROM(devname, 450, 20);
       writeEEPROM(server, 250, 30);
   }

};


void handle_root()
{
  IPAddress ip = WiFi.localIP();
  String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
  String s = "<html>Hello from NodeWire gateway at <b>";
  s += ipStr;
  s += "</b></html>";

  http_server.send(200, "text/html", s);

}

void handle_config() { //Handler
  ((ESPClient*)__iot)->config();
}

void handle_cmd() { //Handler
  ((ESPClient*)__iot)->http_cmd();
}

#endif

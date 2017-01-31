#ifndef MODEM_CLIENT_H
#define MODEM_CLIENT_H

#define BUFF_SIZE 149

#include <sim800.h>
#include <nodewire.h>
#include <nstring.h>
//#include <SoftwareSerial.h>

char server[] = "138.197.6.173";
extern char instance[];

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


class ModemClient: public NodeWire
{
    private:
        Modem mClient= Modem(4);//reset pin
        //SoftwareSerial modemSerial = SoftwareSerial(6,7); // RX, TX
    public:
        ModemClient()
        {
            message = new Message();
            myAddress = nodeName;
            myAddress = "node01";
            cmd = _cmd;
        }


        void modemSetup(){
          //modemSerial.begin(9600);
          if (! mClient.begin(Serial)) {
            Serial.println(F("Couldn't find modem"));
            while(1);
          }
          Serial.print(F("Checking for Cell network..."));
          while (mClient.getNetworkStatus() != 1);
          Serial.println(F("Registered."));

          mClient.setGPRSNetworkSettings(F(network));

          Serial.println("APN set");
          mClient.enableGPRS(true);
          Serial.println("GPRS enabled");
        }

        void begin(char* address)
        {
            myAddress = address;
            begin();
        }
        void begin()
        {
            Serial.begin(38400);
            Serial.print("Connecting...");

            modemSetup();

            while(1)
            {
                if (mClient.TCPconnect(server, 10001)) {
                    Serial.println("Connected");
                    break;
                }
                else {
                    Serial.println("Connect fail");
                }
                Serial.print(".");
                delay(1000);
            }

            wdt_enable(WDTO_8S);
        }

        void checkSend()
        {

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
            if (mClient.TCPconnected() && mClient.TCPsend(sendBuffer, strlen(sendBuffer))){
                //Serial.println(sendBuffer);
                return true;
            }
            //Serial.println(F("Transmit fail"));
            return false;
        }

        void announciate()
        {
            wdt_reset();
            if (ack == 0)
            {
                if (millis() - ackcount >= 5000 && strlen(sendBuffer) == 0)
                {
                    ackcount = millis();
                    nString response(sendBuffer);
                    response = "cp ThisIs ";
                    response+=instance;
                    response+=":";
                    response+=myAddress;
                    if (mClient.TCPconnected())
                        mClient.TCPsend(sendBuffer, strlen(sendBuffer));
                }
            }
        }

        void serialEvent()
        {
            //Serial.println("Entering serial event");
            char * ptr = &buffer[index];
            uint8_t left = BUFF_SIZE - index;
            while (mClient.TCPavailable()) {
                uint16_t read = mClient.TCPread((uint8_t *)ptr, left);
                index += read;
                left -= read;
                char c = buffer[index-1];
                if (c == '\n' || c == '\r' || index >= BUFF_SIZE) {
                    if(index !=0)
                    {
                        if(strncmp(buffer, instance, strlen(instance)) == 0)
                        {
                            strncpy(buffer, buffer+strlen(instance)+1, strlen(buffer)-strlen(instance));
                        }
                        Serial.println(buffer);
                        messageComplete = true;
                        index = 0;
                        //Serial.println("Leaving serial event");
                        return;
                    }
                }
            }
        }
};

#endif

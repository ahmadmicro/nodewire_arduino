#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
class zigbee
{
    Stream* _serial;
public:
      zigbee(Stream* serial)
      {
          _serial = serial;
      }
     /* void findBaud()
      {
          long bauds [] = { 9600, 19200, 38400, 57600, 115200};
          long baud = 0;
          for(int i=0; i<4; i++)
          {
            _serial->flush ();   // wait for send buffer to empty
            delay (100);    // let last character be sent
            _serial->end ();      // close serial
            _serial->begin(bauds[i]);
            _serial->println(bauds[i]);
            if (testBaud())
            {
              baud = bauds[i];
              break;
            }
          }
          Serial.print("The baud rate is:");
          Serial.println(baud);
      }*/

      void send(uint8_t* cmd, int len)
      {
        cmd[len-1]= 0;
        for(int i =0; i<(len-1); i++) cmd[len-1]+=cmd[i];
        _serial->write(cmd, len);
      }

      void receive(uint8_t* res, int len)
      {
        while(_serial->available()==0);
        _serial->readBytes(res, len);
      }

      void setBaud(uint8_t baud)
      {
          //baud: 01,02,03,04,05 for 9600, 19200, 38400, 57600, 115200 respectively
          uint8_t cmd[] = {0xFC, 0x01, 0x91, 0x06, 0x00, 0xF6, 0x00};
          uint8_t response[6];
          cmd[4] = baud;

          send(cmd, sizeof(cmd));
          receive(response, 6);
      }

      bool testBaud()
      {
        uint8_t cmd[] = {0xFC, 0x00, 0x91, 0x07, 0x97, 0xA7, 0x00};
        uint8_t response[8];

        send(cmd, sizeof(cmd));
        receive(response, sizeof(response));
        Serial.print(response[0]);Serial.print(response[1]);Serial.print(response[2]);
        if(response[0] == 1 && response[1] == 2 && response[2]==3) return true; else return false;
      }

      void setChannel(uint8_t channel)
      {
        /// channel:
        // XX = 0B:Channel 11，2405MHz
        // XX = 0C:Channel 12，2410MHz
        // XX = 0D:Channel 13，2415MHz
        // XX = 0E:Channel 14，2420MHz
        // XX = 0F:Channel 15，2425MHz
        // XX = 10:Channel 16，2430MHz
        // XX = 11:Channel 17，2435MHz
        // XX = 12:Channel 18，2440MHz
        // XX = 13:Channel 19，2445MHz
        // XX = 14:Channel 20，2450MHz
        // XX = 15:Channel 21，2455MHz
        // XX = 16:Channel 22，2460MHz
        // XX = 17:Channel 23，2465MHz
        // XX = 18:Channel 24，2470MHz
        // XX = 19:Channel 25，2475MHz
        // XX = 1A:Channel 26，2480MHz
        uint8_t cmd[] = {0xFC, 0x01, 0x91, 0x0C, 0x00, 0x1A, 0x00};
        uint8_t response[5];
        cmd[4] = channel;

        send(cmd, sizeof(cmd));
        receive(response, 5);
      }

      void setCoordinator()
      {
        uint8_t cmd[] = {0xFC, 0x00, 0x91, 0x09, 0xA9, 0xC9, 0x00};
        uint8_t response[8];
        send(cmd, sizeof(cmd));
        receive(response, 8);
      }

      void setRouter()
      {
        uint8_t cmd[] = {0xFC, 0x00, 0x91, 0x0A, 0xBA, 0xDA, 0x00};
        uint8_t response[8];
        send(cmd, sizeof(cmd));
        receive(response, 8);
      }

      void getPanID()
      {
        uint8_t cmd[] = {0xFC, 0x00, 0x91, 0x03, 0xA3, 0xB3, 0x00};
        uint8_t response[2];
        send(cmd, sizeof(cmd));
        receive(response, 2);
        Serial.print("pan id:");
        Serial.print(response[0],HEX);
        Serial.println(response[1],HEX);
      }

      void setPanID(uint8_t* panid)
      {
        uint8_t cmd[] = {0xFC, 0x02, 0x91, 0x01, 0x00, 0x00, 0x00};
        uint8_t response[2];
        cmd[4] = panid[0];
        cmd[5] = panid[1];

        send(cmd, sizeof(cmd));
        receive(response, 2);

        Serial.print("pan id:");
        Serial.print(response[0],HEX);
        Serial.println(response[1],HEX);
      }

      void configDefaultRouter()
      {
        setRouter();
        setChannel(0x1A);

      }

      void configDefaultCoordinator(uint8_t* panid)
      {
        setCoordinator();
        setChannel(0x1A);
        setPanID(panid);
      }
};

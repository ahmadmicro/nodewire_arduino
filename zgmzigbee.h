#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
class zigbee
{
public:
      zigbee()
      {

      }


      void send(uint8_t* cmd, int len)
      {
        cmd[len-1]= 0;
        for(int i =0; i<(len-1); i++) cmd[len-1]^=cmd[i];
        Serial.write(cmd, len);
      }

      void receive(uint8_t* res, int len)
      {
        while(Serial.available()==0);
        Serial.readBytes(res, len);
      }

      bool reset()
      {
          //baud: 01,02,03,04,05 for 9600, 19200, 38400, 57600, 115200 respectively
          uint8_t cmd[] = {0xFC, 0x06, 0x01, 0x00, 0x00, 0x00, 0x00};
          uint8_t response[7];

          send(cmd, sizeof(cmd));
          receive(response, 7);
      }

      void setBaud(uint8_t baud)
      {
          //baud: 00,01,02,03,04 for 9600, 19200, 38400, 57600, 115200 respectively
          uint8_t cmd[] = {0xFC, 0x06, 0x13, 0x00, 0x00, 0x00, 0x00};
          uint8_t response[7];
          cmd[4] = baud;

          send(cmd, sizeof(cmd));
          receive(response, 7);
      }

      void setChannel(uint8_t channel /* channel: 11-26*/)
      {
        // Channel 11，2405MHz
        // Channel 12，2410MHz
        // Channel 13，2415MHz
        // Channel 14，2420MHz
        // Channel 15，2425MHz
        // Channel 16，2430MHz
        // Channel 17，2435MHz
        // Channel 18，2440MHz
        // Channel 19，2445MHz
        // Channel 20，2450MHz
        // Channel 21，2455MHz
        // Channel 22，2460MHz
        // Channel 23，2465MHz
        // Channel 24，2470MHz
        // Channel 25，2475MHz
        // Channel 26，2480MHz
        uint8_t cmd[] = {0xFC, 0x06, 0x09, 0x00, 0x00, 0x00, 0x00};
        uint8_t response[7];
        cmd[4] = channel;

        send(cmd, sizeof(cmd));
        receive(response, 7);
      }

      void setCoordinator()
      {
        uint8_t cmd[] = {0xFC, 0x06, 0x11, 0x00, 0x00, 0x00, 0x00};
        uint8_t response[7];
        send(cmd, sizeof(cmd));
        receive(response, 7);
      }

      void setRouter()
      {
        uint8_t cmd[] = {0xFC, 0x06, 0x11, 0x00, 0x01, 0x00, 0x00};
        uint8_t response[7];
        send(cmd, sizeof(cmd));
        receive(response, 7);
      }

      void getPanID()
      {
        uint8_t cmd[] = {0xFC, 0x03, 0x02, 0x00, 0x00, 0x00, 0x00};
        uint8_t response[7];
        send(cmd, sizeof(cmd));
        receive(response, 7);
        Serial.print("pan id:");
        Serial.print(response[5],HEX);
        Serial.println(response[4],HEX);
      }

      void setDataMode(uint8_t mode)
      {
        //mode:
        uint8_t cmd[] = {0xFC, 0x06, 0x12, 0x00, 0x00, 0x00, 0x00};
        cmd[4]  = mode;
        uint8_t response[7];
        send(cmd, sizeof(cmd));
        receive(response, 7);
      }

      void setPanID(uint8_t* panid)
      {
        uint8_t cmd[] = {0xFC, 0x06, 0x02, 0x00, 0x00, 0x00, 0x00};
        uint8_t response[7];
        cmd[5] = panid[0];
        cmd[4] = panid[1];

        send(cmd, sizeof(cmd));
        receive(response, 7);

        if(response[1]==0x06)
        {
          Serial.print("pan id:");
          Serial.print(response[5],HEX);
          Serial.println(response[4],HEX);
        }
        else
          Serial.println("failed to set panid");
      }

      void configDefaultRouter()
      {
        setRouter();
        setChannel(0x26);

      }

      void configDefaultCoordinator(uint8_t* panid)
      {
        setCoordinator();
        setChannel(0x26);
        setPanID(panid);
      }
};

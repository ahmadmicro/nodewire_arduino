#include <nstring2.h>
#include <nEEPROMFile.h>
#include <nseriallink.h>
#include <nesp8266link.h>

Esp8266Link e_link;
SerialLink s_link;

void setup() {
  Serial.begin(38400);

  s_link.begin(&Serial);
  e_link.begin();
}

void loop() {
  if(s_link.messageArrived())
  {
      e_link.response = s_link.message;
      s_link.resetmessage();
  }
  if(e_link.messageArrived())
  {
      s_link.response = e_link.message;
      e_link.resetmessage();
  }
}

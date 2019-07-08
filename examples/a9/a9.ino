#include <na9.h>
#include <SoftwareSerial.h>

A9Modem modem;
SoftwareSerial m(2,3);

void setup() {
  Serial.begin(38400);
  m.begin(38400);
  modem.begin(&m);

  delay(10000);
  modem.send("08012345678", "this is my message");
  delay(10000);
  modem.call("08012345678");

  modem.on_incoming([](const char* number){
    Serial.print("incoming call:");
    Serial.println(number);
    modem.answer();
  });

  modem.on_message([](const char* number, const char* message){
    Serial.print("received sms:");
    Serial.println(message);
    Serial.print("sent by:");
    Serial.println(number);
  });
}

void loop() {
  modem.run();
}
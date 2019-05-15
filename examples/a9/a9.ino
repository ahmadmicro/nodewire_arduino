#include <na9.h>

A9Modem modem;

void setup() {
  Serial.begin(38400);
  modem.begin(&Serial);

  modem.call("08012345678");
  modem.send("08012345678", "this is my message");

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
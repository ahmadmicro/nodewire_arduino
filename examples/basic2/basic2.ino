#include <nnode.h>
#include <nsim800link.h>
#include <SoftwareSerial.h>

#define LED LED_BUILTIN

Node<int> node;
Sim800Link lnk;
//SoftwareSerial modemSerial = SoftwareSerial(14,12); // RX, TX
SoftwareSerial modemSerial = SoftwareSerial(12,14); // RX, TX

void setup() {
  Serial.begin(38400);
  modemSerial.begin(9600);
  debug.setOutput(&Serial);
  debug.level = LOW_LEVEL;
  
  lnk.begin(&modemSerial);

  node.inputs = "led";
  node.init("node##", &lnk);

  node["led"] >> digitalPin(LED);

  pinMode(LED, OUTPUT);
}

void loop() {
  node.run();
}

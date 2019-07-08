#include <nnode.h>
#include <nesp8266link.h>

#define LED LED_BUILTIN

Node<int> node;
Esp8266Link lnk;

void setup() {
  Serial.begin(38400);
  debug.setOutput(&Serial);
  debug.level = LOW_LEVEL;
  lnk.begin();

  node.inputs = "led";
  node.init("node##", &lnk);

  node["led"] >> digitalPin(LED);

  pinMode(LED, OUTPUT);
}

void loop() {
  node.run();
}

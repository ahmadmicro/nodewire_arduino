#include <nnode.h>
#include <nesp8266link.h>

#define LED LED_BUILTIN

Node<int> node;
Esp8266Link lnk;

void setup() {
  lnk.begin();

  node.inputs = "led";
  node.init("node##", &lnk);

  node["led"] >> digitalPin(LED);

  pinMode(LED, OUTPUT);
}

void loop() {
  node.run();
}
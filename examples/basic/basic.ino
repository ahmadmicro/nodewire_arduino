#include <lnode.h>
#include <nseriallink.h>

#define LED LED_BUILTIN

Node<int> node;
SerialLink lnk;

void setup() {
  Serial.begin(38400);
  lnk.begin(&Serial);

  node.inputs = "led";
  node.init("node##", &lnk);

  node["led"] >> digitalPin(LED);

  pinMode(LED, OUTPUT);
}

void loop() {
  node.run();
}

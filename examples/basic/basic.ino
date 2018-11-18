#include <lnode.h>
#include <nseriallink.h>

#define LED LED_BUILTIN

Node<int> node;
SerialLink link;

void setup() {
  Serial.begin(38400);
  link.begin(&Serial);

  node.inputs = "led";
  node.init("node##", &link);

  node["led"] >> digitalPin(LED);

  pinMode(LED, OUTPUT);
}

void loop() {
  node.run();
}

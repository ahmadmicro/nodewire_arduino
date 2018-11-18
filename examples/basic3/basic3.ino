#include <nnode.h>
#include <nesp32link.h>

#define LED LED_BUILTIN

Node<int> node;
Esp32Link lnk;

void setup() {
  lnk.begin();

  node.inputs = "led";
  node.init("node##", &link);

  node["led"] >> digitalPin(LED);

  pinMode(LED, OUTPUT);
}

void loop() {
  node.run();
}

#include <nnode.h>
#include <nesp32link.h>

#define LED LED_BUILTIN

Node<int> node;
Esp32Link lnk;

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

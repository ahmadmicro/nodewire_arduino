#include <lnode.h>
#include <nseriallink.h>

#define LED LED_BUILTIN

Node<int> node;
SerialLink link;
int pins[] = {11,12,13};

void setup() {
  Serial.begin(38400);
  link.begin(&Serial);

  node.inputs = "11 12 13";
  node.init("smartkon_##", &link);

  node >> digitalPins(node.inputs, pins);

  for(int i=0; i<node.inputs.len;i++)
     pinMode(pins[i], OUTPUT);
}

void loop() {
  node.run();
}
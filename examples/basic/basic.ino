#include <lnode.h>
#include <nseriallink.h>

#define LED LED_BUILTIN

Node<int> node;
SerialLink link;

void setup() {
  Serial.begin(38400);
  link.begin(&Serial);

  node.inputs = "led";
  node.init("node01");
  node.setLink(&link);

  node.on("led",
     [](nString val, nString sender) {
        digitalWrite(LED,(int)val);
     }
  );

  pinMode(LED, OUTPUT);
}

void loop() {
  node.run();
}

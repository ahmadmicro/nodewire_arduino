#include <nnode.h>
#include <nesp8266link.h>

#define LED LED_BUILTIN

Node<int> node;
Esp8266Link link;

void setup() {
  Serial.begin(38400);
  link.begin();

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

#include <nnode.h>
#include <nseriallink.h>

#define LED LED_BUILTIN

Node<int> node;
SerialLink link;

void setup() {
  Serial.begin(38400);
  link.setSerial(&Serial);

  node.inputs = "led:Indicator";
  node.init_with_props("node01");
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

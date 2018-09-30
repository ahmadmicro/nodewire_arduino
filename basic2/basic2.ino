#include <nnode.h>
#include <nesp8266link.h>

#define LED LED_BUILTIN

Node<int> node;
Esp8266Link lnk;

void setup() {
  Serial.begin(38400);
  lnk.begin();

  node.inputs = "led";
  node.init("node01");
  node.setLink(&lnk);

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

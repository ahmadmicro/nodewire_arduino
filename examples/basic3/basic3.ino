#include <nnode.h>
#include <nesp32link.h>

#define LED LED_BUILTIN

Node<int> node;
Esp32Link lnk;

int count;

void setup() {
  Serial.begin(38400);
  debug.setOutput(&Serial);
  debug.level = LOW_LEVEL;
  
  lnk.begin();

  node.inputs = "led reset";
  node.outputs = "count";
  node.init("node##", &lnk);

  node["led"] >> digitalPin(LED);
  
  node["reset"] >> [](nString val, nString sender) {
    count = (int)val;
  };

  node["count"] << []()->nString {
    return count;
  };

  node.timer(1000, []() {
    node["count"] = count++;
  });

  node.startTimer(0);

  pinMode(LED, OUTPUT);
}

void loop() {
  node.run();
}

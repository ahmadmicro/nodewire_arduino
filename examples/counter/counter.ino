#include <nnode.h>
#include <nesp8266link.h>

Node<int> node;
Esp8266Link link;
int count = 0;

void setup() {
  Serial.begin(38400);
  debug.setOutput(&Serial);
  debug.level = LOW_LEVEL;

  link.begin();

  node.outputs = "count";
  node.init("node01");
  node.setLink(&link);

  node["count"] << []()->nString {
    return count;
  };

  node.timer(1000, []() {
    node["count"] = count++;
  });

  node.startTimer(0);
}

void loop() {
  node.run();
}

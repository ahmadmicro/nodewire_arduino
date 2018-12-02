#include <nnode.h>
#include <nseriallink.h>

#define LED LED_BUILTIN

Node<nString> node;
SerialLink lnk;

bool ledon = false;

void setup() {
  Serial.begin(38400);
  lnk.begin(&Serial);

  node.inputs = "led";
  node.init("node##", &lnk);

  node["led"] >> [](nString val, nString sender) {
    node.stopTimer(0);
    if(val=="on")
        digitalWrite(LED,1);
    else if(val=="off")
        digitalWrite(LED,0);
    else if(val=="blink")
        node.startTimer(0);
  };

  node.timer(500, []() {
    if(ledon) digitalWrite(LED, 1); else digitalWrite(LED, 0);
        ledon = !ledon;
  });

  pinMode(LED, OUTPUT);
}

void loop() {
  node.run();
}

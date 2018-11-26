#include <nnode.h>
#include <nesp32link.h>

#define LED LED_BUILTIN
#define BUTTON A0

Node<int> node;
Esp32Link lnk;

bool ledon = false;

void setup() {
  lnk.begin();

  node.inputs = "led run";
  node.outputs = "button";
  node.init("node-##", &lnk);

  node["led"] >> digitalPin(LED);
  node["button"] << outputValue(digitalRead(BUTTON));

  node["run"] >> [](nString val, nString sender) {
     if((int)val == 1)
       node.startTimer(0);
     else
       node.stopTimer(0);
  };

  node.timer(500, []() {
    if(ledon) digitalWrite(LED, 1); else digitalWrite(LED, 0);
    ledon = !ledon;
  });

  node.timer(500, []() { 
    if(node["button"] != digitalRead(BUTTON))
       node["button"] = digitalRead(BUTTON); 
  });

  pinMode(LED, OUTPUT);
  node.startTimer(1);
}

void loop() {
  node.run();
}
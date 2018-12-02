#include <nnode.h>
#include <nseriallink.h>

Node<int> node;
SerialLink lnk;
int count = 0;

void setup() {
  Serial.begin(38400);
  debug.setOutput(&Serial);
  debug.level = LOW_LEVEL;

  lnk.begin(&Serial);

  node.outputs = "count";
  node.init("node_##", &lnk);

  node["count"] << []()->nString {
    return count;
  };

  node.timer(1000, []() {
    node["count"] = count++;
    if(count==10)
    {
      auto& sco = node.get_node<int>("sco");
      sco["buzzer"]=1;
      sco.when("mains",
          [](int value){
              debug.log("mains event");
              if(value==1) debug.log("mains restored");
          }
      );
    }
    if(count==12)
    {
      node.get_node<int>("sco")["buzzer"]=0;
    }
  });

  node.startTimer(0);
}

void loop() {
  node.run();
}

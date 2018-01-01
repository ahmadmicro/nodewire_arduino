#include <nnode.h>
#include <nseriallink.h>

Node<nString> node;
SerialLink link;
int count = 0;

void setup() {
  Serial.begin(38400);
  debug.setOutput(&Serial);
  debug.level = LOW_LEVEL;

  link.begin(&Serial);

  node.outputs = "count";
  node.init("node01");
  node.setLink(&link);

  node.on_read("count",
    []()->nString {
      return count;
    }
  );

  node.on_timer(1000,
     []() {
         node["count"] = count++;
         if(count==10)
         {
            auto sco = node.get_node<int>("sco");
            sco["buzzer"]=1;
         }
         if(count==12)
         {
            auto sco = node.get_node<int>("sco");
            sco["buzzer"]=0;
         }
     }
  );

  node.startTimer(0);
}

void loop() {
  node.run();
}

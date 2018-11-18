#include <lnode.h>
#include <nseriallink.h>

#define LED LED_BUILTIN

Node<int> node;
SerialLink link;

void setup() {
  Serial.begin(38400);
  link.begin(&Serial);

  node.inputs = "11 12 13";
  node.outputs = "2 3";

  node.init("node01");
  node.setLink(&link);

  node >> [](nString port, nString val) {
    if(node.inputs.find(port)!=-1)
        digitalWrite((int)port,(int)val);
  };

  node << [](nString port) -> nString {
    if(node.outputs.find(port)!=-1)
    {
      if(((int)port)>=14)
          return analogRead((int)port);
      return digitalRead((int)port);
    }
    else
      return "error";
  };

  for(int i=0; i<node.inputs.len;i++)
    pinMode((int)node.inputs[i], OUTPUT);
}

void loop() {
  node.run();
}

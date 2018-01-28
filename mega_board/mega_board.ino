#include <nnode.h>
#include <nseriallink.h>

Node<nString> node;
SerialLink link;

nString pins;
nString vals;

char buffer[150];

void setup() {
  Serial.begin(38400);
  link.begin(&Serial);

  node.inputs = "pins";
  node.init("node01");
  node.setLink(&link);

  pins = "3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19";
  pins.split(' ');
  pins.convert_object("D3 D4 D5 D6 D7 D8 D9 D10 D11 D12 D13 A0 A1 A2 A3 A4 A5");
  vals.create_object("D3 D4 D5 D6 D7 D8 D9 D10 D11 D12 D13 A0 A1 A2 A3 A4 A5");

  node.on("pins",
     [](nString val, nString sender) {  // node01 set pins {"D13":1} ah
        val.parse_as_json();
        for(int i=0;i<val.len;i++)
        {
          digitalWrite((int)pins[(*val.keys)[i]], (double)val[i]);
          vals[(*val.keys)[i]] = val[i];
        }
        vals.dump_json(buffer);
        node["pins"] = buffer;
     }
  );

  for(int i=0;i<pins.len;i++)
    pinMode((int)pins[i], OUTPUT);
}

void loop() {
  node.run();
}

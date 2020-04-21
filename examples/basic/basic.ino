#include <nnode.h>
#include <nesp32link.h>
#include <data/ndb.h>

#define LED LED_BUILTIN

Node<nString> node;
DB db(&node);
Esp32Link lnk;

nString vv;

void setup() {
  Serial.begin(38400);
  debug.setOutput(&Serial);
  debug.level = LOW_LEVEL;
  lnk.begin();

  vv = "hello";

  node.inputs = "store query";
  node.outputs = "fetch";
  node.init("node##", &lnk);

  node["store"] >> [](nString val, nString sender) {
    db.set("people", val);
    val.parse_as_json();
    vv = val;
  };

  node["query"] >> [](nString val, nString sender) {
    db.get("people", val, [](nString value){
        node["fetch"] = value;
        vv = value;
        vv.parse_as_json();
    });
  };

  node["fetch"] << []() -> nString {
    nString val = vv;
    val.toString();
    return val;
  };
}

void loop() {
  node.run();
}

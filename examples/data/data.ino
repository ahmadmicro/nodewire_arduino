#include <nnode.h>
#include <nesp32link.h>
#include <data/ndb.h>

#define LED LED_BUILTIN

Node<nString> node;
DB db(&node);
Esp32Link lnk;

nString vv;

void setup()
{
    Serial.begin(38400);
    debug.setOutput(&Serial);
    debug.level = LOW_LEVEL;

    lnk.begin();

    vv = "{name:'someone', age:10}";

    node.inputs = "store query";
    node.outputs = "fetch";
    node.init("node##", &lnk);

    node["store"] >> [](nString val, nString sender) {
        db.set("people", val);
        vv = val;
    };

    node["query"] >> [](nString val, nString sender) {
        db.get("people", val, [](nString value){
            vv = value;
            node["fetch"] = value;
        });
    };

    node["fetch"] << []() -> nString {
        return vv;
    };
}

void loop()
{
    node.run();
}
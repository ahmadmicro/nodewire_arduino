#include <nnode.h>
#include <nesp32link.h>
#include <TimeLib.h>

#define LED LED_BUILTIN

Node<int> node;
Esp32Link lnk;

nString entries;
char ent_buffer[3000];

void setup() {
  Serial.begin(38400);
  lnk.begin();

  entries.setBuffer(ent_buffer, sizeof(ent_buffer));
  entries.create_array(96);

  char buffer[100];
  
  nString entry(buffer, sizeof(buffer));
  entry.create_object("p_pwr energy timestamp");
  entry["p_pwr"] = "peak_power";
  entry["energy"] = 123;//node["energy"];
  entry["timestamp"] = now();

  entry.println(&Serial);

  entries.append(entry);

  for(int i = 0; i<entries.len; i++)
  {
    entries[i].println(&Serial);
  }

  nString thedata;
  thedata.createBuffer(3000);
  thedata.create_array(3);
  thedata.append(entries[0]["timestamp"]);
  thedata.append(node.address);
  thedata.append("");
  thedata[2].create_array(96);
  thedata.convert_object("start_time meter_no values");
  for(int i = 0; i<entries.len; i++)
  {
    nString data_element;
    data_element.createBuffer(30);
    data_element.create_array(3);
    data_element.append(entries[i]["energy"]);
    data_element.append(entries[i]["energy"]);
    data_element.append(entries[i]["power"]);
    data_element.convert_object("active_energy reactive_energy peak_active_power");
    thedata[2].append(data_element);
  }
  thedata.println(&Serial);

  node.inputs = "led";
  node.init("node##", &lnk);

  node["led"] >> digitalPin(LED);

  pinMode(LED, OUTPUT);
}

void loop() {
  node.run();
}

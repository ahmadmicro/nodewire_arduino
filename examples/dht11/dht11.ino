#include <nnode.h>
#include <nesp8266link.h>
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11

/*
 *  https://github.com/adafruit/DHT-sensor-library
 */

Node<double> node;
Esp8266Link lnk;
DHT dht(DHTPIN, DHTTYPE);

double h, t;

void setup() {
  lnk.begin();

  node.outputs = "temperature humidity";
  node.init("dht11", &lnk);
  
  dht.begin();
  node["temperature"] << outputValue(dht.readTemperature());
  node["humidity"] << outputValue(dht.readHumidity());

  node.timer(500, []() {
    if(abs(dht.readTemperature()-t)>0.5)
    {
      node["temperature"] = t = dht.readTemperature();
    }

    if(abs(dht.readHumidity()-h)>0.5)
    {
      node["humidity"] = h = dht.readHumidity();
    }
  });

  node.startTimer(0);
}

void loop() {
  node.run();
}
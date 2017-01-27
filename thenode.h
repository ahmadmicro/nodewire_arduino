Node* __thenode;

void wait(long ms)
{
   long now = millis();
   while((millis()-now)<ms)
       loop();
}

void loop() {
  __thenode->loop();
  __thenode->checkTimer();
  if (__thenode->iot.messageArrived())
  {
        __thenode->handleMessage();
        __thenode->iot.resetmessage();
  }
}

void setNode(Node* node)
{
  __thenode = node;
  __thenode->init();
}

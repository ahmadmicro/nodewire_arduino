/*
Copyright (c) 2016, NodeWire.org
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software
   must display the following acknowledgement:
   This product includes software developed by NodeWire.org.
4. Neither the name of NodeWire.org nor the
   names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY NodeWire.org.ng ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL NodeWire.org.ng BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "nodewire.h"

void NodeWire::begin(char* address)
{
   this->myAddress = address;
   begin();
}

void NodeWire::begin()
{
   Serial.begin(9600);
}

bool NodeWire::messageArrived()
{
  serialEvent();
  announciate();
  if(messageComplete)
  {
    SplitCommand();
    if(myAddress==message->Address || mybroadcastaddress==message->Address)
    {
        if(cmd == "ping")
        {
          ack = 0;
        }
        else if(cmd == "ack")
        {
            ack = 1;
        }
        else
          return true;
    }
    resetmessage();
  }
  return false;
}
void NodeWire::announciate()
{
  if(ack==0)
  {
    if(millis() - ackcount >= 5000)
    {
      ackcount = millis();
      String response = ((iswifi)?String("cp RoutingVia node/" + myAddress + " "):String("cp ThisIs ")) + myAddress;
      Serial.println(response);
    }
  }
}

void NodeWire::serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    buffer[index] = inChar; index++;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n' || inChar == '\r' || index >= sizeof(buffer)-1) {
      messageComplete = true;
      index = 0;
      return;
    }
  }
}

void NodeWire::transmit(String response) {
   Serial.println(((message->Sender!=NULL)?String(message->Sender):String("remote")) + " " + response + " " + myAddress);
}

void NodeWire::resetmessage(){
    messageComplete = false;
    memset(buffer, '\0', sizeof(buffer));
    message->Address = message->Command = message->Sender = NULL;
    for(int i = 0; i<=31; i++)
        message->Params[i] = NULL;
}

void NodeWire::SplitCommand()
{
  int i;
  message->Address = message->Command = message->Sender = NULL;
  for(i = 0; i<=31; i++)
      message->Params[i] = NULL;

  message->Address = strtok (buffer," ,");
  if(message->Address == NULL ) return;
  message->Command = strtok (NULL, " ,");
  if(message->Command == NULL ) return;
  cmd = String(message->Command);
  i = 0;
  do{
    message->Params[i] = strtok (NULL, " ,");
  } while(message->Params[i++] != NULL && i<31);

  if(message->Params[i-1] == NULL )
    message->Sender = message->Params[i-2];
   else
    message->Sender = strtok (NULL, " ,");
   message->Sender[strlen(message->Sender)-1] = 0;
}
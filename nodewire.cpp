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
   myAddress = address;
   begin();
}

void NodeWire::begin()
{
   Serial.begin(38400);
   wdt_enable(WDTO_8S);
}

bool NodeWire::messageArrived()
{
  serialEvent();
  announciate();
  checkSend();
  if(messageComplete)
  {
    SplitCommand();
    if((myAddress==message->Address || mybroadcastaddress==message->Address) && myAddress != message->Sender)
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
  wdt_reset();
  if(ack==0)
  {
    if(millis() - ackcount >= 5000 && strlen(sendBuffer)==0)
    {
      ackcount = millis();
      nString response(sendBuffer);//todo using sendBuffer a bad idea?
      response = "cp ThisIs ";
      response+=myAddress;
      //Serial.println(sendBuffer);
    }
  }
}

void NodeWire::serialEvent() {
  while (Serial.available()) {
    whenlastreceived = millis();
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

bool NodeWire::transmit(nString sender, nString response) {
   if(strlen(sendBuffer)==0)
   {
     nString r(sendBuffer);
     if(strlen(sender.theBuf)>1)
        r = sender;
     else
        r = remote;
     r+=" ";
     r+=response; r+=" "; r+= myAddress;
      return true;
   }
   return false;
}

void NodeWire::checkSend(){
  if(strlen(sendBuffer)!=0 && Serial.available()==0 && (millis()-whenlastreceived)>sendDelay)
  {
    Serial.println(sendBuffer);
    serialEvent();//not tested.  receive a copy of the message sent
    if(strncmp(sendBuffer,buffer, strlen(buffer))!=0)
    {
      whenlastreceived = millis();
      return;//not tested. detect collision
    }
    memset(sendBuffer, '\0', sizeof(sendBuffer));
  }
}

void NodeWire::configureZigbee()
{

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
  cmd = message->Command;
  i = 0;
  do{
    message->Params[i] = strtok (NULL, " ,");
  } while(message->Params[i++] != NULL && i<31);

  if(message->Params[i-1] == NULL )
    message->Sender = message->Params[i-2];
   else
    message->Sender = strtok (NULL, " ,");
   message->Sender[strlen(message->Sender)-1] = 0;
   i = strlen(message->Sender);
   int j;
   for(j=0;j<i;j++)
       if(message->Sender[j]=='\r' || message->Sender[j]=='\n') break;
   message->Sender[j]=0;
}

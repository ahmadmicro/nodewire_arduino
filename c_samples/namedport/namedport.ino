/*
Copyright (c) 2016, nodewire.org
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
   This product includes software developed by nodewire.org.
4. Neither the name of nodewire.org nor the
   names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY nodewire.org ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL nodewire.org BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <nodewire.h>

NodeWire iot;
int led = 13;
int val = 0;
nString response(new char[50]);

void setup() {
  iot.begin();
  pinMode(led, OUTPUT);
}

void loop() {
  if (iot.messageArrived())
  {
    if(iot.cmd == "set" || iot.cmd == "setportvalue")
    {
        nString port = iot.message->Params[0];
        val = atoi(iot.message->Params[1]);
        if(port == "led")
        {
            digitalWrite(led, val);
            response = "portvalue "; response += port; response += " "; response += nString(val);
        }
        else if(port == "ports")
        {
           response = "error attempt to write to readonly port ";  response += port;
        }
        else
        {
           response = "port "; response += port; response += " not found";
        }
        iot.transmit(iot.message->Sender, response.theBuf);
    }
    else if(iot.cmd == "get" || iot.cmd == "getportvalue")
    {
        nString port = iot.message->Params[0];
        if(port == "led")
        {
           response = "portvalue "; response += port;  response += " "; response += nString(val);
        }
        else if(port == "ports")
        {
           response = "ports led ";
        }
        else
        {
           response = " port ";  response += port; response += " not found ";
        }
        iot.transmit(iot.message->Sender, response.theBuf);
    }
    else
    {
        response = "unknown command ";
        response+=iot.cmd;
        iot.transmit(iot.message->Sender, response.theBuf);
    }

    iot.resetmessage();
  }

}

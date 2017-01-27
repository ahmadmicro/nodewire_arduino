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
#include <node.h>

class myNode: public Node
{
  int led = 13;

  nString val = "blink"; //on, off or blink, reserve enough space for the longest string that will be used

  public:
  void init()
  {
    iot.begin("blinky");
    pinMode(led, OUTPUT);
  }

  bool get(nString port)
  {
       nString response(new char[100]);
       if (port == "led")
       {
          response = "portvalue led "; response +=  val;
       }
       else if(port == "ports")
       {
          response = "ports led ";
       }
       else if(port == "properties")
       {
          response = "properties led LIST on off blink";
       }
       iot.transmit(iot.message->Sender, response);
  }

  void set(nString port)
  {
      nString response(new char[100]);
      if(port == "led")
      {
          val = iot.message->Params[1];
          stopTimer();
          if(val == "on")
              digitalWrite(led, 1);
           else if(val == "off")
              digitalWrite(led, 0);
           else if(val == "blink")
               startTimer(500);
          response = "portvalue led "; response += val;
      }
      iot.transmit(iot.message->Sender, response);
  }

  bool ledon = false;
  void timer()
  {
    if(ledon) digitalWrite(led, 1); else digitalWrite(led, 0);
    ledon = !ledon;
  }
};

void setup() {
   setNode(new myNode());
}

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
#include <NodeWire.h>
#include <board.h>

Board board;
NodeWire iot;

String response;

void setup() {
  iot.begin();
  //iot.iswifi = true;
  board.init();  
}

void loop() {
  if( iot.ack)
      board.checkinputs(iot);
  if (iot.messageArrived())
  {
        if(iot.cmd == "set" || iot.cmd == "setportvalue")
        {
            String port = iot.message->Params[0];
            int portindex  = board.getportindex(port);
            if(portindex!=-1)
            {
               response = board.out(port, iot.message->Params[1]);
            }
            else if(port == "direction")
            {
              //set direction port IN/OUT
               String portname = iot.message->Params[1];
               int portindex  = board.getportindex(portname);
               String direction1 = String(iot.message->Params[2]);
               response = board.setdirection(portname, direction1);
            }
            else
                 response = String("error port ") + port + " is not an output port ";
            iot.transmit(response);
        }
        else if(iot.cmd == "get" || iot.cmd == "getportvalue")
        {
             String port = iot.message->Params[0];
             int portindex  = board.getportindex(port);
             if(portindex != -1)
             {
                response = String("portvalue ") + port + " " + String(board.value[portindex]);
             }
             else if (port == "properties")
             {
                String portname = iot.message->Params[1];
                int portindex  = board.getportindex(portname);
                response = String("properties ") + portname + " " + (board.direction[portindex]==0?board.outputtype(portindex): board.inputtype(portindex)) + (board.direction[portindex]==1?" IN ":" OUT ");
             }
             else if(port == "ports")
             {
               //todo: write a generic version of this
                response = String("ports 2 3 4 5 6 7 8 9 10 11 12 13 A0 A1 A2 A3 A4 A5");
             }
             else
                response = String("error port ") + port + " not found";
             iot.transmit(response);
        }
        else if(iot.cmd == "getnoports")
        {
           response = String("noports 14");
           iot.transmit(response);
        }
        else
        {
             iot.transmit(String("unknown command ") + iot.cmd);
        }
        iot.resetmessage();
  }
}





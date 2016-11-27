/*
Copyright (c) 2016, NodeWire.org.ng
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
   This product includes software developed by NodeWire.org.ng.
4. Neither the name of NodeWire.org.ng nor the
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

#ifndef NODEWIREMQTT_H
#define NODEWIREMQTT_H

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

class Message
{
   public:
    	char* Address;
    	char* Command;
    	char* Params[32];
    	char* Sender;
};

class NodeWire
{
   public:
	   String myAddress = "node01";
     String mybroadcastaddress = "any";
	   int ack = 0;
	   boolean messageComplete = false;
	   Message* message;
	   String cmd;
     bool iswifi = false;
     String remote = "remote";

	   NodeWire()
	   {
		     message = new Message();
     }

	   void resetmessage();
     void begin();
     void begin(char*);
     bool messageArrived();
     void transmit(String);

     //candidates for private
     void announciate();
	   void serialEvent();
	   void SplitCommand();

  private:
  	char buffer[256];
  	int index;
  	long ackcount = 500000;
};

#endif

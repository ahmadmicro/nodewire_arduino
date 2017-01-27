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
#include <board.h>
#include <bnode.h>

class myNode: public bNode
{
public:
  void init()
  {
    iot.begin("node01");
    //iot.iswifi = true;
    board.value = new (double[18]){1,1,1,1,1,  1,1,1,1,1,  1,1,1,1,1,  1,0.5,0.5};//stores the value of the port
    board.direction = new (char[18]){0,0,0,0,0,  0,0,0,0,0,  0,0,0,0,0,  0,0,1};//stores the direction of the port, 1=in, 0=out
    board.ports = new (nString[18]){"2","3","4","5","6","7","8","9","10","11","12","13","A0","A1","A2","A3","A4","A5"};//the port name
    board.address = new (int[18]){2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};//the pin that will be mapped to the port
    board.init(18);
  }
};

Node* thenode = new myNode();

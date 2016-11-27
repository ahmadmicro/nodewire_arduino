Arduino NodeWire library
========
A NodeWire project speaks the PlainTalk protocol and is encapsulated as a node.
The Node class implements the behaviour of a Node so you do not have to implement that yourself.
A node has some built-in behaviours that make it useful out of the box. You can interact with a node using the Arduino serial monitor.

But when deployed, a node will usually interact with other NodeWire components such as the gateway or the dashboard through
communication modules such as GSM modem, wifi, ZigBee or ethernet. In this chapter, we will be using the serial monitor.

To create a Node, all you need to do is create an instance of the Node object. Unlike typical arduino sketches, you do not need to define the
setup and loop functions, as these are already defined inside the node.h file.

.. code-block:: c++

    #include <nodewire.h>
    #include <node.h>

    Node* thenode = new Node();

Compile and upload the above sketch to your Arduino board. Launch the serial monitor and note the annunciation.
Type the following in the serial monitor to stop the annunciation:

.. code-block:: none

  node01 ack cp

To get the PlainTalk version, send:

.. code-block:: none

  node01 version cp

To determine the amount of memory (ram) available:

.. code-block:: none

  node01 get memory cp

Customizing the Node
-----------------
To customize the node, e.g. choosing your own node name instead of the default 'node01', you create a subclass of the Node class:

.. code-block:: c++

  #include <nodewire.h>
  #include <node.h>

  class myNode: public Node
  {
    public:
    void init()
    {
      iot.begin("basic");
    }
  };

  Node* thenode = new myNode();

What we did here is to override the init function of the parent Node class. It is called only once during program initialization.
You can include any other initialization routines you may require here.

The iot object is an instance of the NodeWire class and is defined
in the parent Node class.

The iot.begin function initializes the communication parameters. The parameter "basic" is an optional name that can be passed
and will be used as the node name.


Handling messages
-----------------
Nodes are usually used to control or monitor other devices. While they carryout the function of controlling and monitoring, they must constantly
communicate with the rest of the system. They must be able to respond to commands and report status on demand.

In order to take care of all these, our programming will be event and message driven. There are functions in the Node class that you can override to handle
when messages arrive or when events occur.

Lets start with a simple example. This example echoes any message sent to it.

.. code-block:: c++

  #include <nodewire.h>
  #include <node.h>

  class myNode: public Node
  {
    public:
    void init()
    {
      iot.begin("echo");
    }

    void handleMessage() {
        nString response(new char[100]);
        response = iot.message->Command;
        int i = 0;
        while(iot.message->Params[i+1]!=NULL)
        {
          response+=" ";
          response+=iot.message->Params[i++];
        }
        iot.transmit(iot.message->Sender, response);
    }
  };

  Node* thenode = new myNode();

The handleMessage() function is called whenever a message is received by the node. You can inspect the iot.message structure to know the nature of the message received.
The message structure has the following components:

* Command: contains the keyword or command
* Params: is an array of parameters
* Sender: the address or node name of the sender
* Address: the address or node name of the destination, which will always match our node name.

The iot.transmit(sender, response) function is used to send a message back to the system.
The first parameter is the address of the destination, in this case we're responding, so we send back to sender. And the second parameter is the
body of the response which is usually in the form of keyword (or command) followed by parameters, separated by spaces.

How to use Arduino ports
-----------------
Nodes usually read and write to ports in order to measure or actuate. If you're using Arduino ports, there is a special Node type that has predefined
the Arduino ports functionality. All you have to do is to specify how you want the ports to be named.

The board object (instantiated in the bNode class) is used to handle this definition. It has four arrays that holds the port definition:

1. ports: this is an array of strings and each string hold the name of the port. The port name is what the port will be referred to from PlainTalk.

2. direction: this is an array of characters. a value of 1 means the port is an input port, while 0 means it is an output port. The port is automatically configured accordingly.

3. address: this is the Arduino pin that maps to the port.

4. value: this is the initial value that will be written to the port.

.. code-block:: c++

  #include <nodewire.h>
  #include <bnode.h>

  class myNode: public bNode
  {
    public:
    void init()
    {
      iot.begin("node01");

      board.value = new (double[1]) { 0 };
      board.direction = new (char[1]) { 0 };
      board.ports = new (nString [1]) {"led"};
      board.address = new (int[1]) {13};
      board.init(1);
    }
  };

  Node* thenode = new myNode();

The board.init() function takes one argument, which should match the number of ports in the port definition.

On the serial monitor, try the following commands.

.. code-block:: none

  node01 set led 1 cp

This will switch on the LED on the Arduino board. To switch it off:

.. code-block:: none

  node01 set led 0 cp


To get the current status:


.. code-block:: none

  node01 get led cp

Using all of the Arduino ports
----------------------
The following code initializes all the arduino ports, apart from ports 0 and 1 which are used for serial communication:


.. code-block:: c++

    #include <nodewire.h>
    #include <board.h>
    #include <bnode.h>

    class myNode: public bNode
    {
    public:
      void init()
      {
        iot.begin("node02");
        //iot.iswifi = true;
        board.value = new (double[18]){1,1,1,1,1,  1,1,1,1,1,  1,1,1,1,1,  1,0.5,0.5};//stores the value of the port
        board.direction = new (char[18]){1,1,0,0,0,  0,0,0,0,0,  0,0,0,0,0,  0,0,1};//stores the direction of the port, 1=in, 0=out
        board.ports = new (nString[18]){"2","3","4","5","6","7","8","9","10","11","12","13","A0","A1","A2","A3","A4","A5"};//the port name
        board.address = new (int[18]){2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};//the pin that will be mapped to the port
        board.init(18);
      }
    };

    Node* thenode = new myNode();


You can enumerate the available ports by,

.. code-block:: none

  node02 get ports cp

And you can check the properties of any of the ports with,

.. code-block:: none

  node02 get properties 13 cp

Change a port from input to output or vice versa by,

.. code-block:: none

  node02 set direction 13 in cp



Handling port changes
---------------------
You can define override the function Node.changed, in order to receive notification when any of the input port changes value.
For example, to monitor the analog input pin, A0, and then switch on the LED, pin 13, when the input value is higher than 50%:

.. code-block:: c++

    #include <nodewire.h>
    #include <board.h>
    #include <bnode.h>

    class myNode: public bNode
    {
    public:
      void init()
      {
        iot.begin("node02");
        board.value = new (double[2]){0,0};
        board.direction = new (char[2]){1,0};
        board.ports = new (nString[2]){"A0","led"};
        board.address = new (int[2]){A0,13};
        board.init(2);
      }

      void changed(int portIndex)
      {
        if(board.getValue("A0")>0.5)
            board.out("led", "1");
         else
            board.out("led", "0");
      }
    };

    Node* thenode = new myNode();


The function board.getValue is used to read the value of a port. And the function board.out is used to write to an output port.
When you override a function, make it short and quick in order not the disrupt the message-driven structure of the code. In particular the use of long delay and long loops
should be avoided. As a general rule, the routines should last only a few milli-seconds execution time, including any delays.

Longer delays (and loops) can be implemented by using our event friendly wait function instead. This will be covered below.

The wait function
-----------------
We are going to modify the above example to make the led blink whenever the analog input is higher than 50%.
The led will come on for 1 second and then go off. This requires some kind of delay. In order not to impact the message and event-driven nature
of the system, we have to use the wait function instead of the arduino delay function.

.. code-block:: c++

    #include <nodewire.h>
    #include <board.h>
    #include <bnode.h>

    class myNode: public bNode
    {
    public:
      void init()
      {
        iot.begin("node02");
        board.value = new (double[2]){0,0};
        board.direction = new (char[2]){1,0};
        board.ports = new (nString[2]){"A0","led"};
        board.address = new (int[2]){A0,13};
        board.init(2);

      }

      void changed(int portIndex)
      {
        if(board.getValue("A0")>0.5)
        {
            board.out("led", "1");
            wait(1000);
            board.out("led", "0");
            wait(1000);
        }
      }
    };

    Node* thenode = new myNode();


Fine-tuning the sensitivity
----------
The system rotates through the input ports in 200ms intervals (by default). And if any input value has changed by a predefined amount, that change triggers
the changed event and also PlainTalk notification.

The interval period can be reduced or increased to make the system detect changes faster or slower by changing the board.checkInterval value.
Similarly the resolution can be changed by changing the value of board.checkResolution (default value is 0.25). All these should be in the init function.

The loop function
---------
You can monitor the ports directly by overriding the loop function.

.. code-block:: c++

    #include <nodewire.h>
    #include <board.h>
    #include <bnode.h>

    class myNode: public bNode
    {
        public:
        void init()
        {
          iot.begin("node02");
          board.value = new (double[2]){0,0};
          board.direction = new (char[2]){1,0};
          board.ports = new (nString[2]){"A0","led"};
          board.address = new (int[2]){A0,13};
          board.init(2);

        }

        void loop()
        {
          bNode::loop();
          if(board.getValue("A0")>0.5)
              board.out("led", "1");
           else
              board.out("led", "0");
        }
    };

    Node* thenode = new myNode();

You can remove the bNode::loop(); line to switch-off the PlainTalk notification for input port changes.

The loop function should only be used when absolutely necessary, because of it's potential impact on system performance.
And when used, it should be as short and quick as possible. No delays should be used and even the wait function is not supported
within the loop function.


How to define virtual ports
-------------------------
You can define a port that doesn't physically map to any Arduino pin or maps to a pin but takes other values than 1 and 0.
In order to do that, you have to override two functions; the get and set functions.

Here is an example that remakes the led example but this time, the port takes one of three possible values: on, off or blink.

.. code-block:: c++

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

      void get(nString port)
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

    Node* thenode = new myNode();

To switch on the led we send:

.. code-block:: none

  blinky set led on cp

To switch it off:

.. code-block:: none

  blinky set led off cp

and when we send:

.. code-block:: none

  blinky set led blink cp

it blinks continously in one second intervals.

The timer function is explained in the next section.


Pseudo-threading
----------------------
The timer function is called periodically whenever the timer is enabled. It is enabled by calling the startTimer function. The parameter of the function
is the interval between calls to the timer function in milliseconds. To disable the timer, call the stopTimer function.

The timer function can be used as (if it is) a second thread. Here is an example that implements 2 separate counter threads:

.. code-block:: c++

    #include <nodewire.h>
    #include <node.h>

    class myNode: public Node
    {
      public:
      void init()
      {
        iot.begin("threading");
        startTimer(1000);
        for(int i=0; i<=100;i++)
        {
            wait(1000);
            Serial.println(i);
        }
      }

      int j = 100;
      void timer()
      {
          if(j==200) stopTimer();
           Serial.println(j++);
      }
    };

    Node* thenode = new myNode();


The wait function cannot be used in two separate threads simultaneously, otherwise, one thread will block until the other one exits.



nString
---------
NodeWire uses a lot of strings for its logic. In order to help handle the string manipulation easily in a memory safe way, we created the nString class.
nString is a class that points to a normal c string (pointer to chars) and then provide high level functions for appending, copying, and comparison operations.

Declaration: you can declare an nString and then point it to a predefined string buffer. No new memory is created in the process.

.. code-block:: c++

  char buffer[100];
  nString greeting(buffer);

You can also allocate the buffer on the fly:

.. code-block:: c++

  nString greeting(new char[100]);

or

.. code-block:: c++

  nString greeting("hello world");


Copying:
You can copy into a string by using the assignment operation. No new memory is allocated in this process unless if the nString has not been assigned a buffer.
In that case, the right hand side will become the buffer. But this might prevent you from further concatenation since the buffer would be already full.

.. code-block:: c++

   char buffer[100];
   nString greeting(buffer);

   greeting = "hello world";

this copies the characters "hello world" to the buffer associated with the nString.

Appending: To append a string to an existing string, use the + operator:


.. code-block:: c++

   char buffer[100];
   nString greeting(buffer);

   greeting = "hello ";
   greeting = greeting + "world";

or

.. code-block:: c++

   char buffer[100];
   nString greeting(buffer);

   greeting = "hello ";
   greeting += "world";

Comparison:
You can make string comparisons:

.. code-block:: c++

   if(greeting == "hello") Serial.println("correct");

Node that these functions does not check for buffer overflow and it is up to the programmer to make sure that there is enough space in the buffer to handle the operations.

When appending, (+) operator, always make sure that the leftmost variable on the right hand side of the equation has enough space in its buffer to hold the result.


Accessing the buffer:
You can access the buffer by making reference to the original c ctring (char poointer) or by using the nString's internal reference:

.. code-block:: c++

    char buffer[100];
    nString greeting(buffer);

    greeting = "hello ";
    greeting += "world";

    Serial.println(buffer);
    Serial.println(greeting.theBuf);

Tutorial
===============

Minimal Node
--------------
At a minimum we must define a node object which specifies the interface of out node and then a link object
which specifies how our node will communicate with the outside world.

This example is functional but actually does nothing as we have not defined any input or output ports.

.. code-block:: c++

    #include <lnode.h>
    #include <nseriallink.h>

    #define LED LED_BUILTIN

    Node<int> node;
    SerialLink lnk;

    void setup() {
        Serial.begin(38400);
        lnk.begin(&Serial);
        node.init("node##", &lnk);
    }

    void loop() {
        node.run();
    }

Ports
---------------
The ports are defined in this example. But we are not going to specify how these ports
affect the behaviour of the node.

Note that you will be able to set and read the input port values. But the output port will not be readable.

.. code-block:: c++

    #include <lnode.h>
    #include <nseriallink.h>

    #define LED LED_BUILTIN

    Node<int> node;
    SerialLink lnk;

    void setup() {
        Serial.begin(38400);
        lnk.begin(&Serial);

        node.inputs = "1 2";
        node.outputs = "3";
        node.init("node##", &lnk);
    }

    void loop() {
        node.run();
    }

Readable Output port
--------------------
In order for an output port to be readable, you must provide a source for its value.
This can be a variable, an expression or a function call. All you have to do is enclose the source
in the "outputValue" macro.

,, code-block:: c++


    #include <lnode.h>
    #include <nseriallink.h>

    #define LED LED_BUILTIN

    Node<int> node;
    SerialLink lnk;

    void setup() {
        Serial.begin(38400);
        lnk.begin(&Serial);

        node.inputs = "1 2";
        node.outputs = "3";
        node.init("node##", &lnk);

        node["3"] << outputValue(analogRead(A0));
    }

    void loop() {
        node.run();
    }


Monitoring and Reporting changes
------------------
You will notice that the output port does not transmit changes in the output value 
unless it is requested. In order to transmit the value whenever it changes, we will
have to poll the value and send then transmit it whenever we notice any significant change

Polling is done by defining a timer callback. NodeWire supports 3 timers, 0, 1, and 2.
The timer function takes 2 arguments, the first is the interval in milliseconds and the second is
the callback function that will be called eachtime the interval elapses.

You have to start the timer before it will become active.

.. code-block:: c++

    #include <lnode.h>
    #include <nseriallink.h>

    #define LED LED_BUILTIN

    Node<int> node;
    SerialLink lnk;

    void setup() {
        Serial.begin(38400);
        lnk.begin(&Serial);

        node.inputs = "1 2";
        node.outputs = "3";
        node.init("node##", &lnk);

        node["3"] << outputValue(analogRead(A0));

        node.timer(1000, [](){
            node["3"] = analogRead(A0);
        });

        node.startTimer(0);
    }

    void loop() {
        node.run();
    }

Note how you can cange an output port by assigning a new value to the port, thus

.. code-block:: c++

     node["3"] = analogRead(A0);


Detecting and responding to inputs
---------------------------------
**Method 1: Reading the input port**

.. code-block:: c++

    #include <nnode.h>
    #include <nseriallink.h>

    #define LED LED_BUILTIN

    Node<int> node;
    SerialLink lnk;

    void setup() {
        Serial.begin(38400);
        lnk.begin(&Serial);

        node.inputs = "start";
        node.outputs = "value";
        node.init("node##", &lnk);

        node["value"] << outputValue(analogRead(A0));

        node.timer(1000, [](){
            if(node["start"]==1)
                node["value"] = analogRead(A0);
        });

        node.startTimer(0);
    }

    void loop() {
        node.run();
    }


**Method 2: Use a lambda function**

.. code-block:: c++

    #include <nnode.h>
    #include <nseriallink.h>

    #define LED LED_BUILTIN

    Node<int> node;
    SerialLink lnk;

    void setup() {
        Serial.begin(38400);
        lnk.begin(&Serial);

        node.inputs = "start";
        node.outputs = "value";
        node.init("node##", &lnk);

        node["start"] >> [](nString val, nString sender) {
            if(val==1)
                node.startTimer(0);
            else
                node.stopTimer(0);
        };
        
        node["value"] << outputValue(analogRead(A0));

        node.timer(1000, [](){
            if(node["start"]==1)
                node["value"] = analogRead(A0);
        });
    }

    void loop() {
        node.run();
    }
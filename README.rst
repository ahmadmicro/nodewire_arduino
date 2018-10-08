Arduino NodeWire library
========
The Arduino NodeWire Library version 1.0 enables you to create IoT nodes using any Arduino compatible board and it has two main parts:

1. The Node, which specifies the input and output ports
2. the Link, which specifies the medium that the node uses to communicate with the outside world.

A node encapsulates the behavior of a connected device. The input and output ports is how you talk to a node. You write values to the input ports
and read values from the output port. Your code must have functions that will provide a value for when each of the output ports are read
and functions that will consume the value provided when any of the input ports are written to.

.. code-block:: c++

    Node<int> node; // store port values as int
    node.inputs = "led"; // list of input portnames separated by space
    node.outputs = "button"; // list of output portnames separated by space

    node.init("nodename"); // create the node and its ports

    node.on("button",
       []() -> nString {
          return digitalRead(BUTTON);
       }
    );

    node.on("led",
      [](nString val, nString sender) {
           digitalWrite(LED_BUILTIN,(int)val);
       }
    );

This is the core functionality of a node. But a node cannot work in isolation. It has to be able to talk to the outside world. It is the link class
that provides this connectivity. And there are many types of links: SerialLink, esp8266link, ethernetlink, gprslink, nrflink etc, depending
on how you want your node to be connected.

You create the link object and associate it with the node during setup:

.. code-block:: c++

    Node<int> node;
    SerialLink link;

    Serial.begin(38400);
    link.setSerial(&Serial);

    node.begin(link);

And here is the complete code:

.. code-block:: c++

    #include <nnode.h>
    #include <nseriallink.h>

    #define LED LED_BUILTIN

    Node<int> node;
    SerialLink link;

    void setup() {
      Serial.begin(38400);
      link.setSerial(&Serial);

      node.inputs = "led";
      node.init("node01");
      node.begin(&link);

      node.on("led",
         [](nString val, nString sender) {
            digitalWrite(LED,(int)val);
         }
      );

      pinMode(LED, OUTPUT);
    }

    void loop() {
      node.run();
    }

The serial link enables you to interact with a node using PlainTalk over the Serial monitor. The node can also connect to the cloud through a gateway that has a Serial interface
such as an ESP8266-based one or a python node running on Raspberry-pi or a PC.

Compile and upload the above sketch to your Arduino board. Launch the serial monitor and note the annunciation.
Type the following in the serial monitor to stop the annunciation:

.. code-block:: none

  node01 ack cp

Make sure you select to send 'Newline' on the drop down at the bottom of the serial monitor window. Also make sure the selected baud rate
is 38400.

.. image:: images/sm.png


To determine the amount of memory (ram) available:

.. code-block:: none

  node01 get memory cp


How to talk to ports
-----------------

On the serial monitor, try the following commands.

.. code-block:: none

  node01 set led 1 cp

This will switch on the LED on the Arduino board. To switch it off:

.. code-block:: none

  node01 set led 0 cp


To get the current status:


.. code-block:: none

  node01 get led cp


You can enumerate the available ports by,

.. code-block:: none

  node01 get ports cp



How to define ports that takes non integer value
-------------------------
The values you read and write to a port does not have to be a numerical value. It can be any kind of value: an integer, a float,
a string, and array or an object. All these are values that can be represented by nString.

Here is an example that remakes the led example but this time, the port takes one of three possible values: on, off or blink.

.. code-block:: c++

    #include <nnode.h>
    #include <nseriallink.h>

    #define LED LED_BUILTIN

    Node<nString> node;
    SerialLink link;

    bool ledon = false;

    void setup() {
      Serial.begin(38400);
      link.setSerial(&Serial);

      node.inputs = "led";
      node.init("blinky");
      node.begin(&link);

      node.on("led",
         [](nString val, nString sender) {
             node.stopTimer(0);
             if(val=="on")
                 digitalWrite(LED,1);
             else if(val=="off")
                 digitalWrite(LED,0);
             else if(val=="blink")
                 node.startTimer(0);
         }
      );


      node.on_timer(500,
         []() {
              if(ledon) digitalWrite(LED, 1); else digitalWrite(LED, 0);
                 ledon = !ledon;
         }
      );

      pinMode(LED, OUTPUT);
    }

    void loop() {
      node.run();
    }


To switch on the led we send:

.. code-block:: none

  blinky set led on cp

To switch it off:

.. code-block:: none

  blinky set led off cp

and when we send:

.. code-block:: none

  blinky set led blink cp

it blinks continously in one second intervals, 500ms on and 500ms off.

The timer function is used to schedule periodic events. You can define up to three timers: 0, 1 and 3, numbered in order of creation.

Multiple port example
----------------

.. code-block:: c++

  #include <nnode.h>
  #include <nseriallink.h>

  #define MAINS 1
  #define IGNITION LED_BUILTIN
  #define BUZZER 3

  Node<int> node;
  SerialLink link;

  void setup() {
    Serial.begin(38400);
    link.setSerial(&Serial);

    node.outputs = "mains gen";
    node.inputs = "ignition buzzer relay";

    node.init("sco");
    node.begin(&link);

    node.on("ignition",
      [](nString val, nString sender) {
         digitalWrite(IGNITION,(int)val);
       }
    );

    node.on("buzzer",
      [](nString val, nString sender) {
         digitalWrite(BUZZER,(int)val);
       }
    );

    node.on_timer(1000,
      []() {
         Serial.println("tick");
       }
    );

    node.on_timer(500,
      []() {
         Serial.println("tock");
       }
    );

    node.on("mains",
      []() -> nString {
          return digitalRead(MAINS);
       }
    );

    pinMode(IGNITION, OUTPUT);
    node.startTimer(0);
    node.startTimer(1);
  }

  void loop() {
    node.run();
  }



nString
========
NodeWire uses a lot of strings for its logic. In order to help handle the string manipulation easily in a memory safe way, we created the nString class.
nString is a class that points to a normal c string (pointer to chars) and then provide high level functions for appending, copying, and comparison operations.
It can also convert between string and other types such as int and float.

nString can also be used to store and manipulate arrays and dictionaries and can be used to encode and decode JSON objects.

nString requires a string buffer (char*). And this can be created externally, in which case managing the buffer is the responsibility of
the programmer.  Or it can be created internally by nString, in which case the string is managed by the class. A managed string can automatically
expand to accommodate longer strings during assignment, concatenation and other operations.

Creating nString
------------------

.. code-block:: c++

  char buffer[100];
  nString greeting(buffer, 100);


or


.. code-block:: c++

  char buffer[100];
  nString greeting;
  greeting.setBuffer(buffer, 100);


This creates a user managed string.

avoid this:

.. code-block:: c++

  nString greeting(new char[100], 100);

or this:

.. code-block:: c++

  nString greeting(new char[100]);


But you can do this:

.. code-block:: c++

  nString greeting("hello world");

and this

.. code-block:: c++

  nString greeting = "hello world";

Copying
-------
You can copy into a string by using the assignment operation.
No new memory is allocated in this process unless if the nString has not previously been assigned a buffer.


.. code-block:: c++

   #include <nstring2.h>
   void setup()
   {
     Serial.begin(38400);
     char buffer[100];
     nString greeting(buffer, sizeof(buffer));

     greeting = "hello world";
     greeting.println(&Serial);
   }
   void loop()
   {
   }

this copies the characters "hello world" to the buffer associated with the nString.

Appending
----------
To append a string to an existing string, use the + operator:


.. code-block:: c++

   char buffer[100];
   nString greeting(buffer, sizeof(buffer));

   greeting = "hello ";
   greeting = greeting + "world";

or

.. code-block:: c++

   char buffer[100];
   nString greeting(buffer, sizeof(buffer));

   greeting = "hello ";
   greeting += "world";

it is also possible to append to an nString without first associating it with a buffer.

.. code-block:: c++

   nString greeting;

   greeting = "hello ";
   greeting += "world";

But in this case, nString will dynamically allocate memory and reallocate as needed tp accommodate the characters. The buffer is
managed by nString and it will be deleted by nString's destructor.


Comparison
-------------
You can make string comparisons:

.. code-block:: c++

   if(greeting == "hello") Serial.println("correct");

Note that the nString will truncate the result if the buffer is user allocated otherwise the buffer will grow to accommodate the result..

When appending, (+) operator, always make sure that the leftmost variable on the right hand side of the equation has enough space in its buffer to hold the result.


Accessing the buffer
--------------------
You can access the buffer by making reference to the original c string (char pointer) or by using the nString's internal reference:

.. code-block:: c++

    char buffer[100];
    nString greeting(buffer, sizeof(buffer));

    greeting = "hello ";
    greeting += "world";

    Serial.println(buffer);
    Serial.println(greeting.theBuf);
    Serial.println((char*)greeting);

The last 3 lines are all equivalent

Using nString as an Array
--------------------------
You can convert an nString to an array either to splitting the string in its buffer or by explicitly calling the create_array member function.


.. code-block:: c++

    char buffer[100];
    nString greeting(buffer, sizeof(buffer));

    greeting = "these will be the elements";
    greeting.split(' ');

    greeting[1] = "wont"; // change item 1

    for(int i=0; i<greeting.len; i++)
    {
      Serial.println(greeting[i].theBuf);
    }

    greeting.join(' '); // convert back to string
    Serial.println(greeting.theBuf);

The split function does not create any new buffer. It splits the buffer of the parent nString among the elements, each according to its size.
The last element will take whatever is left.
Note that while you can modify the elements of the array, the size of the elements are limited to their initial sizes and assigning any string longer than
that will result in truncation.

The join function converts an array back to string using the character specified to join them.

The second method for creating an Array is by calling the create_array member function and passing the number of elements as a parameter. This requires that the nString already has a buffer.
The buffer is split equally into the elements of the array created.

.. code-block:: c++

    char buffer[100];
    nString greeting(buffer, sizeof(buffer));
    greeting.create_array(10);
    greeting.append("one");
    greeting.append("two");
    greeting.append("three");
    greeting.append("four");

    greeting.pop();

    for(int i=0; i<greeting.len; i++)
    {
      Serial.println(greeting[i].theBuf);
    }

    greeting[1]  = "twenty";
    greeting.append("five");

    int third = greeting.find("three");
    if(third!=-1)
      greeting[third] = "Thirty";

    greeting.join(' ');
    Serial.println(greeting.theBuf);


Using nString as Object
--------------
There are two ways to create an nString object:

1. First create an Array. Then call the convert_object member function to convert it to an object

2. Call the create_object member function.

Both functions text a string which lists the fields of the new object, separated by space.

Example:

.. code-block:: c++

    char buffer[100];
    nString obj(buffer, sizeof(buffer));
    obj.create_object("name age department");
    obj["name"] = "Ahmad";
    obj["age"] = 35;
    obj["department"] = "software";

    obj.println(&Serial);

This can also be achieved by starting with an array:

.. code-block:: c++

    char buffer[100];
    nString obj(buffer, sizeof(buffer));
    obj = "Ahmad 35 software";
    obj.split(' ');
    obj.convert_object("name age department");

    obj.println(&Serial);

Decoding and Encoding Json
------------------
nString can be used to decode (parse) and encode (dump) json objects.

**Decoding**

To create an nString object from a json string,

.. code-block:: c++

    nString obj = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";
    obj.parse_as_json();
    Serial.println((char*)obj["sensor"]);
    Serial.println((long)obj["time"]);
    Serial.println((char*)obj["data"][0]);
    Serial.println((char*)obj["data"][1]);


**Encoding**

To convert an nString Object to a JSON string:

.. code-block:: c++

    char buff[50];//where we will store the json string
    nString list = "one two three four";
    list.split(' '); //create array
    list.dump_json(buff);//convert to json
    Serial.println(buff);


**Parsing PlainTalk**

.. code-block:: c++

    Serial.begin(38400);
    nString dval = "node01 set person {\"name\": \"ahmad\", \"age\": 40} lrsnr49yxurz:re";
    dval.splitPT(' ');
    dval.convert_object("address command port value sender");
    dval["value"].println(&Serial);
    dval["value"].parse_as_json();
    dval["value"].println(&Serial);



EEPROM files
====================
This is a crude file system that allows you to store files in EEPROMs.
It is dependent on Arduino's EEPROM library.

Before you can store files, you must file create the directory:


.. code-block:: c++

  #include <nEEPROMFile.h>

  void setup() {
    Serial.begin(38400);
    EEPROM_File file;
    if(file.no_files()==-1)
    {
      Serial.println("Creating file system");
      file.create_FS(4);
    }

    char buffer[100];
    nString story(buffer, sizeof(buffer));

    if(!file.open("story.txt", story))
    {
      Serial.println("creating file");
      file.create_file("story.txt", 100);
      story = "cowards die many times before their death.";
      file.save("story.txt", story);
    }

    Serial.println(buffer);
  }

  void loop() {

  }

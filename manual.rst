Reference Manual
===================

nString
=========
nString can hold the following types: int/long, float/double, string, dictionary and list

**Numbers**

.. code-block:: c++

    nString num = 1;
    int n = (int) num; //convert to int
    num = 9.7;
    double d = (double) num;

**Strings**

.. code-block:: c++

    nString greeting = "hello";
    greeting += " World";

the internal buffer automatically grows to accomodate the longer string.
When the internal buffer is auto-allocated, nString takes care of releasing it when it is no longer needed.

**Fixed Buffer**
When you do not want unexpected memory allocation, you can pre-assign an existing buffer and the nString will
limit itself to the buffer provided:

.. code-block:: c++

    char buffer[12];
    nString greeting(buffer, 12);
    greeting = "hello";
    greeting += " World";

Or

.. code-block:: c++

    nString greeting;
    greeting.createBuffer(12);
    greeting = "hello";
    greeting += " World";

In this case the concatenation will only be carried out if there is enough space in the buffer.
You are responsible for releasing any buffer that you create, but make sure it is no longer in use.

**Check buffer size**

.. code-block:: c++

    nString greeting = "hello";
    int len = greeting.size;

**Access Buffer**

.. code-block:: c++

    nString greeting = "hello";
    char* buff = greeting.theBuf;

Arrays
----------

Arrays must have a fixed buffer size, either by assigning a value during declaration:

.. code-block:: c++

    nString list = "['one', 2, 'Abuja, FCT', 4.5]";
    list.parse_as_json();
    for(int i = 0; i<list.len; i++)
        Serial.println((char*)list[i]);

Or by manually setting the buffer:

.. code-block:: c++

    char buffer[100];
    nString greeting(buffer, sizeof(buffer));
    greeting.create_array(5);
    greeting.append("one");
    greeting.append("two");
    nString obj = "{name:'Ahmad', numbers:[40,'twenty']}";
    obj.parse_as_json();
    greeting.append(obj);
    greeting.append("four");

    greeting.println(&Serial);

Objects
-----------------

EEPROMFile
------------

Data File
-----------


Link
-------


Node
-------

Remote
---------


Database
----------

Webserver
-----------

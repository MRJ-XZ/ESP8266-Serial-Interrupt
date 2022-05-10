## This is a guide to setup serial interrupt on ESP8266. code is ready-to-use; but if there is a need for more information, proceed.
-Arduino framework is one of the frameworks which are used to develop ESP8266 projects and it gives a feature to read data from rx/tx or UART(microUSB cable).
By using Serial object which is part of Arduino, you can read and write data; but you always need to check for available data on serial and then reading it;
This is not a very suitable way to read recieved data for more complex projects. ESP8266 has a specific header file **"uart_register.h"** which gives access 
to UART registers(interrupts/read/write). Using this feature gives advantage of synchronous data update.

### Step 1: Setting up interrupts
first, you need to disable all uart interrupts using: 

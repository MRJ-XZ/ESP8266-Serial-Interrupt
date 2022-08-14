## Setup serial interrupt on ESP8266.
Arduino framework is one of the frameworks which are used to develop ESP8266 projects and it gives a feature to read data from rx/tx or microUSB port.
By using Serial object which is part of Arduino, you can read and write data; but you always need to check for available data on serial and then read it;
This is not a very suitable way to receive and process data for more complex projects. ESP8266 has a specific header file **"uart_register.h"** which gives access 
to UART registers(interrupts/read/write). Using this feature gives advantage of receiving and processing data anywhere in the program **without using Serial.read(), Serial.readbytesuntil(), etc. functions.**

An example code is provided. For receiving a better concept of UART interrupt configuration, please refer to ###Wiki. 
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    


## This is a guide to setup serial interrupt on ESP8266. code is ready-to-use; but if there is a need for more information, proceed.
-Arduino framework is one of the frameworks which are used to develop ESP8266 projects and it gives a feature to read data from rx/tx or UART(through microUSB cable).
By using Serial object which is part of Arduino, you can read and write data; but you always need to check for available data on serial and then read it;
This is not a very suitable way to read recieved data for more complex projects. ESP8266 has a specific header file **"uart_register.h"** which gives access 
to UART registers(interrupts/read/write). Using this feature gives advantage of synchronous data update.

### Step 1: Setting up interrupts
first, create a function to do settings in it. create another function as interrupt handler. 
in setting function, you need to disable all uart interrupts using: 

    ETS_UART_INTR_DISABLE();
then you have to attach the handler function to interrupts:

    ETS_UART_INTR_ATTACH(<your handler function name>, <arguments of handler function>);
from this point settings change by interrupt type **( full buffer / timeout / overflow / empty tx / error detection )**.
This guide only covers the more applicable interrupts. For more information about interrupts, please refer to [ESP8266 technical reference](https://www.espressif.com/sites/default/files/documentation/esp8266-technical_reference_en.pdf).

### Only Full buffer
set interrupt mask:

    SET_PERI_REG_MASK(UART_INT_ENA(0), UART_RXFIFO_FULL_INT_ENA);    
setup interrupt threshold:

    WRITE_PERI_REG(UART_CONF1(0) , (<max length of buffer> & UART_RXFIFO_FULL_THRHD) << UART_RXFIFO_FULL_THRHD_S);
e.g. if max length of buffer is 0x14 (20 in decimal), after recieving 20 bytes, the interrupt will be activated.

Call these functions to clear other interrupts:

    WRITE_PERI_REG(UART_INT_CLR(0), 0xffff);
Now enable uart interrupts:

    ETS_UART_INTR_ENABLE();
### Only Timeout
set interrupt mask:

    SET_PERI_REG_MASK(UART_INT_ENA(0), UART_RXFIFO_TOUT_INT_ENA); 
setup interrupt threshold:    
        
    WRITE_PERI_REG(UART_CONF1(0), UART_RX_TOUT_EN | ((<number of bits> & UART_RX_TOUT_THRHD) << UART_RX_TOUT_THRHD_S));
> Notice: time is measured by **number of bits / baudrate speed**. e.g. 2 bits with buadrate of 9600 results in 1/4800 s timeout.

Call these functions to clear other interrupts:

    WRITE_PERI_REG(UART_INT_CLR(0), 0xffff);
    CLEAR_PERI_REG_MASK(UART_INT_ENA(0), UART_RXFIFO_FULL_INT_ENA);
> Notice: It is mandatory to clear RX full interrupt. full buffer interrupt is enabled by default.

Now enable uart interrupts:

    ETS_UART_INTR_ENABLE();    
### Only TX empty
set interrupt mask:

    SET_PERI_REG_MASK(UART_INT_ENA(0), UART_TXFIFO_EMPTY_INT_ENA); 
setup interrupt threshold:    
        
    WRITE_PERI_REG(UART_CONF1(0) , (<threshold of buffer> & UART_TXFIFO_EMPTY_THRHD) << UART_TXFIFO_EMPTY_THRHD_S);
e.g. ESP should write some data on serial; transmission speed is much slower than operating speed; so it can do other stuff until tx is empty and ready for new data to be written.      

clear interrupts:

    WRITE_PERI_REG(UART_INT_CLR(0), 0xffff);
and enable interrupts:
    
    ETS_UART_INTR_ENABLE();
### Overflow
set interrupt mask:
    
    SET_PERI_REG_MASK(UART_INT_ENA(0), UART_RXFIFO_OVF_INT_ENA);
clear interrupts:

    WRITE_PERI_REG(UART_INT_CLR(0), 0xffff);
and enable interrupts:
    
    ETS_UART_INTR_ENABLE();
    
### Setting more than 1 interrupt type
use **BITWISE |** to set multiple interrupts:
    
    SET_PERI_REG_MASK(UART_INT_ENA(0), UART_RXFIFO_TOUT_INT_ENA | UART_TXFIFO_EMPTY_INT_ENA);
setup interrupts threshold: 
        
    WRITE_PERI_REG(UART_CONF1(0) , UART_RX_TOUT_EN | ((<number of bits> & UART_RX_TOUT_THRHD) << UART_RX_TOUT_THRHD_S) | (<threshold of buffer> &     UART_TXFIFO_EMPTY_THRHD) << UART_TXFIFO_EMPTY_THRHD_S);
clear interrupts:
    
    WRITE_PERI_REG(UART_INT_CLR(0), 0xffff);
clear full buffer interrupt if you're not using it:

    CLEAR_PERI_REG_MASK(UART_INT_ENA(0), UART_RXFIFO_FULL_INT_ENA);
enable interrupts:

    ETS_UART_INTR_ENABLE();

### Step 2: Writing interrupt handler

at top of function, read the cause of interrupt:

    uint32_t uart_intr_status = READ_PERI_REG(UART_INT_ST(0));
checking the type of interrupt:

    while (uart_intr_status != 0x0)
    {
      if (UART_RXFIFO_TOUT_INT_ST == (uart_intr_status & UART_RXFIFO_TOUT_INT_ST))
      {
        //...
      }
      else if(UART_RXFIFO_FULL_INT_ST == (uart_intr_status & UART_RXFIFO_FULL_INT_ST))
      {
        //...
      }
    }
reading length of recieved buffer:
    
    uint8_t fifo_len = (READ_PERI_REG(UART_STATUS(uart_no)) >> UART_RXFIFO_CNT_S) & UART_RXFIFO_CNT;
reading data from buffer byte by byte;
    
    char buffer[fifo_len + 1] = {0};
    for(int i=0; i < fifo_len; i++)
      buffer[i] = READ_PERI_REG(UART_FIFO(0)) & 0xFF;
    buffer[fifo_len] = '\0';
after processing the recieved data, interrupt status should be cleared:
    
    WRITE_PERI_REG(UART_INT_CLR(0), UART_RXFIFO_TOUT_INT_CLR);
    WRITE_PERI_REG(UART_INT_CLR(0), UART_RXFIFO_FULL_INT_CLR);
at last, it should look like this:
    
    uint32_t uart_intr_status = READ_PERI_REG(UART_INT_ST(0));
    while (uart_intr_status != 0x0)
    {
      if (UART_RXFIFO_TOUT_INT_ST == (uart_intr_status & UART_RXFIFO_TOUT_INT_ST))
      {
        uint8_t fifo_len = (READ_PERI_REG(UART_STATUS(uart_no)) >> UART_RXFIFO_CNT_S) & UART_RXFIFO_CNT;
        char buffer[fifo_len + 1] = {0};
        for(int i=0; i < fifo_len; i++)
          buffer[i] = READ_PERI_REG(UART_FIFO(0)) & 0xFF;
        buffer[fifo_len] = '\0';
        //do something with recieved data...
      }
      else if(UART_RXFIFO_FULL_INT_ST == (uart_intr_status & UART_RXFIFO_FULL_INT_ST))
      {
        uint8_t fifo_len = (READ_PERI_REG(UART_STATUS(uart_no)) >> UART_RXFIFO_CNT_S) & UART_RXFIFO_CNT;
        char buffer[fifo_len + 1] = {0};
        for(int i=0; i < fifo_len; i++)
          buffer[i] = READ_PERI_REG(UART_FIFO(0)) & 0xFF;
        buffer[fifo_len] = '\0';
        //do something with recieved data...
      }
      //checking other interrupt types if enabled...
      WRITE_PERI_REG(UART_INT_CLR(0), UART_RXFIFO_TOUT_INT_CLR);
      WRITE_PERI_REG(UART_INT_CLR(0), UART_RXFIFO_FULL_INT_CLR);
      //clearing other interrupts as well if enabled...
      uart_intr_status = READ_PERI_REG(UART_INT_ST(0)); //reading new interrupts
    }
 > Notes: 
 > 
 > All or part of unprocessed recieved data will be **lost** by the next interrupt.
 > 
 > Interrupt handler must be as fast as possible. Doing complex instructions or using delay(), causes instablity. 
 > 
 > RX full buffer usually does not occur if timeout interrupt is active.
 > 
 > When using RX full buffer, these is no need to read the length of recieved buffer; It only occurs when certain number of bytes that we determined in threshold, is   recieved.
 
Any comments or suggestions are welcome.


    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    

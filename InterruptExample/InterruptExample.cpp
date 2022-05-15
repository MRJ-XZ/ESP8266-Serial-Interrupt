#include <Arduino.h>
#include "uart_register.h"

void Settings();
void Interrupthandler(void*);
uint32_t interrupt_status = 0 , recieved = 0 , buffer_length = 0;

void setup() {
  Serial.begin(9600);
  Settings();
  pinMode(LED_BUILTIN , OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN , HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN , LOW);
  delay(500);
  Serial.println(recieved);
}
void Settings()
{
  ETS_UART_INTR_DISABLE();
  ETS_UART_INTR_ATTACH(Interrupthandler, NULL);
  SET_PERI_REG_MASK(UART_INT_ENA(0), UART_RXFIFO_TOUT_INT_ENA );
  WRITE_PERI_REG(UART_CONF1(0), UART_RX_TOUT_EN | ((0x2 & UART_RX_TOUT_THRHD) << UART_RX_TOUT_THRHD_S));
  WRITE_PERI_REG(UART_INT_CLR(0), 0xffff);
  CLEAR_PERI_REG_MASK(UART_INT_ENA(0), UART_RXFIFO_FULL_INT_ENA);
  ETS_UART_INTR_ENABLE();
}
void Interrupthandler(void*)
{
  interrupt_status = READ_PERI_REG(UART_INT_ST(UART0)); //#define UART0 0
  while(interrupt_status != 0)
  {
    if(UART_RXFIFO_TOUT_INT_ST == (interrupt_status & UART_RXFIFO_TOUT_INT_ST))
    {
      buffer_length = (READ_PERI_REG(UART_STATUS(UART0)) >> UART_RXFIFO_CNT_S) & UART_RXFIFO_CNT; // read rf fifo length
      char buffer[buffer_length];
      for(int i =0; i < buffer_length;i++)
        buffer[i] = READ_PERI_REG(UART_FIFO(UART0)) & 0xFF;
      recieved = atoi(buffer);
      WRITE_PERI_REG(UART_INT_CLR(UART0), UART_RXFIFO_TOUT_INT_CLR);
    }
    //else if....
    interrupt_status = READ_PERI_REG(UART_INT_ST(UART0)); 
  }
}
